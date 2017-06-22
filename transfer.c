/* transfer.c */
#include "transfer.h"

extern entry_t tknzr_table[MAX_TRANSFER_ENTRIES][MAX_TRANSFER_ENTRIES];
extern int tknzr_entry_counters[MAX_TRANSFER_ENTRIES]
;
void add_transfer(tokenizer_state from,
    tokenizer_state state, 
    entry_flag flags, entry_action act,
    char_class_enum cclass);


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

/* find the entry that defines a transfer going from */
/* `state_from` to `state_to` */
static
transfer_entry seek_entry_r (entry_t **table, int *entry_counters ,int state_from, int state_to)
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
  return seek_entry_r ((entry_t**) tknzr_table, tknzr_entry_counters,from,to);

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

  bool is_in_class = strchr (char_class, character);
  return (is_in_class && ! TFE_IS_REVERSED(entry)) || 
    (!is_in_class && TFE_IS_REVERSED(entry));

}

  static
void add_transfer_r(entry_t **table, int *entry_counters,
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
int do_transfer_r (entry_t **table, int *entry_counters,
    int state,
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
        (entry_t **) tknzr_table, tknzr_entry_counters,
        state, ch, entry, TK_NULL);

}

void add_transfer(tokenizer_state from,
    tokenizer_state state, 
    entry_flag flags, entry_action act,
    char_class_enum cclass)
{
  assert (state >=0 && state < MAX_TRANSFER_ENTRIES);
  add_transfer_r((entry_t **) tknzr_table, tknzr_entry_counters, from,state,flags,act,cclass);

}

void clear_table_r (entry_t **table, int *entry_counters)
{
  memset(table, 0, sizeof table);
  memset(entry_counters, 0, sizeof entry_counters);
}
