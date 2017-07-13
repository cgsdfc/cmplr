#ifndef RULES_H 
#define RULES_H 1
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


#define def_rule(gr,...) _def_rule(gr,__VA_ARGS__, RULE_END)
#define def_oneof(gr,...) _def_oneof(gr,__VA_ARGS__, RULE_END)
#define def_symbol(gr,SYM) _def_symbol(gr, #SYM, SYM)

enum
{
  RULE_OPT=-2,
  RULE_END=-1,

  N_SYMBOLS_IDX=0,
  N_NONTERM_IDX,
  N_TERMINAL_IDX,

  IS_MAX_ITEM=30,
  RL_MAX_RHS=20,
  GR_MAX_ITEM=1024,
  GR_MAX_RULE=350,
  GR_MAX_NONTERM=100,
  GR_MAX_SYMBOL=200,
  GR_MAX_PER_RULE=20,
  GR_MAX_IS=1024,

};



typedef struct item
{
  int ruleid;
  int dot;
} item;

typedef struct item_set
{
  int items[IS_MAX_ITEM];
  int len;
} item_set;



typedef struct rule 
{
  int lhs;
  int rhs[RL_MAX_RHS];
  int len;
} rule;


typedef struct grammar
{
  // all the rules stored here
  rule rules[GR_MAX_RULE];
  int nrule;
  int nonterm[GR_MAX_NONTERM][GR_MAX_PER_RULE];
  int nnont_rule[GR_MAX_NONTERM];

  // symbol name
  char *symbol[GR_MAX_SYMBOL];

  // for constructing item sets
  item items[GR_MAX_ITEM];
  int item_id;
  item_set item_sets[GR_MAX_IS];
  int item_set_id;
  int set2symbols[GR_MAX_IS][GR_MAX_SYMBOL];

  // telling symbol's types
  int symbol_blk[10];

} grammar;



void _def_symbol(grammar *gr, char *sym, int id);
void _def_rule(grammar *gr, int lhs,...);
void _def_oneof(grammar *gr, int lhs,...);
void def_onemore(grammar *gr, int lhs, int rhs);
void def_sepmore(grammar *gr, int lhs, int sep, int rhs);

void show_rules(grammar*);

#endif

