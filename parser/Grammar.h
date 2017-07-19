#ifndef GRAMMAR_H
#define GRAMMAR_H 1

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <array>

#define def_rule(gr,...)\
  grammar_def_rule(gr,__VA_ARGS__, RULE_END)

#define def_oneof(gr,...)\
  grammar_def_oneof(gr,__VA_ARGS__, RULE_END)

#define def_symbol(gr,SYM)\
 grammar _def_symbol(gr, #SYM, SYM)

#define def_sepmore(gr, LHS, SEP, RHS)\
  grammar_def_sepmore(gr, (LHS), (SEP), (RHS))

#define def_onemore(gr, LHS, RHS)\
  grammar_def_onemore(gr, (LHS), (RHS))


typedef enum grammar_symbol_index
{
  N_SYMBOLS_IDX=0,
  N_NONTERM_IDX,
  N_TERMINAL_IDX,

} grammar_symbol_index;

typedef enum rule_flag
{
  RULE_OPT=-2,
  RULE_END=-1,
} rule_flag;



struct Rule 
{
  int lhs;
  std::vector <int> rhs;
} ;


struct Grammar
{
  // all the rules stored here
 
  const static GR_MAX_SYMBLK=10;

  std::vector <Rule> rules;
  int nonterm[GR_MAX_NONTERM][GR_MAX_RULE_PER_NONTERM];
  std::vector <std::vector <int> > rulesOfNonterm;

  // symbol name
  const char *symbols[GR_MAX_SYMBOL];
  
  // telling symbol's types
  std::array <int, GR_MAX_SYMBLK> symbol_blk;

  
  void printRule(int ruleID);
  void defSymbol(const char *symbol, int id);
  void defRule(int lhs, ...);
  void defOneof(int lhs, ...);
  void defOneMore(int lhs,int rhs);
  void defSepMore(int lhs,int sep, int rhs)
  void printRules(void);


} ;


#endif

