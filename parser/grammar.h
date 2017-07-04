#ifndef GRAMMAR_H
#define GRAMMAR_H 1
#include "tknzr/token_type.h"
#define END_RULE (-1)
#define OPT_RULE (-2)

// this file is attempted to descript
// the grammar of C programming language
// in form of enum and arrays.
// 
// later on there will be parser generator
// looking at this file and generating a 
// table for parsing it

typedef struct sym_pair
{
  char *rep;
  int id;
} terminal;



typedef struct grammar
{
  /* here goes all the rules */
  rule **all_rules;

  /* the len of all_rules */
  int rule_len;

  /* the maximum len of all_rules */
  int rule_max;

  /* a mapping for each nonterms to its rules */
  int **nont2rules;

  /* the len for each such mapping */
  int *len2rule;

  /* the maximum of the len of the mapping */
  int *max2rule;

  /* the number of non terminals */
  int nnonterms;

  /* the number of terminals */
  int nterms;

} grammar;

typedef struct grammar_config
{
  /* non terminal id alloc */
  int nonterm_id;

  /* terminal id alloc */
  int term_id;

  /* item id alloc, the same item is alloc only once */
  int item_id;

  /* the max of all_items */
  int item_max;

  /* all the items are put here */
  item **all_items;

  /* itset id alloc */
  int item_set_id;

  /* the max of all_item_sets */
  int item_set_max;

  /* all the item_set goes here */
  item_set **all_item_sets;

} grammar_config;

typedef struct const_symbols
{
  sym_pair *keywords;
  sym_pair *terminals;
  sym_pair *nonterms;

  int 
} const_symbols;
 

#endif

