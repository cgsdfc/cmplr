#ifndef UTILLIB_RULE_H
#define UTILLIB_RULE_H
#include "symbol.h"
#include "vector.h"
#define UTILLIB_RULE_MAX_RHS 10
#define UTILLIB_RULE_RHS(RULE) ((RULE)->RHS)
#define UTILLIB_RULE_LHS(RULE) ((RULE)->LHS)
#define UTILLIB_RULE_NULL (&utillib_rule_null)
#define UTILLIB_RULE_ELEM(LHS, ...) { .LHS_LIT=(LHS), .RHS_LIT= {  __VA_ARGS__ , UT_SYM_NULL } },
#define UTILLIB_RULE_BEGIN(NAME) static const struct utillib_rule_literal NAME[]={
#define UTILLIB_RULE_END(NAME) {0}};

struct utillib_rule_literal {
  int LHS_LIT;
  int RHS_LIT[UTILLIB_RULE_MAX_RHS];
};

struct utillib_rule {
  struct utillib_symbol * LHS;
  struct utillib_symbol * RHS[UTILLIB_RULE_MAX_RHS];
  size_t size;
};

extern struct utillib_rule utillib_rule_null;

/**
 * \struct utillib_rule_index
 * Provides fast access to each rule
 * given the left-hand-side non terminal symbol
 * of the rule. 
 * Each element of the index is a pair of the LHS
 * symbol and its productions in a vector.
 */
struct utillib_rule_index {
  struct utillib_vector rules;
};

void utillib_rule_index_init(struct utillib_rule_index * self, 
    struct utillib_symbol const * symbols,
    struct utillib_rule_literal const * rule_literals);

#endif // UTILLIB_RULE_H
