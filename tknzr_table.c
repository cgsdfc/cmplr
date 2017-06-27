#include "tknzr_table.h"

state_table *tknzr_table;

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


}
