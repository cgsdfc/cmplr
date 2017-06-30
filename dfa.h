#ifndef DEFININESS_FINITE_AUTO_H
#define DEFININESS_FINITE_AUTO_H 1

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
typedef struct dfa_state
{
  int state;
  int cond;
  int action;
  int usrd;

} dfa_state;

typedef int (*transfer_func ) (dfa_state *, int cond);
typedef struct dfa_table
{
  transfer_func func;
  int *max;
  int *len;
  dfa_state **diagram;
  int nrows;

} dfa_table;

typedef struct dfa_config
{
  int from;
  int to;
  int action;
  int cond;
  int usrd;
  int idcnt;
  int nrows;

} dfa_config;

dfa_table *alloc_dfa(int nrows, transfer_func func);
void config_from(int from);
void config_to(int to);
void config_condition(int cond);
void config_action(int action);
void config_usrd(int usrd);
int config_handler(int state, int handler);
void config_end(void);
int add_from(int from);
int add_to(int to);
int alloc_state(bool is_non_terminal);
int transfer(dfa_table *dfa,int from, int cond, dfa_state **to);
int config_table(int);
int dfa_get_handler(dfa_table *tb, int state, dfa_state **entry);

#endif

