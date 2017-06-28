/* state_table.c */
#include "state_table.h"

#define STATE_TABLE_MAN_LEN 20
static state_table all_tables[STATE_TABLE_MAN_LEN];
static int all_tables_count;
static state_table *cur_table;


state_table *alloc_table(void)
{
  assert (all_tables_count < STATE_TABLE_MAN_LEN);
  cur_table = &all_tables[all_tables_count++];
  return cur_table;

}

/* reenterable api */
/* zero is reserved for initial state, thus no need to set */
 int
init_state_table(state_table *table,
    char *name,
    int nrows,
    int ncols, 
    entry_t null_st,
    find_func func)
{
  assert (name);
  assert(nrows>0);
  assert(ncols>0);
  assert(table);
  assert(null_st != 0);
  assert(func);

  table->name=name;
  table->nrows=nrows;
  table->ncols=ncols;
  table->null_st=null_st;
  table->func = func;

  if (NULL == (table->diagram=malloc(sizeof(entry_t*) * nrows)))
  {
    perror("malloc");
    return -1;
  }
  table->count=malloc(sizeof(int) * nrows);

  for (int i=0;i<nrows;++i)
  {
    table->diagram[i]=malloc(sizeof(entry_t) * ncols);
    table->count[i]=0;
  }

  return 0;
} 

  void 
st_add_transfer(state_table *table, entry_t from, entry_t to, entry_t flags, entry_t act, entry_t cond)
{
  assert(0<=from && from < table->nrows);
  assert(0<= to && to < table->ncols);
  entry_t entry = TFE_MAKE_ENTRY(act,cond,to,flags);
  table->diagram[from][table->count[from]++]=entry;
}



 
/** look up possible transfer in `table` for `state` and `cc`
 * 
 * if found, return the destination and store the corresponding
 * entry into `entry`.
 * 
 * if no found, return `nonf` to indicate that.
 * caller should make sure `nonf` is not a valid state.
 * 
 * if error happened, -1 is returned and caller should
 * consult the **errno according to what `table->find_func`
 * ought to set. caller should make sure -1 is not a valid
 * state.
 */
node st_do_transfer(state_table *table, entry_t state, entry_t cc,entry_t *entry)
{
  assert (state >= 0 && state < table->nrows);
  assert (entry);
  assert (table);
  assert (state != table->null_st);
  assert (cc);
  entry_t *ent;
  int len;
  node nstate;

  ent=table->diagram[state];
  len=table->count[state];

  for (int i=0;i<len;++i)
  {
    int in_class = table->func (TFE_COND(ent[i]), cc);
    if (in_class == -1)
    {
      // error happened
      return -1;
    }

    bool is_reversed = TFE_IS_REVERSED(ent[i]);
    if (in_class && !is_reversed
        || !in_class && is_reversed)
    {
      *entry=ent[i];
      ent[i]=ent[0];
      ent[0]=*entry;
      nstate=TFE_STATE(*entry);
      assert(table->null_st != nstate);
      return nstate;
    }
  }

  return table->null_st;
}

/* reenterable api to add transfers into the state_table */
  void
st_add_initial(state_table *table, entry_t state,entry_t cond)
{
  st_add_transfer(table,0,state,0,TFE_ACT_INIT,cond);
}

  void 
st_add_intermedia(state_table *table, entry_t from, entry_t to, entry_t cond)
{
  st_add_transfer(table,from,to,0,TFE_ACT_APPEND,cond);
}

  void 
st_add_accepted(state_table *table, entry_t from, entry_t to,  entry_t cond)
{
  st_add_transfer(table,from,to,TFE_FLAG_ACCEPTED,TFE_ACT_ACCEPT,cond);
}

  void
st_add_selfloop(state_table *table, entry_t state, entry_t cond)
{
  st_add_transfer(table,state,state,0,TFE_ACT_APPEND,cond);
}

/* reversed versions */
  void 
st_add_intermedia_rev (state_table *table, entry_t from, entry_t to, entry_t cond)
{
  st_add_transfer(table,from,to,TFE_FLAG_REVERSED,TFE_ACT_APPEND,cond);
}

  void 
st_add_accepted_rev (state_table *table,entry_t from, entry_t to, entry_t cond)
{
  st_add_transfer(table,from ,to,TFE_FLAG_REVERSED | TFE_FLAG_ACCEPTED,TFE_ACT_ACCEPT,cond);
}

  void
st_add_selfloop_rev (state_table *table, entry_t state, entry_t cond)
{
  st_add_transfer(table,state,state,TFE_FLAG_REVERSED,TFE_ACT_APPEND,cond);
}

/* no reenterable but convenient api to add transfers into `cur_table` */
/* which should be the immediate result of `alloc_table` */
void add_initial(node to, entry_t cond)
{
  st_add_transfer(cur_table, 0, to, 0,TFE_ACT_INIT,cond);
}

void add_intermedia_rev (node from, node to, entry_t cond ) 
{
  st_add_transfer(cur_table, from, to, TFE_FLAG_REVERSED, TFE_ACT_APPEND, cond);
}

void add_intermedia (node from, node to, entry_t cond) 
{
  st_add_transfer(cur_table, from, to, 0, TFE_ACT_APPEND, cond);
}

void add_selfloop (node from, entry_t cond)
{
  add_intermedia(from,from,cond);
}

void add_selfloop_rev (node from, entry_t cond)
{
  add_intermedia_rev(from,from,cond);
}

void add_accepted_rev(node from,  node to, entry_t cond) 
{
  st_add_transfer(cur_table, from, to, TFE_FLAG_REVERSED|TFE_FLAG_ACCEPTED, TFE_ACT_ACCEPT, cond);
}
void add_accepted(node from, node to , entry_t cond) 
{
  st_add_transfer(cur_table, from, to, TFE_FLAG_ACCEPTED, TFE_ACT_ACCEPT, cond);
}

void add_skip(node from, node to, entry_t cond)
{
  st_add_transfer(cur_table,from,to,0,TFE_ACT_SKIP,cond);
}

void add_skip_rev (node from, node to, entry_t cond)
{
  st_add_transfer (cur_table,from,to,TFE_FLAG_REVERSED,TFE_ACT_SKIP,cond);
}

void add_skip_loop (node from, entry_t cond)
{
  st_add_transfer(cur_table,from,from,0,TFE_ACT_SKIP,cond);
}

void add_skip_rev_loop (node from, entry_t cond)
{
  st_add_transfer (cur_table,from,from,TFE_FLAG_REVERSED,TFE_ACT_SKIP,cond);
}
