#ifndef RULES_H 
#define RULES_H 1
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// global 
#define RULE_OPT (-2)
#define CLANG_LG (&lang_clang)
#define CLANG_GR (&grammar_clang)

// variant of rule definitions
#define DEF_RULE(...) def_rule(CLANG_GR,__VA_ARGS__, -1)
#define DEF_ONEOF(...) def_oneof(CLANG_GR,__VA_ARGS__,-1)
#define DEF_ONEMORE(L,R) def_onemore(CLANG_GR, (L), (R))

// variant of symbol definitions
#define DEF_NONTERM(REP) def_nonterm(CLANG_GR,REP)
#define DEF_TERMINAL(REP) def_terminal(CLANG_GR,REP)
#define DEF_KEYWORD(REP) def_keyword(CLANG_GR, REP)
#define DEF_PUNC(REP) def_punc(CLANG_GR, REP)

// initial entry
#define DEF_GRAMMAR() init_grammar(CLANG_GR, CLANG_LG);

// textual rep
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

typedef struct language
{
  char *name;
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
  rule rules[1024];
  int nrule;
  int nonterm[1024][1024];
  int nnont_rule[1024];

  // the id alloc for each kind of
  // symbol
  int nonterm_id;
  int terminal_id;
  int keyword_id;
  int operator_id;
  int punctuation_id;

  // symbol name
  char *symbol[1024];

} grammar;


int init_grammar(grammar *gr, language *lang);
int def_nonterm(grammar *gr, char *rep);
int def_terminal(grammar *gr, char *rep);
int def_keyword(grammar *gr, char *rep);
int def_punc(grammar *gr, char *rep);
void def_rule(grammar *gr, int lhs,...);
void def_oneof(grammar *gr, int lhs,...);
void def_onemore(grammar *gr, int lhs, int rhs);
extern grammar grammar_clang;
extern language lang_clang;

bool is_terminal(grammar *gr, int symbol);
bool is_nonterm(grammar *gr, int symbol);
void show_rules(grammar*);

#endif
