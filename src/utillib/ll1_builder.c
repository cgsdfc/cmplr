#include "ll1_builder.h"
#include "equal.h" // for utillib_equal_bool
#include <stdlib.h>

UTILLIB_ETAB_BEGIN(utillib_ll1_error_kind)
  UTILLIB_ETAB_ELEM_INIT(UT_LL1_OK, "Success")
  UTILLIB_ETAB_ELEM_INIT(UT_LL1_ENOTLL1, "Input is not LL(1)")
UTILLIB_ETAB_END(utillib_ll1_error_kind);

/*
 * Private interfaces
 */

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

static void ll1_builder_FIRST_partial_eval(struct utillib_ll1_builder *self)
{
  struct utillib_rule_index const *rule_index=self->rule_index;
  struct utillib_vector const * rules_vector = utillib_rule_index_rules(rule_index);
  struct utillib_symbol const * LHS;
  struct utillib_symbol const * FIRST;
  struct utillib_ll1_set * FIRST_SET;
  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, rule, rules_vector) {
    LHS=utillib_rule_lhs(rule);
    FIRST=utillib_vector_at(utillib_rule_rhs(rule), 0);
    if (utillib_symbol_kind(FIRST) == UT_SYMBOL_TERMINAL) {
      size_t index=utillib_rule_index_non_terminal_index(rule_index, utillib_symbol_value(LHS));
      FIRST_SET=utillib_vector_at(&self->FIRST, index);
      utillib_bitset_set(&FIRST_SET->bitset, utillib_symbol_value(FIRST));
    }
  }
}

static bool ll1_builder_FIRST_increamental(struct utillib_ll1_builder *self)
{
  bool changed=false;
  struct utillib_rule_index const *rule_index;
  struct utillib_vector const * rules_vector=utillib_rule_index_rules(rule_index);
  UTILLIB_VECTOR_FOREACH(struct utillib_rule const*, rule, rules_vector) {
    struct utillib_vector const * RHS=utillib_rule_rhs(rule);
    struct utillib_symbol const * LHS=utillib_rule_lhs(rule);
    UTILLIB_VECTOR_FOREACH(struct utillib_symbol const *, symbol, RHS) {
      if (utillib_symbol_kind(symbol) == UT_SYMBOL_TERMINAL) 
        break;
      size_t to_set_index=utillib_rule_index_non_terminal_index(rule_index, utillib_symbol_value(LHS));
      struct utillib_ll1_set * from_set=utillib_vector_at(&self->FIRST, 
          utillib_rule_index_non_terminal_index(rule_index, utillib_symbol_value(symbol)));
      struct utillib_ll1_set * to_set=utillib_vector_at(&self->FIRST, to_set_index);
      changed=utillib_bitset_union(&to_set->bitset, &from_set->bitset);
    }
  }
  return changed;
}

static void ll1_builder_FIRST_finalize(struct utillib_ll1_builder *self)
{
  struct utillib_rule_index const *rule_index=self->rule_index;
  bool last_eps;
  for (size_t rule_id =0, size=utillib_rule_index_rules_size(rule_index);
      rule_id < size; ++rule_id) {
    struct utillib_rule const * rule=utillib_rule_index_rule_at(rule_index, rule_id);
    struct utillib_ll1_set * FIRST=utillib_vector_at(&self->FIRST_RULE, rule_id);
    struct utillib_vector const * RHS=utillib_rule_rhs(rule);
    UTILLIB_VECTOR_FOREACH(struct utillib_symbol const *, symbol, RHS) {
      if (utillib_symbol_kind(symbol) == UT_SYMBOL_TERMINAL) {
        utillib_bitset_set(&FIRST->bitset, utillib_symbol_value(symbol));
        last_eps=false;
        break;
      } else {
        size_t index=utillib_rule_index_non_terminal_index(rule_index, utillib_symbol_value(symbol));
        struct utillib_ll1_set const * RHS_FIRST=utillib_vector_at(&self->FIRST, index);
        utillib_bitset_union(&FIRST->bitset, &RHS_FIRST->bitset);
        last_eps=utillib_symbol_value(symbol) == UT_SYM_EPS;
      }
    }
    FIRST->flag=last_eps;
  } 
}

static void ll1_builder_FOLLOW_partial_eval(struct utillib_ll1_builder *self)
{
  struct utillib_rule_index const *rule_index=self->rule_index;
  struct utillib_ll1_set * FOLLOW_SET;

  FOLLOW_SET=utillib_vector_at(&self->FOLLOW, 
      utillib_rule_index_non_terminal_index(rule_index, 
        utillib_symbol_value(utillib_rule_index_top_symbol(rule_index))));
  utillib_bitset_set(&FOLLOW_SET->bitset, utillib_symbol_value(UTILLIB_SYMBOL_EOF));
}

static bool ll1_builder_FOLLOW_incremental(struct utillib_ll1_builder *self)
{
  struct utillib_rule_index const *rule_index=self->rule_index;
  struct utillib_vector const * rules_vector = utillib_rule_index_rules(rule_index);
  struct utillib_symbol const * LAST;
  struct utillib_symbol const* LHS;
  struct utillib_symbol const* symbol;
  struct utillib_vector const * RHS;
  struct utillib_ll1_set * FIRST_SET;
  struct utillib_ll1_set * FOLLOW_SET;
  size_t size;
  int LAST_value;
  int LHS_value;
  bool changed=false;

  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, rule, rules_vector) {
    LAST=utillib_vector_back(utillib_rule_rhs(rule));
    LHS=utillib_rule_lhs(rule);
    LHS_value=utillib_symbol_value(LHS);
    RHS=utillib_rule_rhs(rule);
    size_t index=utillib_rule_index_non_terminal_index(rule_index,
        utillib_symbol_value(LHS));
    FOLLOW_SET=utillib_vector_at(&self->FOLLOW, index);
    LAST_value=utillib_symbol_value(LAST);
    if (LAST_value == UT_SYM_EPS) {
      if (!FOLLOW_SET->flag) {
        FOLLOW_SET->flag=true;
        changed=true;
      }
    } else if (utillib_symbol_kind(LAST) == UT_SYMBOL_TERMINAL) {
      if (!utillib_bitset_test(&FOLLOW_SET->bitset, LAST_value)) {
          utillib_bitset_set(&FOLLOW_SET->bitset, LAST_value);
          changed=true;
      }
    } else { /* non_terminal */
      FIRST_SET=utillib_vector_at(&self->FIRST,
          utillib_rule_index_non_terminal_index(rule_index, LAST_value));
      changed=utillib_bitset_union(&FOLLOW_SET->bitset, &FIRST_SET->bitset);
      if (FIRST_SET->flag && (size=utillib_vector_size(RHS)) > 1
          && utillib_symbol_kind(symbol=utillib_vector_at(RHS, size-2)) 
          == UT_SYMBOL_NON_TERMINAL) {
        FOLLOW_SET=utillib_vector_at(&self->FOLLOW,
            utillib_rule_index_non_terminal_index(rule_index, LHS_value));
        struct utillib_ll1_set * FOLLOW2=utillib_vector_at(&self->FOLLOW,
            utillib_rule_index_non_terminal_index(rule_index, utillib_symbol_value(symbol)));
        changed=utillib_bitset_union(&FOLLOW2->bitset, &FOLLOW_SET->bitset);
      }
    }
  }
  return changed;
}

static void ll1_builder_fixed_point_loop(struct utillib_ll1_builder *self,
    void ( * partail_eval ) (struct utillib_ll1_builder *),
    bool ( * incremental ) (struct utillib_ll1_builder *))
{
  size_t size=utillib_rule_index_non_terminals_size(self->rule_index);
  partail_eval(self);
  while(incremental(self))
    ;
}

static void ll1_builder_build_FIRST_FOLLOW(struct utillib_ll1_builder *self)
{
  ll1_builder_fixed_point_loop(self, 
      ll1_builder_FIRST_partial_eval,
      ll1_builder_FIRST_increamental
    );
  ll1_builder_fixed_point_loop(self,
      ll1_builder_FOLLOW_partial_eval,
      ll1_builder_FOLLOW_incremental
    );
  ll1_builder_FIRST_finalize(self);
}

/*
 * Public interfaces
 */

void utillib_ll1_builder_init(struct utillib_ll1_builder *self,
    struct utillib_rule_index const * rule_index)
{
  utillib_vector_init(&self->FIRST);
  utillib_vector_init(&self->FOLLOW);
  utillib_vector_init(&self->errors);
  utillib_vector_init(&self->FIRST_RULE);

  self->rule_index=rule_index;
  size_t non_terminals_size=utillib_rule_index_non_terminals_size(rule_index);
  size_t terminals_size=utillib_rule_index_terminals_size(rule_index);
  size_t symbols_size=non_terminals_size+terminals_size;
  size_t rules_size=utillib_rule_index_rules_size(rule_index);

  utillib_vector_reserve(&self->FIRST, non_terminals_size);
  utillib_vector_reserve(&self->FOLLOW, non_terminals_size);
  utillib_vector_reserve(&self->FIRST_RULE, rules_size);

  for (size_t i=0;i < non_terminals_size; ++i) {
    utillib_vector_push_back(&self->FIRST, ll1_set_create(symbols_size));
    utillib_vector_push_back(&self->FOLLOW, ll1_set_create(symbols_size));
  }
  for (size_t i=0;i < rules_size; ++i) {
    utillib_vector_push_back(&self->FIRST_RULE, ll1_set_create(symbols_size));
  }
  ll1_builder_build_FIRST_FOLLOW(self);
}

void utillib_ll1_builder_destroy(struct utillib_ll1_builder *self)
{
  utillib_vector_destroy_owning(&self->FIRST, (utillib_destroy_func_t*) ll1_set_destroy);
  utillib_vector_destroy_owning(&self->FIRST_RULE, (utillib_destroy_func_t*) ll1_set_destroy);
  utillib_vector_destroy_owning(&self->FOLLOW, (utillib_destroy_func_t*) ll1_set_destroy);
  utillib_vector_destroy(&self->errors);
}

void utillib_ll1_builder_build_table(struct utillib_ll1_builder *self,
    struct utillib_vector2 * table)
{
  struct utillib_ll1_set const *FIRST;
  struct utillib_ll1_set const *FOLLOW;
  struct utillib_rule const * rule;
  struct utillib_symbol const * LHS;
  struct utillib_rule_index const * rule_index=self->rule_index;
  size_t non_terminals_size=utillib_rule_index_non_terminals_size(rule_index);
  size_t terminals_size=utillib_rule_index_terminals_size(rule_index);
  size_t symbols_size=non_terminals_size+terminals_size;
  utillib_vector2_init(table, non_terminals_size + 1, terminals_size + 1);

  for (size_t rule_id=0, size=utillib_rule_index_rules_size(rule_index);
      rule_id < size; ++ rule_id) {
    FIRST=utillib_vector_at(&self->FIRST_RULE, rule_id);
    rule=utillib_vector_at(utillib_rule_index_rules(rule_index), rule_id);
    LHS=utillib_rule_lhs(rule);
    size_t LHS_index=utillib_rule_index_non_terminal_index(rule_index, utillib_symbol_value(LHS));
    for (size_t symbol_id=0; symbol_id<symbols_size; ++symbol_id) {
      if (utillib_bitset_test(&FIRST->bitset, symbol_id)) {
         utillib_vector2_set(table, 
             LHS_index, utillib_rule_index_terminal_index(rule_index, symbol_id),
             (utillib_element_t) rule);
      }
    }
    if (FIRST->flag) {
      FOLLOW=utillib_vector_at(&self->FOLLOW, LHS_index);
      for (size_t symbol_id=0; symbol_id<symbols_size; ++symbol_id) {
        if (utillib_bitset_test(&FOLLOW->bitset, symbol_id)) {
          utillib_vector2_set(table,
              LHS_index, utillib_rule_index_terminal_index(rule_index, symbol_id), 
              (utillib_element_t) UTILLIB_RULE_NULL);
        }
      }
    }
  }
}

int utillib_ll1_builder_check(struct utillib_ll1_builder *self)
{



}

