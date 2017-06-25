/* transfer.c */
#include "transfer.h"
#include "token_defs.h"

/* TODO: change this file's name to state_table, */
/*   as transfer is only part of a state_table */
/*   and how to `do_transfer` is defined by clients */
#define STATE_TABLE_MAN_LEN 10
static state_table all_tables[STATE_TABLE_MAN_LEN];
static int all_tables_count;
static state_table *cur_table;

static bool char_is_in_class(entry_t cond,char ch);

state_table *alloc_table(void)
{
  assert (all_tables_count < STATE_TABLE_MAN_LEN);
  cur_table = &all_tables[all_tables_count++];
  return cur_table;

}

/* reenterable api */
 int
init_state_table(state_table *table,
    char *name,
    int nrows,
    int ncols, 
    entry_t init_st,
    find_func func)
{
  assert (name);
  assert(nrows>0);
  assert(ncols>0);
  assert(init_st>=0);
  assert(table);

  table->name=name;
  table->nrows=nrows;
  table->ncols=ncols;
  table->init_st=init_st;
  table->func = (func ? func : char_is_in_class);
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

static bool char_is_in_class(entry_t cond, char ch)
{
  char *chcl=char_class2string[cond];
  assert (chcl);
  bool in_class = strchr(chcl, ch);
  return in_class;
}

 

entry_t st_do_transfer(state_table *table, entry_t state, entry_t cc, entry_t nonf)
{
  assert (state >= 0 && state < table->nrows);
  entry_t *ent;
  entry_t entry;
  int len;

  ent=table->diagram[state];
  len=table->count[state];

  for (int i=0;i<len;++i)
  {
    bool in_class = table->func (TFE_COND(ent[i]), cc);
    bool is_reversed = TFE_IS_REVERSED(ent[i]);
    if (in_class && !is_reversed
        || !in_class && is_reversed)
    {
      entry=ent[i];
      ent[i]=ent[0];
      ent[0]=entry;
      return entry;
    }
  }

  return nonf;
}

/* reenterable api to add transfers into the state_table */
  void
st_add_initial(state_table *table, entry_t state,entry_t cond)
{
  st_add_transfer(table,table->init_st,state,0,TFE_ACT_INIT,cond);
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

void st_set_len_type (state_table*table, node from, node to, token_len_type len_type)
{
  if (from >=0 && from < table->nrows && 0 <= to && to < table->count[from])
    TFE_SET_LEN_TYPE(table->diagram[from][to], len_type);
  else
    ; /* do nothing */

}

void st_set_len_type_row (state_table*table,
    node from,
    token_len_type len_type)
{
  if (from >=0 && from < table->nrows) 
  {
    int len=table->count[from];
    for (int i=0;i<len;++i)
    {
      TFE_SET_LEN_TYPE(table->diagram[from][i], len_type);
    }
  }
  else
    ; /* do nothing */

}

/* no reenterable but convenient api to add transfers into `cur_table` */
/* which should be the immediate result of `alloc_table` */
void add_initial(node to, entry_t cond)
{
  st_add_transfer(cur_table, cur_table->init_st, to, 0,TFE_ACT_INIT,cond);
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

void set_len_type(node from, node to, token_len_type len_type)
{
  st_set_len_type (cur_table, from, to, len_type);
}

void set_len_type_row (node from, token_len_type len_type)
{
  st_set_len_type_row(cur_table, from, len_type);
}



