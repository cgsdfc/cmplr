#ifndef RULES_H 
#define RULES_H 1
#include "stack_buffer.h"

typedef struct item
{
  int dot;
  int rule;
  int lookahead;
} item;

typedef struct rule 
{
  int lhs;
  int *rhs;
  int max;
  int len;
} rule;

typedef struct item_set
{
  int *items;
  int len;
  int max;
  int id;
} item_set;

#endif

