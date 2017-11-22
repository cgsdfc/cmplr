/*
   Cmplr Library
   Copyright (C) 2017-2018 Cong Feng <cgsdfc@126.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301 USA

*/
#include "ll1_builder.h"
#include "ll1_builder_impl.h"
#include "print.h"

#include <assert.h>
#include <stdlib.h>

/**
 * \file utillib/ll1_builder.c
 * Engine for building LL(1) parser table.
 */

/*
 * FIRST sets construction
 */

/**
 * \function ll1_builder_FIRST_updated
 * This function treats the sequence of symbols represented
 * by `RHS' as a whole and union its FIRST into `self'. It
 * returns whether `self' was updated.
 * \param builder to look up FIRST of non terminals from it.
 * \param RHS a sequence of symbols from which FIRST is virtually
 * computed.
 * \return Whether self was updated.
 */

bool ll1_builder_FIRST_updated(struct utillib_ll1_builder *self,
                               struct utillib_ll1_set *updated_set,
                               struct utillib_vector const *RHS) {
  bool last_eps = false;
  bool changed = false;
  struct utillib_ll1_set const *FIRST = NULL;

  UTILLIB_VECTOR_FOREACH(struct utillib_symbol const *, symbol, RHS) {
    if (symbol == UTILLIB_SYMBOL_EPS) {
      last_eps = true;
      continue;
    }
    if (symbol->kind == UT_SYMBOL_TERMINAL) {
      if (utillib_ll1_set_insert_updated(updated_set, symbol))
        changed = true;
      last_eps = false;
      break;
    }
    FIRST = ll1_builder_FIRST_get(self, symbol);
    if (utillib_ll1_set_union_updated(updated_set, FIRST))
      changed = true;
    if (!(last_eps = FIRST->flag))
      break;
  }
  if (last_eps &&
      utillib_ll1_set_insert_updated(updated_set, UTILLIB_SYMBOL_EPS))
    changed = true;
  return changed;
}
/**
 * \function ll1_builder_FIRST_partial_eval
 * Does a partial evaluation on FIRST of each
 * non terminal.
 *
 * For any rule having the form `X := aA' where `a' is a terminal
 * symbol, add `a' to the intermedia first set of `X'.
 */
static void ll1_builder_FIRST_partial_eval(struct utillib_ll1_builder *self) {
  struct utillib_rule_index const *rule_index = self->rule_index;
  struct utillib_vector const *rules_vector = &rule_index->rules;

  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, rule, rules_vector) {
    struct utillib_symbol const *LHS = rule->LHS;
    struct utillib_vector const *RHS = &rule->RHS;
    assert(utillib_vector_size(RHS) > 0 &&
           "Each `RHS' has a least one element");
    struct utillib_symbol const *first_symbol = utillib_vector_front(RHS);
    struct utillib_ll1_set *LHS_FIRST = ll1_builder_FIRST_get(self, LHS);
    if (first_symbol->kind != UT_SYMBOL_TERMINAL)
      continue;
    utillib_ll1_set_insert_updated(LHS_FIRST, first_symbol);
  }
}

/**
 * \ll1_builder_FIRST_increamental
 * Makes an incremental update to the FIRST sets.
 *
 * \return Whether any changes took place in this incremental evaluation.
 */
static bool ll1_builder_FIRST_increamental(struct utillib_ll1_builder *self) {
  bool changed = false;
  struct utillib_rule_index const *rule_index = self->rule_index;
  struct utillib_vector const *rules_vector = &rule_index->rules;

  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, rule, rules_vector) {
    struct utillib_symbol const *LHS = rule->LHS;
    struct utillib_vector const *RHS = &rule->RHS;
    struct utillib_ll1_set *LHS_FIRST = ll1_builder_FIRST_get(self, LHS);
    if (ll1_builder_FIRST_updated(self, LHS_FIRST, RHS))
      changed = true;
  }
  return changed;
}

/**
 * \function ll1_builder_FIRST_finalize
 * Similar to `ll1_builder_FIRST_increamental', goes through
 * the right hand side symbols of a rule to evaluate the `FIRST'
 * of each rule.
 * However, it does not return boolean value to indicate changes.
 * This is needed to be run only once to fill the `self->FIRST_RULE'
 * field which means `FIRST sets for all the rules'.
 */
static void ll1_builder_FIRST_finalize(struct utillib_ll1_builder *self) {
  struct utillib_rule_index const *rule_index = self->rule_index;
  struct utillib_vector const *rules_vector = &rule_index->rules;

  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, rule, rules_vector) {
    struct utillib_ll1_set *FIRST = ll1_builder_FIRST_RULE_get(self, rule);
    struct utillib_vector const *RHS = &rule->RHS;
    ll1_builder_FIRST_updated(self, FIRST, RHS);
  }
}

/*
 * FOLLOW construction
 */

/**
 * \function ll1_builder_FOLLOW_partial_eval
 * Partial-evaluates the `FOLLOW's of all the non-terminal symbols.
 * First, Simply adds the special `end-of-input' symbol into the `FOLLOW'
 * of the special TOP symbol which starts the whole grammar.
 * Second, for all the rules of the form `A := aBb' where `b' is a sequence
 * of symbols and `B' is a non-terminal symbol,
 * adds all the symbols in `First(b)' excluding `epsilon' into `FOLLOW(B)'.
 *
 * Notes that `b' is an arbitrary sequence of symbols and we emulate its
 * FIRST by the same way as done in `ll1_builder_FIRST_finalize'.
 * That is constantly
 *
 */
#define ll1_builder_check_not_eof(value)                                       \
  do {                                                                         \
    assert((value) != UT_SYM_EOF &&                                            \
           "End-of-input special symbol should not appear");                   \
  } while (0)

static void ll1_builder_FOLLOW_partial_eval(struct utillib_ll1_builder *self) {
  struct utillib_rule_index const *rule_index = self->rule_index;
  struct utillib_vector const *rules_vector = &rule_index->rules;
  struct utillib_symbol const *TOP_symbol =
      utillib_rule_index_top_symbol(rule_index);
  struct utillib_ll1_set *TOP_FOLLOW = ll1_builder_FOLLOW_get(self, TOP_symbol);
  TOP_FOLLOW->flag = true;

  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, rule, rules_vector) {
    struct utillib_vector const *RHS = &rule->RHS;
    /* The form of rule does not match */
    size_t RHS_size = utillib_vector_size(RHS);
    if (RHS_size < 2) {
      continue;
    }
    for (int i = 0; i < RHS_size - 1; ++i) {
      struct utillib_symbol const *PREV = utillib_vector_at(RHS, i);
      if (PREV->kind == UT_SYMBOL_TERMINAL)
        continue;
      struct utillib_ll1_set *PREV_FOLLOW = ll1_builder_FOLLOW_get(self, PREV);
      for (int j = i + 1; j < RHS_size; ++j) {
        struct utillib_symbol const *LAST = utillib_vector_at(RHS, j);
        if (LAST == UTILLIB_SYMBOL_EPS)
          continue;
        if (LAST->kind == UT_SYMBOL_TERMINAL) {
          utillib_ll1_set_insert_updated(PREV_FOLLOW, LAST);
          break;
        }
        /* non terminal */
        struct utillib_ll1_set const *LAST_FIRST =
            ll1_builder_FIRST_get(self, LAST);
        utillib_ll1_set_union_updated(PREV_FOLLOW, LAST_FIRST);
        if (!LAST_FIRST->flag)
          break;
      }
    }
  }
}

/**
 * \function ll1_builder_FIRST_contains_eps
 * Judges whether the FIRST of symbol contains epsilon.
 * Notes that the FIRST is not computed.
 * \param symbol The symbol whose FIRST is to judge.
 */

static bool
ll1_builder_FIRST_contains_eps(struct utillib_ll1_builder *self,
                               struct utillib_symbol const *symbol) {
  if (symbol->value == UT_SYM_EPS)
    return true;
  /* terminal other than epsilon */
  if (symbol->kind == UT_SYMBOL_TERMINAL)
    return false;
  struct utillib_ll1_set const *FIRST = ll1_builder_FIRST_get(self, symbol);
  return FIRST->flag;
}

static bool
utillib_ll1_set_FOLLOW_updated(struct utillib_ll1_set *self,
                               struct utillib_ll1_set const *other) {
  bool changed = false;
  if (!self->flag && other->flag) {
    self->flag = true;
    changed = true;
  }
  if (utillib_ll1_set_union_updated(self, other))
    changed = true;
  return changed;
}

/**
 * \function ll1_builder_FOLLOW_incremental
 * Based on the following algorithm:
 * If the rule has form `A := aBb' and `epsilon'
 * can be derived from `b', then what follows `A' can
 * also follows `B'.
 * Otherwise, if the rule has the form `A := aB', then
 * what follows `A' can also follows `B'.
 * Thus, we merge `FOLLOW' for `A' into that of `B'.
 * Notes that `b' here is in fact a sequence of symbol of any length
 * and FIRST(b) is computed on the fly rather than pre-computed.
 */

static bool ll1_builder_FOLLOW_incremental(struct utillib_ll1_builder *self) {
  struct utillib_rule_index const *rule_index = self->rule_index;
  struct utillib_vector const *rules_vector = &rule_index->rules;
  bool changed = false;

  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, rule, rules_vector) {
    struct utillib_vector const *RHS = &rule->RHS;
    size_t RHS_size = utillib_vector_size(RHS);
    struct utillib_symbol const *LHS = rule->LHS;
    struct utillib_symbol const *LAST = utillib_vector_back(RHS);
    struct utillib_ll1_set const *LHS_FOLLOW =
        ll1_builder_FOLLOW_get(self, LHS);

    if (utillib_symbol_kind(LAST) == UT_SYMBOL_NON_TERMINAL) {
      struct utillib_ll1_set *LAST_FOLLOW = ll1_builder_FOLLOW_get(self, LAST);
      if (utillib_ll1_set_FOLLOW_updated(LAST_FOLLOW, LHS_FOLLOW))
        changed = true;
    }
    for (int i = 0; i < RHS_size - 1; ++i) {
      struct utillib_symbol const *PREV = utillib_vector_at(RHS, i);
      if (PREV->kind == UT_SYMBOL_TERMINAL)
        continue; /* Only non terminal haves FOLLOW */
      bool is_eps = true;
      for (int j = i + 1; j < RHS_size; ++j) {
        struct utillib_symbol const *symbol = utillib_vector_at(RHS, j);
        if (!ll1_builder_FIRST_contains_eps(self, symbol)) {
          is_eps = false;
          break;
        }
      }
      if (is_eps) {
        struct utillib_ll1_set *PREV_FOLLOW =
            ll1_builder_FOLLOW_get(self, PREV);
        if (utillib_ll1_set_FOLLOW_updated(PREV_FOLLOW, LHS_FOLLOW))
          changed = true;
      }
    }
  }
  return changed;
}

/**
 * \function ll1_builder_fixed_point_loop
 * It is a helper to perform fixed-pointed evaluation
 * carried by 2 input arguments namely `partail_eval'
 * and `incremental'.
 */
static void ll1_builder_fixed_point_loop(
    struct utillib_ll1_builder *self,
    void (*partail_eval)(struct utillib_ll1_builder *),
    bool (*incremental)(struct utillib_ll1_builder *)) {
  partail_eval(self);
  while (incremental(self))
    ;
}

/**
 * \function ll1_builder_build_FIRST_FOLLOW
 * Performs all the set-evaluations in one step.
 */
static void ll1_builder_build_FIRST_FOLLOW(struct utillib_ll1_builder *self) {
  ll1_builder_fixed_point_loop(self, ll1_builder_FIRST_partial_eval,
                               ll1_builder_FIRST_increamental);
  ll1_builder_fixed_point_loop(self, ll1_builder_FOLLOW_partial_eval,
                               ll1_builder_FOLLOW_incremental);
  ll1_builder_FIRST_finalize(self);
}

/*
 * Public interfaces
 */

/**
 * \function utillib_ll1_builder_init
 * Builds the sets.
 *
 */
void utillib_ll1_builder_init(struct utillib_ll1_builder *self,
                              struct utillib_rule_index *rule_index) {
  utillib_vector_init(&self->errors);

  self->rule_index = rule_index;
  size_t symbols_size = utillib_rule_index_symbols_size(rule_index);
  size_t non_terminals_size = utillib_rule_index_non_terminals_size(rule_index);
  size_t rules_size = utillib_rule_index_rules_size(rule_index);

  self->FIRST = malloc(sizeof self->FIRST[0] * non_terminals_size);
  self->FOLLOW = malloc(sizeof self->FOLLOW[0] * non_terminals_size);
  self->FIRST_RULE = malloc(sizeof self->FIRST_RULE[0] * rules_size);

  for (size_t i = 0; i < non_terminals_size; ++i) {
    utillib_ll1_set_init(&self->FIRST[i], symbols_size);
    utillib_ll1_set_init(&self->FOLLOW[i], symbols_size);
  }
  for (size_t i = 0; i < rules_size; ++i) {
    utillib_ll1_set_init(&self->FIRST_RULE[i], symbols_size);
  }

  ll1_builder_build_FIRST_FOLLOW(self);
}

/**
 * \function utillib_ll1_builder_build_table
 * Builds the LL(1) table according to FIRST and FOLLOW sets.
 * For all the rules, if the FIRST of it contains terminal symbol
 * `a', then there will be an entry `M[LHS][a]=rule' in the table.
 * Also, if FIRST contains `epsilon', then for all the `b' in FOLLOW,
 * there will be an entry `M[b][LHS]=LHS := epsilon' in the table.
 * Any entry not filled in the above ways will be `NULL' to indicate
 * error.
 * \param table The table to be filled in.
 */
void utillib_ll1_builder_build_table(struct utillib_ll1_builder *self,
                                     struct utillib_vector2 *table) {
  struct utillib_rule_index const *rule_index = self->rule_index;
  struct utillib_vector const *rules_vector = &rule_index->rules;
  size_t symbols_size = rule_index->symbols_size;
  utillib_vector2_init(table, rule_index->non_terminals_size,
                       rule_index->terminals_size);

  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, rule, rules_vector) {
    struct utillib_ll1_set const *FIRST =
        ll1_builder_FIRST_RULE_get(self, rule);
    struct utillib_symbol const *LHS = rule->LHS;
    size_t LHS_index = utillib_rule_index_symbol_index(rule_index, LHS);
    for (size_t symbol_id = 1; symbol_id < symbols_size; ++symbol_id) {
      if (utillib_ll1_set_contains(FIRST, symbol_id)) {
        struct utillib_symbol const *symbol =
            utillib_rule_index_symbol_at(rule_index, symbol_id);
        size_t symbol_index =
            utillib_rule_index_symbol_index(rule_index, symbol);
        utillib_vector2_set(table, LHS_index, symbol_index, rule);
      }
    } /* epsilon */
    if (FIRST->flag) {

      struct utillib_ll1_set const *FOLLOW = ll1_builder_FOLLOW_get(self, LHS);
      for (size_t symbol_id = 1; symbol_id < symbols_size; ++symbol_id) {
        if (utillib_ll1_set_contains(FOLLOW, symbol_id)) {
          struct utillib_symbol const *symbol =
              utillib_rule_index_symbol_at(rule_index, symbol_id);
          size_t symbol_index =
              utillib_rule_index_symbol_index(rule_index, symbol);
          utillib_vector2_set(table, LHS_index, symbol_index, UTILLIB_RULE_EPS);
        }
      }
      if (FOLLOW->flag) { /* special `eof' */
        utillib_vector2_set(table, LHS_index, UT_SYM_EOF, UTILLIB_RULE_EPS);
      }
    }
  }
}

/**
 * \function ll1_builder_check_EFIRST
 * Checks for the FIRST/FIRST conflict.
 * The FIRST of rules with the same LHS should not intersect with each other.
 */

static void
ll1_builder_check_EFIRST(struct utillib_ll1_builder *self,
                         struct utillib_vector const *same_LHS_rules) {
  struct utillib_rule_index const *rule_index = self->rule_index;
  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, foo_rule,
                         same_LHS_rules) {
    UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, bar_rule,
                           same_LHS_rules) {
      if (foo_rule->id <= bar_rule->id)
        continue;
      struct utillib_ll1_set const *foo_FIRST =
          ll1_builder_FIRST_RULE_get(self, foo_rule);
      struct utillib_ll1_set const *bar_FIRST =
          ll1_builder_FIRST_RULE_get(self, bar_rule);
      if (utillib_ll1_set_intersect(foo_FIRST, bar_FIRST, true)) {
        utillib_vector_push_back(
            &self->errors,
            ll1_builder_error_create_as_EFIRST(rule_index, foo_FIRST, bar_FIRST,
                                               foo_rule, bar_rule));
      }
    }
  }
}

/**
 * \function ll1_builder_check_EFOLLOW
 * Checks that if one rule deducts epsilon, the FOLLOWs of other rule
 * should intersect with its FIRST.
 */

static void
ll1_builder_check_EFOLLOW(struct utillib_ll1_builder *self,
                          struct utillib_vector const *same_LHS_rules) {
  struct utillib_rule_index const *rule_index = self->rule_index;
  bool deducts_eps = false;
  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, foo_rule,
                         same_LHS_rules) {
    struct utillib_ll1_set const *foo_FIRST =
        ll1_builder_FIRST_RULE_get(self, foo_rule);
    if (foo_FIRST->flag) {
      deducts_eps = true;
      break;
    }
  }
  if (!deducts_eps)
    return;
  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, bar_rule,
                         same_LHS_rules) {
    struct utillib_ll1_set const *bar_FIRST =
        ll1_builder_FIRST_get(self, bar_rule->LHS);
    struct utillib_symbol const *LHS = bar_rule->LHS;
    struct utillib_ll1_set const *LHS_FOLLOW =
        ll1_builder_FOLLOW_get(self, LHS);

    if (utillib_ll1_set_intersect(bar_FIRST, LHS_FOLLOW, false)) {
      utillib_vector_push_back(
          &self->errors, ll1_builder_error_create_as_EFOLLOW(
                             rule_index, bar_rule, bar_FIRST, LHS, LHS_FOLLOW));
    }
  }
}

/**
 * \function utillib_ll1_builder_check
 * Checks for LL(1) confirmness of the input based on
 * FIRST and FOLLOW.
 * \return The number of error (conflict) found.
 */
int utillib_ll1_builder_check(struct utillib_ll1_builder *self) {
  struct utillib_rule_index *rule_index = self->rule_index;
  utillib_rule_index_build_LHS_index(rule_index);
  for (int i = 0; i < rule_index->non_terminals_size; ++i) {
    struct utillib_vector const *same_LHS_rules = &rule_index->LHS_index[i];
    ll1_builder_check_EFIRST(self, same_LHS_rules);
    ll1_builder_check_EFOLLOW(self, same_LHS_rules);
  }
  return utillib_vector_size(&self->errors);
}

/**
 * \function utillib_ll1_builder_destroy
 */
void utillib_ll1_builder_destroy(struct utillib_ll1_builder *self) {
  struct utillib_rule_index const *rule_index = self->rule_index;
  size_t non_terminals_size = utillib_rule_index_non_terminals_size(rule_index);
  size_t rules_size = utillib_rule_index_rules_size(rule_index);

  for (size_t i = 0; i < non_terminals_size; ++i) {
    utillib_ll1_set_destroy(&self->FIRST[i]);
    utillib_ll1_set_destroy(&self->FOLLOW[i]);
  }
  for (size_t i = 0; i < rules_size; ++i) {
    utillib_ll1_set_destroy(&self->FIRST_RULE[i]);
  }

  utillib_vector_destroy_owning(&self->errors,
                                (void *)utillib_ll1_builder_error_destroy);
  free(self->FIRST);
  free(self->FOLLOW);
  free(self->FIRST_RULE);
}

void utillib_ll1_builder_print_errors(struct utillib_ll1_builder *self) {
  struct utillib_vector const *errors = &self->errors;
  UTILLIB_VECTOR_FOREACH(struct utillib_ll1_builder_error const *, err,
                         errors) {
    ll1_builder_print_error(err);
  }
  size_t error_size = utillib_vector_size(errors);
  utillib_error_printf("%lu errors detected.\n", error_size);
}
