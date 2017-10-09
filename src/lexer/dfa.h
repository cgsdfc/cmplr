#ifndef DEFININESS_FINITE_AUTO_H
#define DEFININESS_FINITE_AUTO_H 1
#define DFA_TAB_MAX_LEN 10
#include <stdbool.h>

typedef struct dfa_state {
  int state;
  int cond;
  int action;
  int usrd;

} dfa_state;

typedef int (*transfer_func)(dfa_state *, int cond);
typedef struct dfa_table {
  transfer_func func;
  int *max;
  int *len;
  dfa_state **diagram;
  int nrows;

} dfa_table;

void destroy_all_dfa(void);
dfa_table *alloc_dfa(int nrows, transfer_func func);
void config_from(int from);
void config_to(int to);
void config_condition(int cond);
void config_action(int action);
void config_usrd(int usrd);
void config_end(void);
int add_from(int from);
int add_to(int to);
int add_config(void);
int alloc_state(bool is_non_terminal);
int transfer(struct dfa_table *dfa, int from, int cond, dfa_state **to);

#endif
