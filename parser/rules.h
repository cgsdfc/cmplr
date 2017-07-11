#ifndef RULES_H 
#define RULES_H 1
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// global 
#define RULE_OPT (-2)

// variant of rule definitions
#define def_rule(gr,...) _def_rule(gr,__VA_ARGS__, -1)
#define def_oneof(gr,...) _def_oneof(gr,__VA_ARGS__,-1)

typedef struct item
{
  int ruleid;
  int dot;
} item;

typedef struct item_set
{
  int items[30];
  int len;
} item_set;



typedef struct rule 
{
  int lhs;
  int rhs[20];
  int len;
} rule;


typedef struct language
{
  char *name;
  int num_symbol;

  int num_terminal;
  /* [0, nnont) is for nonterms */
  int num_nonterm;
  /* [nnont, nkws) is for terminals */ 
  /*   excluding keywords */
  int num_keyword;
  /* [nkws, nopers) is for keywords excluding */
  /*   operators */
  int num_operator;
} language;


typedef struct grammar
{
  char *name;
  // this struct helps to
  // increamentally build up
  // the grammar rules

  // all the rules stored here
  rule rules[350];
  int nrule;
  int nonterm[100][100];
  int nnont_rule[100];

  // the id alloc for each kind of
  // symbol
  int nonterm_id;
  int terminal_id;
  int keyword_id;
  int operator_id;
  int punctuation_id;
  int symbol_id;

  // symbol name
  char *symbol[200];

  // for constructing item sets
  item items[1024];
  int item_id;
  item_set item_sets[1024];
  int item_set_id;
  int set2symbols[1024][1024];

} grammar;


int init_grammar(grammar *gr, language *lang);
int def_nonterm(grammar *gr, char *rep);
int def_terminal(grammar *gr, char *rep);
int def_keyword(grammar *gr, char *rep);
int def_punc(grammar *gr, char *rep);
int def_oper(grammar *gr, char *rep);

void _def_rule(grammar *gr, int lhs,...);
void _def_oneof(grammar *gr, int lhs,...);
void def_onemore(grammar *gr, int lhs, int rhs);
void def_sepmore(grammar *gr, int lhs, int sep, int rhs);


extern grammar grammar_clang;
extern language lang_clang;

bool is_terminal(grammar *gr, int symbol);
bool is_nonterm(grammar *gr, int symbol);
void show_rules(grammar*);

#endif

