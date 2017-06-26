#include "tknzr_table.h"

state_table *tknzr_table;

static
void set_len_type (node from, node to, token_len_type len_type)
{
  if (from >=0 && from < tknzr_table->nrows && 0 <= to && to < tknzr_table->count[from])
    TFE_SET_LEN_TYPE(tknzr_table->diagram[from][to], len_type);
  else
    ; /* do nothing */

}

static
void set_len_type_row (node from, token_len_type len_type)
{
  if (from >=0 && from < tknzr_table->nrows) 
  {
    int len=tknzr_table->count[from];
    for (int i=0;i<len;++i)
    {
      TFE_SET_LEN_TYPE(tknzr_table->diagram[from][i], len_type);
    }
  }
  else
    ; /* do nothing */

}

static
void init_len_type(void)
{
  node from=TK_INIT;
  entry_t entry;
  node to;
  int len=tknzr_table->count[from];

  for (int i=0;i < len;++i)
  {
    entry=tknzr_table->diagram[from][i];
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

  for (int i=0;i<N_TOKENIZER_ROWS;++i)
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

int init_tknzr_table (void)
{
  tknzr_table = alloc_table();
  if (init_state_table(tknzr_table,
        "tokenizer's table",
        N_TOKENIZER_ROWS,
        N_TOKENIZER_COLS,
        TK_NULL,
        char_is_in_class)<0)
  {
    return -1;
  }

  // TODO tokenizer does not do this!
  // pp does it.
  /* skip spaces */
  add_skip(TK_INIT,TK_INIT,CHAR_CLASS_SPACES);

  /* identifier */
  init_identifier();

  /* punctuation */
  init_punctuation();

  /* integer -- dec, oct, hex, long or not , unsigned or not */
  init_integer_literal();

  /* float */ 
  init_float_literal();

  /* single line coment */
  init_single_line_comment_cfamily();

  /* multi_line coment */
  init_multi_line_comment_cfamily();

  /* operators */
  init_operator();

  /* character */ 
  init_char_literal();

  /* string */
  init_string_literal();

  /* len type */
  init_len_type();

}
