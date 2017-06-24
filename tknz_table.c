#include "tknz_table.h"
#include "token_defs.h"

transfer_table_t tknzr_table;
int tknzr_entry_counters[MAX_TRANSFER_ENTRIES];


void clear_tknzr_table(void)
{
  memset(tknzr_entry_counters,0,sizeof tknzr_entry_counters);
}

static
void init_identifier(void)
{
  add_initial(TK_IDENTIFIER_BEGIN,CHAR_CLASS_IDENTIFIER_BEGIN);
  add_selfloop (TK_IDENTIFIER_BEGIN, CHAR_CLASS_IDENTIFIER_PART);
  add_accepted_rev(TK_IDENTIFIER_BEGIN, TK_IDENTIFIER_END, CHAR_CLASS_IDENTIFIER_PART);
  set_len_type_row(TK_IDENTIFIER_BEGIN,TFE_VARLEN);
  set_len_type(TK_INIT, TK_IDENTIFIER_BEGIN,TFE_VARLEN);
}


static
void init_punctuation(void)
{
  add_initial(TK_PUNCTUATION_BEGIN,CHAR_CLASS_PUNCTUATION);
  add_accepted_rev (TK_PERIOD,TK_PERIOD_END,CHAR_CLASS_DEC_PART);
  add_accepted_rev(TK_PUNCTUATION_BEGIN, TK_PUNCTUATION_END, CHAR_CLASS_EMPTY);
}

void init_single_line_coment(void)
{
  /* single line coment */
  add_transfer(TK_SLASH,TK_SINGLE_LINE_COMENT_BEGIN,0,TFE_ACT_SKIP,CAHR_CLASS_SLASH);
  add_transfer(TK_SINGLE_LINE_COMENT_BEGIN,TK_SINGLE_LINE_COMENT_BEGIN, 
      TFE_FLAG_REVERSED,TFE_ACT_SKIP, CHAR_CLASS_NEWLINE);
  add_transfer(TK_SINGLE_LINE_COMENT_BEGIN, TK_INIT,0,TFE_ACT_SKIP,CHAR_CLASS_NEWLINE);


}

// TODO: check bad comment in error handle
void init_multi_line_coment(void)
{
  /* multi_line coment */
  add_transfer(TK_SLASH,TK_MULTI_LINE_COMENT_BEGIN,0,TFE_ACT_SKIP,CHAR_CLASS_STAR);

  // loop on stars
  add_transfer(TK_MULTI_LINE_COMENT_END, TK_MULTI_LINE_COMENT_END,0,
      TFE_ACT_SKIP, CHAR_CLASS_STAR);

  // selfloop on non stars
  add_transfer(TK_MULTI_LINE_COMENT_BEGIN, TK_MULTI_LINE_COMENT_BEGIN, TFE_FLAG_REVERSED, TFE_ACT_SKIP,
      CHAR_CLASS_STAR);

  add_transfer(TK_MULTI_LINE_COMENT_END,TK_MULTI_LINE_COMENT_BEGIN,TFE_FLAG_REVERSED, TFE_ACT_SKIP,CHAR_CLASS_STAR_SLASH);

  add_transfer(TK_MULTI_LINE_COMENT_BEGIN,TK_MULTI_LINE_COMENT_END,0,TFE_ACT_SKIP,CHAR_CLASS_STAR);

  // end
  add_transfer(TK_MULTI_LINE_COMENT_END, TK_INIT,0,TFE_ACT_SKIP,CAHR_CLASS_SLASH);

}

void init_len_type(void)
{
  node from=TK_INIT;
  entry_t entry;
  node to;
  int len=tknzr_entry_counters[TK_INIT];

  for (int i=0;i < len;++i)
  {
    entry=tknzr_table[from][i];
    to=TFE_STATE(entry);
    if (state_is_brief(to)) {
      set_len_type(from,i,TFE_BRIEF);
    }
  
    else if (state_is_fixlen(to)) {
      set_len_type(from,i,TFE_FIXLEN);
    }

    else if (state_is_varlen(to)) {
      set_len_type(from,i,TFE_VARLEN);
    }

  }

  for (int i=0;i<MAX_TRANSFER_ENTRIES;++i)
  {
    from=i;
    if (state_is_brief(from))
      set_len_type_row(from,TFE_BRIEF);
  
    else if (state_is_fixlen(from))
      set_len_type_row(from,TFE_FIXLEN);


    else if (state_is_varlen(from))
      set_len_type_row(from,TFE_VARLEN);
  }


}

void init_tknzr_table (void)
{
  /* skip spaces */
  add_transfer(TK_INIT, TK_INIT, 0, TFE_ACT_SKIP, CHAR_CLASS_SPACES);

  /* identifier */
  init_identifier();

  /* punctuation */
  init_punctuation();

  /* integer -- dec, oct, hex, long or not , unsigned or not */
  init_integer_literal();

  /* float */ 
  init_float_literal();

  /* single line coment */
  init_single_line_coment();

  /* multi_line coment */
  init_multi_line_coment();

  /* operators */
  init_operator();

  /* character */ 
  init_char_literal();

  /* string */
  init_string_literal();

  /* len type */
  init_len_type();

  /* escaped */
  init_escaped();

}
