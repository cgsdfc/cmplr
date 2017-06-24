/* transfer.c */
#include "transfer.h"
#include "token_defs.h"

extern transfer_table_t tknzr_table;
extern int tknzr_entry_counters[MAX_TRANSFER_ENTRIES] ;



void add_initial(node to, char_class_enum char_class)
{
  add_transfer (TK_INIT, to, 0,TFE_ACT_INIT,char_class);
}


void add_intermedia_rev (node from, node to, char_class_enum char_class ) 
{
  add_transfer(from, to, TFE_FLAG_REVERSED, TFE_ACT_APPEND, char_class);
}

void add_intermedia (node from, node to, char_class_enum char_class) 
{
  add_transfer(from, to, 0, TFE_ACT_APPEND, char_class);
}

void add_selfloop (node from, char_class_enum char_class)
{
  add_intermedia(from,from,char_class);
}

void add_selfloop_rev (node from, char_class_enum char_class)
{
  add_intermedia_rev(from,from,char_class);
}

void add_accepted_rev(node from,  node to, char_class_enum  char_class) 
{
  add_transfer(from, to, TFE_FLAG_REVERSED|TFE_FLAG_ACCEPTED, TFE_ACT_ACCEPT, char_class);
}
void add_accepted(node from, node to , char_class_enum char_class) 
{
  add_transfer(from, to, TFE_FLAG_ACCEPTED, TFE_ACT_ACCEPT, char_class);
}

void set_len_type_r (transfer_table_t table, int *entry_counters, node from, node to, token_len_type len_type)
{
  if (from >=0 && from < MAX_TRANSFER_ENTRIES && 0 <= to && to < entry_counters[from])
    TFE_SET_LEN_TYPE(table[from][to], len_type);
  else
    ; /* do nothing */

}

void set_len_type(node from, node to, token_len_type len_type)
{
  set_len_type_r (tknzr_table, tknzr_entry_counters, from, to, len_type);
}

void set_len_type_row_r (transfer_table_t table, 
    int *counter,
    node from,
    token_len_type len_type)
{
  if (from >=0 && from < MAX_TRANSFER_ENTRIES) 
  {
    int len=counter[from];
    for (int i=0;i<len;++i)
    {
      TFE_SET_LEN_TYPE(table[from][i], len_type);
    }
  }
  else
    ; /* do nothing */

}

void set_len_type_row (node from, token_len_type len_type)
{
  set_len_type_row_r(tknzr_table, tknzr_entry_counters, from, len_type);
}

void check_set_len_type(void)
{

  token_len_type len_type;
  /* loop for all the from=non-accepted and to=all-state with all len_type */
  puts("check_set_len_type begin");
  for (int i=TK_INIT; i<_TK_NON_ACCEPTED_END;++i)
  {
    for (int j=TK_INIT;j<TK_NULL;++j)
    {
      for (int k=TFE_BRIEF;k<_TFE_LEN_TYPE_END;++k)
      {
        set_len_type(i,j,k);
        len_type=TFE_LEN_TYPE(tknzr_table[i][j]);
        assert(len_type == k);
      }
    }
  }
  void clear_tknzr_table(void);
  clear_tknzr_table();
  puts("check_set_len_type passed");
}


/* find the entry that defines a transfer going from */
/* `state_from` to `state_to` */
/* should only be called after init_tknzr_table */
  static
transfer_entry seek_entry_r (transfer_table_t table, int *entry_counters ,int state_from, int state_to)
{

  transfer_entry entry;
  for (int i=0;i<entry_counters[state_from];++i)
  {
    entry = table[state_from][i];
    if (TFE_STATE(entry) == state_to)
      return entry;
  }
  return 0;
}

entry_t seek_entry(tokenizer_state from,
    tokenizer_state to)
{
  assert (from >=0 && from < MAX_TRANSFER_ENTRIES);
  return seek_entry_r (tknzr_table, tknzr_entry_counters,from,to);

}

  static
char *get_char_class(char_class_enum char_class)
{
  char *ch=char_class2string[char_class];
  assert (ch != NULL);
  return ch;
}

bool can_transfer(entry_t entry,  int character) 
{
  char_class_enum cclass = TFE_CHAR_CLASS(entry);
  char *char_class=get_char_class(cclass);

  // TODO: use bsearch
  bool is_in_class = strchr (char_class, character);
  return (is_in_class && ! TFE_IS_REVERSED(entry)) || 
    (!is_in_class && TFE_IS_REVERSED(entry));

}

void add_transfer_r(transfer_table_t table, int *entry_counters,
    int from,
    int state, 
    entry_flag flags,
    entry_action act,
    char_class_enum cclass)
{
  int len=entry_counters[from];

  /* the order is : */
  /* (action | char_class | state | flags) */
  transfer_entry entry = TFE_MAKE_ENTRY(act,cclass,state,flags);
  table[from][len]=entry;
  entry_counters[from]++;

}

  static
int do_transfer_r (transfer_table_t table, int *entry_counters,
    int state, /* state must be a non-accepted and not null */
    int ch, transfer_entry *entry, int state_not_found)
{
  assert (entry);
  assert (state != state_not_found);
  assert (table);
  assert(entry_counters);
  assert (state >= 0);

  int  nstate;
  char_class_enum char_class;

  for (int i=0;i<entry_counters[state];++i)
  {
    *entry = table[state][i];
    nstate = TFE_STATE(table[state][i]);
    if (can_transfer (table[state][i], ch)) 
    {
      // TODO: swap table[state][0] and table[state][i]
      return nstate;
    }
  }
  return state_not_found;

}

tokenizer_state do_transfer (tokenizer_state state,
    int ch, transfer_entry *entry)
{
  assert (state >=0 && state < MAX_TRANSFER_ENTRIES);
  return
    (tokenizer_state) do_transfer_r(
        tknzr_table, tknzr_entry_counters,
        state, ch, entry, TK_NULL);

}

/** add a transfer from `from` to `to`
 * the `from` must be a non-accepted
 */
void add_transfer(tokenizer_state from,
    tokenizer_state state, 
    entry_flag flags, entry_action act,
    char_class_enum cclass)
{
  assert (from >=0 && from < MAX_TRANSFER_ENTRIES);
  add_transfer_r( tknzr_table, tknzr_entry_counters, from,state,flags,act,cclass);

}

