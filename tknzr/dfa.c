#include "dfa.h"
#define DFA_TAB_MAX_LEN 10
static int
add_state(int from, int to);

enum 
{
  DFA_USR_HANDLER=1,
  DFA_DEF_HANDLER,
};

static dfa_table *cur_dfa;
static dfa_config cur_config;
static dfa_table all_dfas[DFA_TAB_MAX_LEN];
static int dfa_count;

int init_dfa(dfa_table *dfa, int nrows, 
    transfer_func func)
{
  if (!func)
    return -1;
  if (!dfa)
    return -1;
  if (nrows <= 0)
    return -1;

  int *len=malloc(sizeof(int)*nrows);
  if(!len)
    return -1;

  int *max=malloc(sizeof(int)*nrows);
  if(!max)
    return -1;

  dfa_state **states=malloc(sizeof(dfa_state*) * nrows);
  if (!states)
    return -1;

  // the rows are not alloced by now
  memset(max, 0, sizeof(int)*nrows);
  memset(len, 0, sizeof(int)*nrows);
  dfa->nrows=nrows;
  dfa->func=func;
  dfa->diagram=states;
  dfa->len=len;
  dfa->max=max;

  return 0;

}

dfa_table *alloc_dfa(int nrows, transfer_func func)
{
  int count=dfa_count;
  if (count == DFA_TAB_MAX_LEN)
    return NULL;

  cur_dfa=&all_dfas[count];
  init_dfa(cur_dfa,nrows,func);

  memset (&cur_config,0,sizeof cur_config);
  cur_config.nrows=nrows;
  cur_config.idcnt=1;
  /* important! 0 is reserved for *init* */

  ++dfa_count;
  return cur_dfa;
}


void config_from(int from)
{
  cur_config.from=from;
}

void config_to(int to)
{
  cur_config.to=to;
}

void config_condition(int cond)
{
  cur_config.cond=cond;
}

void config_action(int action)
{
  cur_config.action=action;
}

void config_usrd(int usrd)
{
  cur_config.usrd=usrd;
}

void config_end(void)
{ 
  int idcnt=cur_config.idcnt;
  int nrows=cur_config.nrows;
  memset(&cur_config, 0, sizeof(dfa_config));
  cur_config.idcnt=idcnt;
  cur_config.nrows=nrows;
   
}

// when add_** finds that one
// row of entries is full, resize_row
// will be called to enlarge the row
// it takes in the old size `len`
// and return the len of the newly
// alloc array
static
dfa_state *resize_row(dfa_state *ds, int len, int *newlen)
{
  dfa_state *newspace;
  *newlen=(len&1) ?len << 1 : len * 3 + 5;
  // newlen = len*2 if len is odd else 3len+5
  newspace=calloc(*newlen, sizeof (dfa_state));
  if (!newspace)
    return NULL;

  // when initial len==0 no problem!
  // check if **the standard** make it sure.
  memcpy(newspace, ds, sizeof(dfa_state) * len);
  free(ds);
  return newspace;

}

int add_config(void)
{
  add_state(cur_config.from, cur_config.to);
}

static int
add_state(int from, int to)
{
  dfa_state *ds;
  int len, newlen;

  len=cur_dfa->len[from];
  ds=cur_dfa->diagram[from];
  if (len==cur_dfa->max[from])
  {
    ds=resize_row(ds, len, &newlen);
    if (!ds) {
      return -1;
    }
    cur_dfa->diagram[from]=ds;
    cur_dfa->max[from]=newlen;
  }

  ds=cur_dfa->diagram[from]+len;
  ds->state=to;
  ds->cond=cur_config.cond;
  ds->action=cur_config.action;
  ds->usrd=cur_config.usrd;
  cur_dfa->len[from]++;
  return 0;

}

// after calling config_to,
// call add_from will add to,
// which was configed, to `from`'s
// entries
int add_from(int from)
{
  return add_state(from, cur_config.to);
}


// after calling config_from,
// call add_to will add `to`
// into `from`'s entries configed
// before
int add_to(int to)
{
  return add_state(cur_config.from, to);
}

int alloc_state(bool is_non_terminal)
{
  // bad practice if they overlap
  if (is_non_terminal) 
  {
    if (cur_config.idcnt == cur_config.nrows-1)
    {
      // TODO write a little logger
      printf("[FATAL] alloc_state: they overlap\n");
      exit(1);
    }
    return cur_config.idcnt ++;
  }

  return cur_config.nrows ++;
}

// this function look up the diagram inside 
// `dfa`, to find a suitabl state to go to
// the return value depends on the usr `func`
// `func` returns 1 if can go, 0 if can not,
// -1 if error happened.
// this function does a little diff than the
// usr func does: if success, it returns 0,
// if fail, it return 1, error is -1, which
// is the same as the usr
// the last entry of each row is the error 
// handler's state, every one should install 
// this.
int transfer(dfa_table *dfa,int from, int cond, dfa_state **to)
{
  int len=dfa->len[from];
  dfa_state *ds=dfa->diagram[from];

  for (int i=0;i<len;++i)
  {
    *to=(ds+i);
    switch (dfa->func(ds + i, cond))
    {
      case 1:
        return 0;
      case -1:
        return -1;
      case 0:
        continue;
    }
  }
  return 1;
}

// the function set error handler for state
// which takes effect when a call to transfer
// search through the row but reach the end.
// in other words, the **end** of each row 
// must not be a valid state. call to config_table()
// graruantees that, as it walks through all the 
// rows and set for you a default handler.
// 
// the last entry in a row is a handler.
// the state field of it is used to mark whether usr 
// has given specific value to it, if it is 0,
// this means usr never touched it, so default
// handler provided by usr will be set;
// otherwise, the the handler is left untouched.
int config_handler(int state, int handler)
{
  if (0<=state && state < cur_dfa->nrows)
  {
    cur_config.action=handler;
    add_state(state, DFA_USR_HANDLER);
    return 0;
  }

  return -1;
}

int dfa_get_handler(dfa_table *tb, int state, dfa_state **entry)
{
  if (!tb)
    return -1;
  if (!entry)
    return -1;

  if (0<=state && state < tb->nrows)
  {
    int last=tb->len[state]-1;
    *entry = tb->diagram[state] + last;
    return 0;
  }

  return -1;
}

int config_table(int def_handler)
{
  // walk through the table
  dfa_state **table;
  int last;
  int actual_rows=cur_config.idcnt;

  table=cur_dfa->diagram;
  for (int i=0;i<actual_rows;++i)
  {
    last = cur_dfa->len[i];
    if (last == 0)
    {
      printf("error! empty row discovered %d\n", i);
      continue;
    }

    if (table[i][last].state == DFA_USR_HANDLER)
    {
      continue;
    }
    cur_config.action=def_handler;
    add_state(i, DFA_DEF_HANDLER);
  }
  cur_dfa->nrows=actual_rows;

  return 0;
}




