#include "dfa.h"
#define DFA_TAB_MAX_LEN 10

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
  cur_dfa=&all_dfas[count];
  init_dfa(cur_dfa,nrows,func);
  memset (&cur_config,0,sizeof cur_config);
  cur_config.nrows=nrows;
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
  cur_config.from=0;
  cur_config.to=0;
  cur_config.action=0;
  cur_config.cond=0;
  cur_config.usrd=0;
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
  newspace=malloc(*newlen * sizeof (dfa_state));
  if (!newspace)
    return NULL;
  memcpy(newspace, ds, sizeof(dfa_state) * len);
  free(ds);
  return newspace;

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

  ds=cur_dfa->diagram[from]+cur_dfa->len[from];
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
  return is_non_terminal ?
    cur_config.idcnt ++
    : cur_config.nrows ++;
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

void config_handler(int state, int handler)
{
  int to=-1; // not a valid state
  cur_config.action=handler;
  add_state(state,to);
}

