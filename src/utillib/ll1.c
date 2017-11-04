#include "ll1.h"
#include <stdlib.h>

static struct utillib_ll1_set * ll1_set_create(size_t N) {
  struct utillib_ll1_set * self=malloc( sizeof *self);
  self->flag=false;
  utillib_bitset_init(&self->bitset, N);
  return self;
}

static void ll1_set_destroy(struct utillib_ll1_set * self) {
  utillib_bitset_destroy(&self->bitset);
  free(self);
}

void utillib_ll1_builder_init(struct utillib_ll1_builder *self, 
    struct utillib_rule_index const * rule_index)
{
  self->rule_index=rule_index;
}

static void ll1_builder_FIRST_init(struct utillib_ll1_builder *self) 
{
  struct utillib_vector const * rules_vector = utillib_rule_index_rules(self->rule_index);
  struct utillib_symbol const * LHS;
  struct utillib_symbol const * FIRST;
  struct utillib_ll1_set * FIRST_SET;
  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, rule, rules_vector) {
    LHS=utillib_rule_lhs(rule);
    FIRST=utillib_vector_at(utillib_rule_rhs(rule), 0);
    if (utillib_symbol_kind(FIRST) == UT_SYMBOL_TERMINAL) {
      size_t index=utillib_rule_index_non_terminal_index(self->rule_index,
          utillib_symbol_value(LHS));
      FIRST_SET=utillib_vector_at(&self->FIRST, index);
      utillib_bitset_set(&FIRST_SET->bitset, utillib_symbol_value(FIRST));
    }
  }
}


