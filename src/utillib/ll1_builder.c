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
#include <assert.h>
#include <stdlib.h>
/**
 * \file utillib/ll1_builder.c
 * Engine for building LL(1) parser table.
 */

/**
 * JSON interfaces
 */

static struct utillib_json_value_t *
ll1_builder_set_json_array_create(struct utillib_ll1_set const *self,
                                  struct utillib_rule_index const *rule_index,
                                  bool is_eps) {
  struct utillib_vector const *terminals =
      utillib_rule_index_terminals(rule_index);
  struct utillib_json_value_t *array = utillib_json_array_create_empty();
  UTILLIB_VECTOR_FOREACH(struct utillib_symbol const *, symbol, terminals) {
    if (utillib_ll1_set_contains(self, symbol->value)) {
      utillib_json_array_push_back(
          array, utillib_json_string_create(&symbol->name, 0));
    }
  }
  if (self->flag) {
    utillib_json_array_push_back(
        array,
        utillib_json_string_create(
            is_eps ? &UTILLIB_SYMBOL_EPS->name : &UTILLIB_SYMBOL_EOF->name, 0));
  }
  return array;
}

static struct utillib_json_value_t *
ll1_builder_set_json_array2_create(struct utillib_ll1_set const *self,
                                   struct utillib_rule_index const *rule_index,
                                   bool is_eps) {
  struct utillib_json_value_t *array = utillib_json_array_create_empty();
  for (int i = 0; i < rule_index->non_terminals_size; ++i) {
    utillib_json_array_push_back(
        array, ll1_builder_set_json_array_create(&self[i], rule_index, is_eps));
  }
  return array;
}

struct utillib_json_value_t *
utillib_ll1_builder_json_object_create(struct utillib_ll1_builder const *self) {
  struct utillib_rule_index const *rule_index = self->rule_index;
  struct utillib_json_value_t *object = utillib_json_object_create_empty();
  struct utillib_json_value_t *FIRST =
      ll1_builder_set_json_array2_create(self->FIRST, rule_index, true);
  struct utillib_json_value_t *FOLLOW =
      ll1_builder_set_json_array2_create(self->FOLLOW, rule_index, false);
  utillib_json_object_push_back(object, "FIRST", FIRST);
  utillib_json_object_push_back(object, "FOLLOW", FOLLOW);
  return object;
}

/*
 * struct utillib_ll1_set interfaces
 * Wrapping a `struct utillib_bitset_set' may seen silly but
 * the `flag' field of `utillib_ll1_set' has different semantic
 * (being an `epsilon' or `eof')
 * depending on context so a `utillib_ll1_set' should have its
 * own way of doing things.
 */
void utillib_ll1_set_init(struct utillib_ll1_set *self, size_t symbols_size) {
  self->flag = false;
  utillib_bitset_init(&self->bitset, symbols_size);
}

bool utillib_ll1_set_union(struct utillib_ll1_set *self,
                           struct utillib_ll1_set const *other) {
  return utillib_bitset_union(&self->bitset, &other->bitset);
}

void utillib_ll1_set_insert(struct utillib_ll1_set *self, size_t value) {
  utillib_bitset_set(&self->bitset, value);
}

bool utillib_ll1_set_contains(struct utillib_ll1_set const *self,
                              size_t value) {
  return utillib_bitset_test(&self->bitset, value);
}

bool utillib_ll1_set_equal(struct utillib_ll1_set const *self,
                           struct utillib_ll1_set const *other) {
  return self->flag == other->flag &&
         utillib_bitset_equal(&self->bitset, &other->bitset);
}

/**
 * \function ll1_set_create
 * Creates a `utillib_ll1_set' with no terminal symbol
 * and no epsilon special symbol.
 * Only builder can create sets.
 * \param symbols_size The number of all the symbols.
 */
static struct utillib_ll1_set *ll1_set_create(size_t symbols_size) {
  struct utillib_ll1_set *self = malloc(sizeof *self);
  utillib_ll1_set_init(self, symbols_size);
  return self;
}

static void ll1_set_destroy(struct utillib_ll1_set *self) {
  utillib_ll1_set_destroy(self);
  free(self);
}

/*
 * Getter for different sets given
 * an symbol or a rule.
 * Simply for better readability and maintainness
 * because FOLLOW and FIRST are represented with the same
 * struct namely `utillib_ll1_set'.
 */

/**
 * \function ll1_builder_FIRST_get
 * Helper to get the `FIRST' for a particular non-terminal
 * symbol.
 */
static struct utillib_ll1_set *
ll1_builder_FIRST_get(struct utillib_ll1_builder *self,
                      struct utillib_symbol const *symbol) {
  assert (symbol->kind == UT_SYMBOL_NON_TERMINAL &&
      "Only non terminal symbols can have FIRST set");
  return &self->FIRST[utillib_rule_index_symbol_index(self->rule_index,
                                                      symbol)];
}

/**
 * \function ll1_builder_FOLLOW_get
 * Helper to get the `FOLLOW' for a particular non-terminal
 * symbol.
 */
static struct utillib_ll1_set *
ll1_builder_FOLLOW_get(struct utillib_ll1_builder const *self,
                       struct utillib_symbol const *symbol) {
  assert (symbol->kind == UT_SYMBOL_NON_TERMINAL &&
      "Only non terminal symbols can have FOLLOW set");
  return &self->FOLLOW[utillib_rule_index_symbol_index(self->rule_index,
                                                       symbol)];
}

/**
 * \function ll1_builder_FIRST_RULE_get
 * Helper to get the `FIRST' for a particular rule.
 */
static struct utillib_ll1_set *
ll1_builder_FIRST_RULE_get(struct utillib_ll1_builder const *self,
                           struct utillib_rule const *rule) {
  return &self->FIRST_RULE[rule->id];
}

/*
 * FIRST sets construction
 */

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
  struct utillib_vector const *rules_vector =
      utillib_rule_index_rules(rule_index);

  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, rule, rules_vector) {
    struct utillib_symbol const *LHS = rule->LHS;
    struct utillib_vector const *RHS = &rule->RHS;
    assert(utillib_vector_size(RHS) > 0 &&
           "Each `RHS' has a least one element");
    struct utillib_symbol const *FIRST = utillib_vector_front(RHS);
    struct utillib_ll1_set *LHS_FIRST = ll1_builder_FIRST_get(self, LHS);
    if (utillib_symbol_value(FIRST) == UT_SYM_EPS) {
      printf("ll1_builder_FIRST_partial_eval: RHS has eps, LHS %s\n",
          utillib_symbol_name(LHS));
      /* Do not test existence of `epsilon' in the bitset way, it hurts */
      LHS_FIRST->flag = true;
    } else if (utillib_symbol_kind(FIRST) == UT_SYMBOL_TERMINAL) {
      utillib_ll1_set_insert(LHS_FIRST, utillib_symbol_value(FIRST));
    }
  }
}

/**
 * \ll1_builder_FIRST_increamental
 * Makes an incremental update to the FIRST sets.
 * Visits the symbols on the right hand side sequentially while
 * merging their `FIRST's into that of the left hand side symbol and
 * stops at the first occurrence of a terminal symbol or a non-terminal
 * symbol whose `FIRST' does not contain `epsilon'.
 *
 * If the whole sequence contains exclusively non-terminal symbols with
 * `epsilon' inside their `FIRST's, the `FIRST' of the LHS symbol will
 * also contains `epsilon'.
 *
 * Notes the `last_eps' boolean is used to record whether the last-visited
 * symbol (a) is `epsilon' or (b) FIRST contains `epsilon'.
 *
 * \return Whether any changes took place in this incremental evaluation.
 */
static bool ll1_builder_FIRST_increamental(struct utillib_ll1_builder *self) {
  bool changed = false;
  bool last_eps;
  struct utillib_rule_index const *rule_index = self->rule_index;
  struct utillib_vector const *rules_vector =
      utillib_rule_index_rules(rule_index);

  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, rule, rules_vector) {
    struct utillib_symbol const *LHS = rule->LHS;
    struct utillib_vector const *RHS = &rule->RHS;
    struct utillib_ll1_set *LHS_FIRST = ll1_builder_FIRST_get(self, LHS);
    UTILLIB_VECTOR_FOREACH(struct utillib_symbol const *, symbol, RHS) {
      size_t RHS_value = symbol->value;
      if (RHS_value == UT_SYM_EPS) {
        /* Still not the timing to insert `epsilon' */
        last_eps = true;
        continue;
      }
      if (symbol->kind == UT_SYMBOL_TERMINAL) {
        if (!utillib_ll1_set_contains(LHS_FIRST, RHS_value)) {
          utillib_ll1_set_insert(LHS_FIRST, utillib_symbol_value(symbol));
          changed = true;
        }
        last_eps = false;
        /* Traversal ends with terminal symbol */
        break;
      }
      struct utillib_ll1_set *RHS_FIRST = ll1_builder_FIRST_get(self, symbol);
      changed = utillib_ll1_set_union(LHS_FIRST, RHS_FIRST);
      if (!(last_eps = RHS_FIRST->flag)) {
        /* Traversal ends with non-terminal symbol without `epsilon' in FIRST.
         */
        break;
      }
    }
    if (last_eps && !LHS_FIRST->flag) {
      LHS_FIRST->flag = true;
      changed = true;
    }
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
  struct utillib_vector const *rules_vector =
      utillib_rule_index_rules(rule_index);
  bool last_eps;

  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, rule, rules_vector) {
    struct utillib_ll1_set *FIRST = ll1_builder_FIRST_RULE_get(self, rule);
    UTILLIB_VECTOR_FOREACH(struct utillib_symbol const *, symbol, &rule->RHS) {
      size_t RHS_value = utillib_symbol_value(symbol);
      if (RHS_value == UT_SYM_EPS) {
        last_eps = true;
        continue;
      }
      if (utillib_symbol_kind(symbol) == UT_SYMBOL_TERMINAL) {
        utillib_ll1_set_insert(FIRST, RHS_value);
        last_eps = false;
        break;
      }
      struct utillib_ll1_set const *RHS_FIRST =
          ll1_builder_FIRST_get(self, symbol);
      utillib_ll1_set_union(FIRST, RHS_FIRST);
      if (!(last_eps = RHS_FIRST->flag)) {
        break;
      }
    }
    FIRST->flag = last_eps;
    if (FIRST->flag) {
      printf("ll1_builder_FIRST_finalize: This FIRST has eps, LHS %s\n",
          utillib_symbol_name(rule->LHS));
    }
  }
}

/*
 * FOLLOW construction
 */

/**
 * \function ll1_builder_FOLLOW_has_tailing
 * Helper to judge whether the right hand side of the rule is in the form
 * required by FOLLOW evaluation, i.e., `aBb', where `B' is of non-terminal
 * symbol.
 */

static bool ll1_builder_FOLLOW_has_tailing(struct utillib_vector const *RHS) {
  size_t RHS_size = utillib_vector_size(RHS);
  assert(RHS_size > 0 && "Right hand side of rule should not be empty");
  if (RHS_size < 2)
    return false;
  struct utillib_symbol const *PREV = utillib_vector_at(RHS, RHS_size - 2);
  return utillib_symbol_kind(PREV) == UT_SYMBOL_NON_TERMINAL;
}

/**
 * \function ll1_builder_FOLLOW_last_epsilon
 * Helper to judge whether the last symbol of a rule
 * has anything to do with `epsilon'.
 * \param LAST A symbol at the last of a `RHS'.
 * \return True if `LAST' is directly `epsilon'
 * or a non-terminal symbol whose `FIRST' contains
 * `epsilon'.
 */
static bool ll1_builder_FOLLOW_last_epsilon(struct utillib_ll1_builder *self,
                                            struct utillib_symbol const *LAST) {
  size_t LAST_value = utillib_symbol_value(LAST);
  if (LAST_value == UT_SYM_EPS)
    return true;
  if (utillib_symbol_kind(LAST) == UT_SYMBOL_TERMINAL)
    return false;
  struct utillib_ll1_set const *LAST_FIRST = ll1_builder_FIRST_get(self, LAST);
  if (LAST_FIRST->flag)
    return true;
  return false;
}

/**
 * \function ll1_builder_FOLLOW_partial_eval
 * Partial-evaluates the `FOLLOW's of all the non-terminal symbols.
 * First, Simply adds the special `end-of-input' symbol into the `FOLLOW'
 * of the special TOP symbol which starts the whole grammar.
 * Second, for all the rules of the form `A := aBb', if `b' is not `epsilon',
 * and `B' is a non-terminal symbol,
 * adds all the symbols in `First(b)' excluding `epsilon' into `FOLLOW(B)'.
 * Since `FIRST' is known now, this evaluation can be done in partial
 * evaluation.
 *
 */
#define ll1_builder_check_not_eof(value)                                       \
  do {                                                                         \
    assert((value) != UT_SYM_EOF &&                                            \
           "End-of-input special symbol should not appear");                   \
  } while (0)

static void ll1_builder_FOLLOW_partial_eval(struct utillib_ll1_builder *self) {
  struct utillib_rule_index const *rule_index = self->rule_index;
  struct utillib_vector const *rules_vector =
      utillib_rule_index_rules(rule_index);
  struct utillib_symbol const *TOP_symbol =
      utillib_rule_index_top_symbol(rule_index);
  struct utillib_ll1_set *TOP_FOLLOW = ll1_builder_FOLLOW_get(self, TOP_symbol);
  TOP_FOLLOW->flag = true;

  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, rule, rules_vector) {
    struct utillib_vector const *RHS = &rule->RHS;
    /* The form of rule does not match */
    if (!ll1_builder_FOLLOW_has_tailing(RHS))
      continue;
    size_t RHS_size = utillib_vector_size(RHS);
    struct utillib_symbol const *PREV = utillib_vector_at(RHS, RHS_size - 2);
    struct utillib_symbol const *LAST = utillib_vector_back(RHS);
    size_t LAST_value = utillib_symbol_value(LAST);
    ll1_builder_check_not_eof(LAST);
    if (LAST_value == UT_SYM_EPS)
      /* Do not let bitset see `epsilon'. It hurts */
      continue;
    struct utillib_ll1_set *PREV_FOLLOW = ll1_builder_FOLLOW_get(self, PREV);
    if (utillib_symbol_kind(LAST) == UT_SYMBOL_TERMINAL) {
      utillib_ll1_set_insert(PREV_FOLLOW, LAST_value);
    } else { /* non terminal */
      struct utillib_ll1_set const *LAST_FIRST =
          ll1_builder_FIRST_get(self, LAST);
      utillib_ll1_set_union(PREV_FOLLOW, LAST_FIRST);
    }
  }
}

/**
 * \function ll1_builder_FOLLOW_update
 * Updates self with other considering both bitset and flag.
 * Only FOLLOW construction need such operations as a whole.
 */
static bool ll1_builder_FOLLOW_update(struct utillib_ll1_set *self,
                                      struct utillib_ll1_set const *other) {
  bool changed = utillib_ll1_set_union(self, other);
  if (!self->flag && other->flag) {
    self->flag = true;
    changed = true;
  }
  return changed;
}

/**
 * \function ll1_builder_FIRST_contains_eps
 * Judges whether the FIRST of symbol contains epsilon.
 * Notes that the FIRST is not computed.
 * \param symbol The symbol whose FIRST is to judge.
 */

static bool ll1_builder_FIRST_contains_eps(struct utillib_ll1_builder *self,
  struct utillib_symbol const* symbol)
{
  if (symbol->value==UT_SYM_EPS)
    return true;
  /* terminal other than epsilon */
  if (symbol->kind==UT_SYMBOL_TERMINAL)
    return false;
  struct utillib_ll1_set const * FIRST=ll1_builder_FIRST_get(self, symbol);
  return FIRST->flag;
}


/**
 * \function ll1_builder_FOLLOW_incremental
 * Based on the following algorithm:
 * If the rule has form `A := aBb' and `b' satisfies
 * `ll1_builder_FOLLOW_last_epsilon' implying `epsilon'
 * can be derived from `b' (or `b' is epsilon itself), then what follows `A' can
 * also follows `B'.
 * Otherwise, if the rule has the form `A := aB', then
 * what follows `A' can also follows `B'.
 * Thus, we merge `FOLLOW' for `A' into that of `B'.
 * Notes that `b' here is in fact a sequence of symbol of any length
 * and FIRST(b) is computed on the fly rather than pre-computed.
 */

static bool ll1_builder_FOLLOW_incremental(struct utillib_ll1_builder *self) {
  struct utillib_rule_index const *rule_index = self->rule_index;
  struct utillib_vector const *rules_vector =
      utillib_rule_index_rules(rule_index);
  struct utillib_ll1_set *FIRST_SET;
  bool changed = false;

  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, rule, rules_vector) {
    struct utillib_vector const *RHS = &rule->RHS;
    struct utillib_symbol const *LHS = rule->LHS;
    struct utillib_symbol const *LAST = utillib_vector_back(RHS);
    struct utillib_ll1_set const *LHS_FOLLOW = ll1_builder_FOLLOW_get(self, LHS);
    if (utillib_symbol_kind(LAST) == UT_SYMBOL_NON_TERMINAL) {
      struct utillib_ll1_set *LAST_FOLLOW = ll1_builder_FOLLOW_get(self, LAST);
      changed = ll1_builder_FOLLOW_update(LAST_FOLLOW, LHS_FOLLOW);
    }
    size_t RHS_size = utillib_vector_size(RHS);
    for (int i=0; i<RHS_size-1; ++i ) {
      struct utillib_symbol const *PREV = utillib_vector_at(RHS, i);
      if (PREV->kind==UT_SYMBOL_TERMINAL)
        continue; /* Only non terminal haves FOLLOW */
      bool is_eps=true;
      for (int j=i+1; j<RHS_size; ++j) {
        struct utillib_symbol const * symbol=utillib_vector_at(RHS, j);
        if (!ll1_builder_FIRST_contains_eps(self, symbol)) {
          is_eps=false;
          break;
        }
      }
      if (is_eps) {
        struct utillib_ll1_set *PREV_FOLLOW = ll1_builder_FOLLOW_get(self, PREV);
        changed = ll1_builder_FOLLOW_update(PREV_FOLLOW, LHS_FOLLOW);
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
  size_t terminals_size = utillib_rule_index_terminals_size(rule_index);
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
  struct utillib_vector const *rules_vector =
      utillib_rule_index_rules(rule_index);
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
      printf("utillib_ll1_builder_build_table: This FIRST set "
          "has eps, so look into its FOLLOW set: %s\n", utillib_symbol_name(rule->LHS));

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
        printf("utillib_ll1_builder_build_table: "
            "this guy has eof in its FOLLOW: %s\n", utillib_symbol_name(LHS));
        utillib_vector2_set(table, LHS_index, UT_SYM_EOF, UTILLIB_RULE_EPS);
      }
    }
  }
}

/*
 * Error handling.
 */

UTILLIB_ETAB_BEGIN(utillib_ll1_error_kind)
UTILLIB_ETAB_ELEM_INIT(UT_LL1_EFIRST, "FIRST/FIRST conflict")
UTILLIB_ETAB_ELEM_INIT(UT_LL1_EFOLLOW, "FIRST/FOLLOW conflict")
UTILLIB_ETAB_END(utillib_ll1_error_kind);

/**
 * \function utillib_ll1_set_intersect
 * Judges whether `lhs' and `rhs' have common
 * elements.
 * \param about_flag Whether to compare the `flag' field
 * of the operands.
 * If both of the operands are `FIRST', flag should be considered
 * because it is a mark for `epsilon' being inside.
 * However, it is meaningless to tell a FIRST having `epsilon'
 * while a FOLLOW having `eof' by comparing their flags because
 * `flag' is of different semantic.
 *
 */
bool utillib_ll1_set_intersect(struct utillib_ll1_set const *lhs,
                               struct utillib_ll1_set const *rhs,
                               bool about_flag) {
  bool bitset_intersect =
      utillib_bitset_is_intersect(&lhs->bitset, &rhs->bitset);
  if (!about_flag)
    return bitset_intersect;
  return lhs->flag == rhs->flag && bitset_intersect;
}

/**
 * \function ll1_builder_error_create_as_EFIRST
 * Creates an error telling the offending (overlapping)
 * FIRST and its corresponding rules.
 */
static struct utillib_ll1_builder_error *
ll1_builder_error_create_as_EFIRST(struct utillib_rule_index const *rule_index,
                                   struct utillib_ll1_set const *lhs_FIRST,
                                   struct utillib_ll1_set const *rhs_FIRST,
                                   struct utillib_rule const *lhs_rule,
                                   struct utillib_rule const *rhs_rule) {
  struct utillib_ll1_builder_error *self = malloc(sizeof *self);
  self->values[0] = utillib_rule_json_object_create(lhs_rule, 0);
  self->values[1] =
      ll1_builder_set_json_array_create(lhs_FIRST, rule_index, true);
  self->values[2] = utillib_rule_json_object_create(rhs_rule, 0);
  self->values[3] =
      ll1_builder_set_json_array_create(rhs_FIRST, rule_index, true);
  self->kind = UT_LL1_EFIRST;
  return self;
}

/**
 * \funtion ll1_builder_error_create_as_EFOLLOW
 * Creates an error telling the in-trouble
 * `epsilon'-leading rule, the FOLLOW that
 * leads it to `epsilon' and the FIRST that
 * leads it to something non `epsilon'.
 */
static struct utillib_ll1_builder_error *
ll1_builder_error_create_as_EFOLLOW(struct utillib_rule_index const *rule_index,
                                    struct utillib_rule const *lhs_rule,
                                    struct utillib_ll1_set const *lhs_FIRST,
                                    struct utillib_symbol const *rhs_symbol,
                                    struct utillib_ll1_set const *rhs_FOLLOW) {
  struct utillib_ll1_builder_error *self = malloc(sizeof *self);
  self->kind = UT_LL1_EFOLLOW;
  self->values[0] = utillib_rule_json_object_create(lhs_rule, 0);
  self->values[1] =
      ll1_builder_set_json_array_create(lhs_FIRST, rule_index, true);
  self->values[2] = utillib_symbol_json_string_create(rhs_symbol);
  self->values[3] =
      ll1_builder_set_json_array_create(rhs_FOLLOW, rule_index, false);
  return self;
}

static void
utillib_ll1_builder_error_destroy(struct utillib_ll1_builder_error *self) {
  for (int i = 0; i < UT_LL1_ERR_VAL_MAX; ++i)
    if (self->values[i])
      utillib_json_value_destroy(self->values[i]);
  free(self);
}

/**
 * \function ll1_builder_check_impl
 * Checks whether fundamental conditions for a grammar to be LL(1) hold.
 * They are:
 * (a) Different rules having the same non terminal symbol on the left hand
 * side,
 * the FIRST of one rule does not intersect with another.
 *
 * (b) If `epsilon' can be deducted from one rule, then the FOLLOW of the LHS
 * symbol should not overlap the FIRST set of any other rules.
 *
 * In plain words, when the parser sees that the lookaheaded token
 * can make rule `A := epsilon' applied to the stack-top symbol `A',
 * the grammar should not surprise us with that same lookahead token
 * leading to a different rule.
 */

static void
ll1_builder_check_impl(struct utillib_ll1_builder *self,
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
      struct utillib_ll1_set const *LHS_FOLLOW =
          ll1_builder_FOLLOW_get(self, foo_rule->LHS);
      if (utillib_ll1_set_intersect(foo_FIRST, bar_FIRST, true)) {
        utillib_vector_push_back(
            &self->errors,
            ll1_builder_error_create_as_EFIRST(rule_index, foo_FIRST, bar_FIRST,
                                               foo_rule, bar_rule));
      }
      if (foo_FIRST->flag &&
          utillib_ll1_set_intersect(bar_FIRST, LHS_FOLLOW, false)) {
        utillib_vector_push_back(
            &self->errors,
            ll1_builder_error_create_as_EFOLLOW(rule_index, bar_rule, bar_FIRST,
                                                foo_rule->LHS, LHS_FOLLOW));
      }
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
  for (size_t i = 0; i < rule_index->non_terminals_size; ++i) {
    struct utillib_vector const *same_LHS_rules = &rule_index->LHS_index[i];
    ll1_builder_check_impl(self, same_LHS_rules);
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
