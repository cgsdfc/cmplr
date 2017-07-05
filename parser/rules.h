#ifndef RULES_H 
#define RULES_H 1
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


typedef struct item
{
  int dot;
  int rule;
  int lookahead;
} item;

typedef struct rule 
{
  int lhs;
  int rhs[20];
  int len;
} rule;

typedef struct item_set
{
  int *items;
  int len;
  int max;
  int id;
} item_set;

typedef struct grammar
{
  // this struct helps to
  // increamentally build up
  // the grammar rules

  // all the rules stored here
  rule rules[1024];
  char *lang;
  int nrule;

  // for each non-terminal, their rules
  int nonterm[1024][1024];
  int nnont_rule[1024];
  int nnont;
  int nterm;
  char *symbol[1024];

} grammar;


int def_nonterm(grammar *gr, char *rep);
int def_terminal(grammar *gr, char *rep);
bool is_terminal(grammar *gr, int symbol);
bool is_nonterm(grammar *gr, int symbol);
void def_rule(grammar *gr, int lhs,...);
int init_grammar(grammar *gr,char *lang, int nnont);

void show_rules(grammar*);

#endif

