#include "tknz_table.h"
#include "token_defs.h"

transfer_table_t tknzr_table;
int tknzr_entry_counters[MAX_TRANSFER_ENTRIES];
void check_can_transfer (tokenizer_state from, 
    tokenizer_state to, char_class_enum cc);


static
void check_table (void)
{
  // TODO: check more throughly
#ifndef NDEBUG
  puts("check_table begin");
  init_tknzr_table ();

  /* TK_INIT -> TK_IDENTIFIER_BEGIN */
  check_can_transfer(TK_INIT,TK_IDENTIFIER_BEGIN,CHAR_CLASS_IDENTIFIER_BEGIN);

  /* check TK_INIT -> TK_INIT */ 
  check_can_transfer(TK_INIT,TK_INIT,CHAR_CLASS_SPACES);

  /* check TK_IDENTIFIER_BEGIN -> TK_IDENTIFIER_END */
  check_can_transfer(TK_IDENTIFIER_BEGIN,TK_IDENTIFIER_END,CHAR_CLASS_SEPARATOR);

  /* check TK_IDENTIFIER_BEGIN -> TK_IDENTIFIER_BEGIN */
  check_can_transfer(TK_INIT,TK_IDENTIFIER_BEGIN,CHAR_CLASS_IDENTIFIER_BEGIN);

  /* check TK_INIT -> TK_PUNCTUATION_END */
  check_can_transfer(TK_INIT,TK_PUNCTUATION_END,CHAR_CLASS_PUNCTUATION);

  check_can_transfer(TK_STRING_LITERAL_BEGIN,TK_STRING_LITERAL_ESCAPED,
      CHAR_CLASS_BACKSLASH);

  printf ("check_init_table passed\n");
  clear_tknzr_table();
#endif

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
  /* .0 => float_literal */
  /* .a => period */
  add_initial(TK_PERIOD,CHAR_CLASS_PERIOD);
  add_accepted_rev (TK_PERIOD,TK_PERIOD_END,CHAR_CLASS_DEC_PART);
  add_accepted(TK_INIT, TK_PUNCTUATION_END, CHAR_CLASS_PUNCTUATION);
}

void init_single_line_coment(void)
{
  /* single line coment */
  add_transfer(TK_SLASH,TK_SINGLE_LINE_COMENT_BEGIN,0,TFE_ACT_SKIP,CAHR_CLASS_SLASH);
  add_transfer(TK_SINGLE_LINE_COMENT_BEGIN,TK_SINGLE_LINE_COMENT_BEGIN, 
      TFE_FLAG_REVERSED,TFE_ACT_SKIP, CHAR_CLASS_NEWLINE);
  add_transfer(TK_SINGLE_LINE_COMENT_BEGIN, TK_INIT,0,TFE_ACT_SKIP,CHAR_CLASS_NEWLINE);


}

// TODO: change it , donot detect bad multi_line 
// just let the parser get fucked
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

}
void check_counters(void)
{
  for (int i=0;i<MAX_TRANSFER_ENTRIES;++i)
  {
    assert(tknzr_entry_counters[i] == 0);
  }
}

void check_tknzr_table (void) 
{
  puts("check_tknzr_table begin");
 
  check_operators();
  check_fields();
  check_char_class();
  check_set_len_type();
  check_counters();
  check_table();
  clear_tknzr_table();

  puts ("check_table passed");

}

void check_fields(void)
{
#ifndef NDEBUG
  entry_t entry;
  entry_t flag, action, state, char_class;
  puts("check fields begin");

  /* check all the action fields */
  for (int i=TFE_ACT_INIT;i<TFE_ACT_SKIP+1;++i)
  {
    entry = TFE_MAKE_ENTRY(i, 0, 0,0); 
    action = TFE_ACTION(entry);
    assert (action == i );
  }
  puts ("check action passed");

  /* check all the char_class fields */
  for (int i=CHAR_CLASS_EMPTY;i<_CHAR_CLASS_NULL;++i)
  {
    entry = TFE_MAKE_ENTRY(0, i,0,0);
    char_class=TFE_CHAR_CLASS (entry);
    assert (char_class== i);
    assert (char_class2string[i]!=NULL);
  }
  puts ("check char_class passed");


  /* check all the state fields */
  for (int i=TK_INIT;i<TK_NULL+1;++i)
  {
    entry = TFE_MAKE_ENTRY(0, 0,i,0);
    state=TFE_STATE(entry);
    assert (state == i);
  }

  puts ("check state passed");

  for (int l=0;l<_TFE_LEN_TYPE_END;++l)
  {
    TFE_SET_LEN_TYPE(entry,l);
    assert (TFE_LEN_TYPE(entry) == l);
  }

  puts("check len_type passed");

  entry = TFE_MAKE_ENTRY(0,0,0,TFE_FLAG_ACCEPTED);
  assert (TFE_IS_ACCEPTED(entry));

  entry = TFE_MAKE_ENTRY(0,0,0,TFE_FLAG_REVERSED);
  assert(TFE_IS_REVERSED(entry));

  puts("check flags passed");

  puts("check all possible combination begin");
  /* use a big loop to test all possible */
  for (int i=TFE_ACT_INIT;i<TFE_ACT_SKIP+1;++i)
  {
    for (int j=CHAR_CLASS_EMPTY;j<_CHAR_CLASS_NULL;++j)
    {
      for (int k=TK_INIT;k<TK_NULL+1;++k)
      {
        for (int l=0;l<_TFE_LEN_TYPE_END;++l)
        {
          entry=TFE_MAKE_ENTRY(i,j,k,0);
          TFE_SET_LEN_TYPE(entry,l);
          assert (j==TFE_CHAR_CLASS(entry));
          assert (i==TFE_ACTION(entry));
          assert (k==TFE_STATE(entry));
          assert (TFE_LEN_TYPE(entry) == l);
        }
      }
    }
  }

  puts ("check_fields passed");
#endif

}

/** for each char in `char_class`, check `from` can transfer to `to` */
void check_can_transfer (tokenizer_state from, 
    tokenizer_state to, char_class_enum cc)
{
  char *char_class=char_class2string[cc];
  for (int i=0;i<strlen(char_class);++i) 
  {
    int letter = char_class[i];
    assert (can_transfer(seek_entry(from, to), letter));

  }

}
