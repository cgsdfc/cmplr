#ifndef RULES_H 
#define RULES_H 1
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#define CLANG_GR (&grammar_clang)
#define DEF_RULE(...) def_rule(CLANG_GR,__VA_ARGS__, -1)
#define DEF_ONEOF(...) def_oneof(CLANG_GR,__VA_ARGS__,-1)
#define DEF_NONTERM(REP) def_nonterm(CLANG_GR,REP)
#define DEF_TERMINAL(REP) def_terminal(CLANG_GR,REP)
#define DEF_GRAMMAR() init_grammar(CLANG_GR,"C programming language", 100)
#define SHOW_RULES() show_rules(CLANG_GR)


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


int init_grammar(grammar *gr,char *lang, int nnont);
int def_nonterm(grammar *gr, char *rep);
int def_terminal(grammar *gr, char *rep);
bool is_terminal(grammar *gr, int symbol);
bool is_nonterm(grammar *gr, int symbol);
void def_rule(grammar *gr, int lhs,...);
void def_oneof(grammar *gr, int lhs,...);
void show_rules(grammar*);

extern grammar grammar_clang;



#endif

