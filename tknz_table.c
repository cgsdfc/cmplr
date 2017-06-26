#include "tknz_table.h"
#include "token_defs.h"

static state_table *table;

node do_transfer(node state, char ch, entry_t *entry)
{
  return st_do_transfer(table,state,ch,entry);
}

static
void init_len_type(void)
{
  node from=TK_INIT;
  entry_t entry;
  node to;
  int len=table->count[from];

  for (int i=0;i < len;++i)
  {
    entry=table->diagram[from][i];
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

state_table *alloc_tokenizer_table(void)
{
  state_table *atable = alloc_table();
  int r=0;
  if(init_state_table(atable,
        "tokenizer's table",
        N_TOKENIZER_ROWS,
        N_TOKENIZER_COLS,
        TK_NULL,
        char_is_in_class)<0) {
    return NULL;
  }
  return atable;

}

state_table *get_tokenizer_table(void)
{
  return table;
}

void init_tknzr_table (void)
{
  table =  alloc_tokenizer_table();
  if (table == NULL) {
    perror("alloc_tokenizer_table");
    exit(2);
  }
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

  /* escaped must be the last as it has its own table*/
  init_escaped();

}
