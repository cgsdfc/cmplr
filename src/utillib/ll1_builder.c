#include "ll1_builder.h"
#include "equal.h" // for utillib_equal_bool
#include <stdlib.h>

UTILLIB_ETAB_BEGIN(utillib_ll1_error_kind)
UTILLIB_ETAB_ELEM_INIT(UT_LL1_OK, "Success")
UTILLIB_ETAB_ELEM_INIT(UT_LL1_ENOTLL1, "Input is not LL(1)")
UTILLIB_ETAB_END(utillib_ll1_error_kind);

/**
 * JSON interfaces
 */

UTILLIB_JSON_OBJECT_FIELD_BEGIN(LL1BuilderSet_Fields)
  UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_ll1_set, "epsilon", flag, utillib_json_bool_create)
  UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_ll1_set, "elements", bitset, utillib_bitset_json_array_create)
UTILLIB_JSON_OBJECT_FIELD_END(LL1BuilderSet_Fields);

static utillib_json_value_t *
ll1_builder_set_json_object_create(void *base, size_t offset)
{
  return utillib_json_object_create(base, offset, LL1BuilderSet_Fields);
}

static utillib_json_value_t *
ll1_builder_set_json_array_create(void *base, size_t offset) 
{
  utillib_json_array_create_from_vector(base, offset, ll1_builder_set_json_object_create);
}
  
UTILLIB_JSON_OBJECT_FIELD_BEGIN(LL1Builder_Fields)
  UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_ll1_builder, "FIRST", FIRST, ll1_builder_set_json_array_create)
  UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_ll1_builder, "FOLLOW", FOLLOW, ll1_builder_set_json_array_create)
UTILLIB_JSON_OBJECT_FIELD_END(LL1Builder_Fields);

utillib_json_value_t* utillib_ll1_builder_json_object_create(void *base, size_t offset)
{
  return utillib_json_object_create(base, offset, LL1Builder_Fields);
}

/*
 * Private interfaces
 */

/**
 * \function ll1_set_create
 * Creates a `utillib_ll1_set' with no terminal symbol
 * and no epsilon special symbol.
 * \param N The number of all the symbols.
 */
static struct utillib_ll1_set *ll1_set_create(size_t N) {
  struct utillib_ll1_set *self = malloc(sizeof *self);
  self->flag = false;
  utillib_bitset_init(&self->bitset, N);
  return self;
}

static void ll1_set_destroy(struct utillib_ll1_set *self) {
  utillib_bitset_destroy(&self->bitset);
  free(self);
}

/**
 * \function ll1_builder_FIRST_get
 * Helper to get the `FIRST' for a particular non-terminal
 * symbol.
 */
static struct utillib_ll1_set *
ll1_builder_FIRST_get(struct utillib_ll1_builder *self,
                      struct utillib_symbol const *symbol) {
  return utillib_vector_at(&self->FIRST,
                           utillib_rule_index_non_terminal_index(
                               self->rule_index, utillib_symbol_value(symbol)));
}

/**
 * \function ll1_builder_FIRST_partial_eval
 * Does a partial evaluation of the intermedia first set
 * before incremental evaluation.
 * For any rule having the form `X := aA' where `a' is a terminal
 * symbol, add `a' to the intermedia first set of `X'.
 */
static void ll1_builder_FIRST_partial_eval(struct utillib_ll1_builder *self) {
  struct utillib_rule_index const *rule_index = self->rule_index;
  struct utillib_vector const *rules_vector =
      utillib_rule_index_rules(rule_index);

  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, rule, rules_vector) {
    struct utillib_symbol const *LHS = utillib_rule_lhs(rule);
    /* Each `RHS' has a least one element */
    struct utillib_symbol const *FIRST =
        utillib_vector_at(utillib_rule_rhs(rule), 0);
    struct utillib_ll1_set *LHS_FIRST = ll1_builder_FIRST_get(self, LHS);
    if (utillib_symbol_value(FIRST) == UT_SYM_EPS) {
      utillib_ll1_set_flag(LHS_FIRST)=true;
    } else if (utillib_symbol_kind(FIRST) == UT_SYMBOL_TERMINAL) {
      utillib_ll1_set_insert(LHS_FIRST, utillib_symbol_value(FIRST));
    }   
  }
}

/**
 * \ll1_builder_FIRST_increamental
 * Makes an incremental update to the intermedia first sets.
 * Visits the symbols on the right hand side sequentially while
 * merging their `FIRST's into that of the left hand side symbol and
 * stops at the first occurrence of a terminal symbol or a non-terminal
 * symbol whose `FIRST' does not contain `epsilon'.
 * If the whole sequence contains exclusively non-terminal symbols with
 * `epsilon' inside their `FIRST's, the `FIRST' of the LHS symbol will
 * also contains `epsilon'.
 * \return Whether any changes took place in this incremental evaluation.
 */
static bool ll1_builder_FIRST_increamental(struct utillib_ll1_builder *self) {
  bool changed = false;
  /* Will be `true' if all the symbols on the right hand side has `epsilon' in
   */
  /* their `FIRST's. */
  bool last_eps;
  struct utillib_rule_index const *rule_index = self->rule_index;
  struct utillib_vector const *rules_vector =
      utillib_rule_index_rules(rule_index);

  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, rule, rules_vector) {
    struct utillib_symbol const *LHS = utillib_rule_lhs(rule);
    struct utillib_vector const *RHS = utillib_rule_rhs(rule);
    size_t LHS_index = utillib_rule_index_non_terminal_index(
        rule_index, utillib_symbol_value(LHS));
    struct utillib_ll1_set *LHS_FIRST =
        utillib_vector_at(&self->FIRST, LHS_index);
    UTILLIB_VECTOR_FOREACH(struct utillib_symbol const *, symbol, RHS) {
      size_t RHS_value = utillib_symbol_value(symbol);
      if (utillib_symbol_kind(symbol) == UT_SYMBOL_TERMINAL) {
        if (!utillib_ll1_set_contains(LHS_FIRST, RHS_value)) {
          utillib_ll1_set_insert(LHS_FIRST, utillib_symbol_value(symbol));
          changed = true;
        }
        last_eps = false;
        /* Traversal ends with terminal symbol */
        break;
      }
      size_t RHS_index =
          utillib_rule_index_non_terminal_index(rule_index, RHS_value);
      struct utillib_ll1_set *RHS_FIRST =
          utillib_vector_at(&self->FIRST, RHS_index);
      changed = utillib_ll1_set_union(LHS_FIRST, RHS_FIRST);
      if (!(last_eps = utillib_ll1_set_flag(RHS_FIRST))) {
        /* Traversal ends with non-terminal symbol without `epsilon'. */
        break;
      }
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
  size_t rules_size = utillib_rule_index_rules_size(rule_index);
  bool last_eps;
  /* This loop needs to loop over all the rules and their `FIRST's so */
  /* indices are needed instead of elements */
  for (size_t rule_id = 0; rule_id < rules_size; ++rule_id) {
    struct utillib_rule const *rule =
        utillib_rule_index_rule_at(rule_index, rule_id);
    struct utillib_ll1_set *FIRST =
        utillib_vector_at(&self->FIRST_RULE, rule_id);
    struct utillib_vector const *RHS = utillib_rule_rhs(rule);
    UTILLIB_VECTOR_FOREACH(struct utillib_symbol const *, symbol, RHS) {
      size_t RHS_value = utillib_symbol_value(symbol);
      if (utillib_symbol_kind(symbol) == UT_SYMBOL_TERMINAL) {
        utillib_ll1_set_insert(FIRST, RHS_value);
        last_eps = false;
        break;
      }
      size_t RHS_index =
          utillib_rule_index_non_terminal_index(rule_index, RHS_value);
      struct utillib_ll1_set const *RHS_FIRST =
          utillib_vector_at(&self->FIRST, RHS_index);
      utillib_ll1_set_union(FIRST, RHS_FIRST);
      if (!(last_eps = RHS_FIRST->flag)) {
        break;
      }
    }
    FIRST->flag = last_eps;
  }
}

/*
 * FOLLOW construction
 */

/**
 * \function ll1_builder_FOLLOW_form_correct
 * Helper to judge whether the right hand side of the rule is in the form
 * required
 * by FOLLOW evaluation, i.e., `aBb', where `a' can be nothing.
 */
static bool ll1_builder_FOLLOW_form_correct(struct utillib_vector const *RHS) {
  size_t RHS_size;
  if ((RHS_size = utillib_vector_size(RHS)) < 2)
    return false;
  struct utillib_symbol const *PREV = utillib_vector_at(RHS, RHS_size - 2);
  if (utillib_symbol_kind(PREV) == UT_SYMBOL_TERMINAL)
    return false;
  return true;
}

/**
 * \function ll1_builder_FOLLOW_get
 * Helper to get the `FOLLOW' for a particular non-terminal
 * symbol.
 */
static struct utillib_ll1_set *
ll1_builder_FOLLOW_get(struct utillib_ll1_builder *self,
                       struct utillib_symbol const *symbol) {
  return utillib_vector_at(&self->FOLLOW,
                           utillib_rule_index_non_terminal_index(
                               self->rule_index, utillib_symbol_value(symbol)));
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

static void ll1_builder_FOLLOW_partial_eval(struct utillib_ll1_builder *self) {
  struct utillib_rule_index const *rule_index = self->rule_index;
  struct utillib_vector const *rules_vector =
      utillib_rule_index_rules(rule_index);
  struct utillib_symbol const *TOP_symbol =
      utillib_rule_index_top_symbol(rule_index);
  struct utillib_ll1_set *TOP_FOLLOW = ll1_builder_FOLLOW_get(self, TOP_symbol);
  utillib_ll1_set_insert(TOP_FOLLOW, UT_SYM_EOF);

  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, rule, rules_vector) {
    struct utillib_vector const *RHS = utillib_rule_rhs(rule);
    /* The form of rule does not match */
    if (!ll1_builder_FOLLOW_form_correct(RHS))
      continue;
    size_t RHS_size = utillib_vector_size(RHS);
    struct utillib_symbol const *PREV = utillib_vector_at(RHS, RHS_size - 2);
    struct utillib_symbol const *LAST = utillib_vector_back(RHS);
    size_t LAST_value = utillib_symbol_value(LAST);
    /* The `b' symbol is `epsilon' */
    if (LAST_value == UT_SYM_EPS)
      continue;
    struct utillib_ll1_set *PREV_FOLLOW = ll1_builder_FOLLOW_get(self, PREV);
    if (utillib_symbol_kind(LAST) == UT_SYMBOL_TERMINAL) {
      utillib_ll1_set_insert(PREV_FOLLOW, LAST_value);
      continue;
    }
    struct utillib_ll1_set const *LAST_FIRST =
        ll1_builder_FIRST_get(self, LAST);
    utillib_ll1_set_union(PREV_FOLLOW, LAST_FIRST);
  }
  ll1_builder_FOLLOW_print(self);
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
  if (utillib_ll1_set_flag(LAST_FIRST))
    return true;
  return false;
}

/**
 * \function ll1_builder_FOLLOW_incremental
 * Based on the following algorithm:
 * If the rule has form `A := aBb' and `b' satisfies
 * `ll1_builder_FOLLOW_last_epsilon' implying `epsilon'
 * can be derived from `b', then what follows `A' can
 * also follows `B'.
 * Thus, we merge `FOLLOW' for `A' into that of `B'.
 */

static bool ll1_builder_FOLLOW_incremental(struct utillib_ll1_builder *self) {
  struct utillib_rule_index const *rule_index = self->rule_index;
  struct utillib_vector const *rules_vector =
      utillib_rule_index_rules(rule_index);
  struct utillib_ll1_set *FIRST_SET;
  size_t size;
  bool changed = false;

  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, rule, rules_vector) {
    struct utillib_vector const *RHS = utillib_rule_rhs(rule);
    struct utillib_symbol const *LHS = utillib_rule_lhs(rule);
    /* The form of rule does not match */
    if (!ll1_builder_FOLLOW_form_correct(RHS))
      continue;
    struct utillib_symbol const *LAST = utillib_vector_back(RHS);
    if (!ll1_builder_FOLLOW_last_epsilon(self, LAST))
      continue;
    size_t RHS_size = utillib_vector_size(RHS);
    struct utillib_ll1_set const *LHS_FOLLOW =
        ll1_builder_FOLLOW_get(self, LHS);
    struct utillib_symbol const *PREV = utillib_vector_at(RHS, RHS_size - 2);
    struct utillib_ll1_set *PREV_FOLLOW = ll1_builder_FOLLOW_get(self, PREV);
    changed = utillib_ll1_set_union(PREV_FOLLOW, LHS_FOLLOW);
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
 */
void utillib_ll1_builder_init(struct utillib_ll1_builder *self,
                              struct utillib_rule_index const *rule_index) {
  utillib_vector_init(&self->FIRST);
  utillib_vector_init(&self->FOLLOW);
  utillib_vector_init(&self->errors);
  utillib_vector_init(&self->FIRST_RULE);

  self->rule_index = rule_index;
  size_t non_terminals_size = utillib_rule_index_non_terminals_size(rule_index);
  size_t terminals_size = utillib_rule_index_terminals_size(rule_index);
  /* Since our symbol starts from one. */
  size_t symbols_size = non_terminals_size + terminals_size + 1;
  size_t rules_size = utillib_rule_index_rules_size(rule_index);

  utillib_vector_reserve(&self->FIRST, non_terminals_size);
  utillib_vector_reserve(&self->FOLLOW, non_terminals_size);
  utillib_vector_reserve(&self->FIRST_RULE, rules_size);

  for (size_t i = 0; i < non_terminals_size; ++i) {
    utillib_vector_push_back(&self->FIRST, ll1_set_create(symbols_size));
    utillib_vector_push_back(&self->FOLLOW, ll1_set_create(symbols_size));
  }
  for (size_t i = 0; i < rules_size; ++i) {
    utillib_vector_push_back(&self->FIRST_RULE, ll1_set_create(symbols_size));
  }
  ll1_builder_build_FIRST_FOLLOW(self);
}

/**
 * \function utillib_ll1_builder_destroy
 */
void utillib_ll1_builder_destroy(struct utillib_ll1_builder *self) {
  utillib_vector_destroy_owning(&self->FIRST,
                                (utillib_destroy_func_t *)ll1_set_destroy);
  utillib_vector_destroy_owning(&self->FIRST_RULE,
                                (utillib_destroy_func_t *)ll1_set_destroy);
  utillib_vector_destroy_owning(&self->FOLLOW,
                                (utillib_destroy_func_t *)ll1_set_destroy);
  utillib_vector_destroy(&self->errors);
}

/**
 * \function utillib_ll1_builder_build_table
 * Builds the LL(1) table according to FIRST and FOLLOW sets.
 * For all the rules, if the FIRST of it contains terminal symbol
 * `a', then there will be an entry `M[a][LHS]=rule' in the table.
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
  size_t non_terminals_size = utillib_rule_index_non_terminals_size(rule_index);
  size_t terminals_size = utillib_rule_index_terminals_size(rule_index);
  size_t symbols_size = non_terminals_size + terminals_size;
  utillib_vector2_init(table, non_terminals_size + 1, terminals_size + 1);

  for (size_t rule_id = 0, size = utillib_rule_index_rules_size(rule_index);
       rule_id < size; ++rule_id) {
    struct utillib_ll1_set const *FIRST =
        utillib_vector_at(&self->FIRST_RULE, rule_id);
    struct utillib_rule const *rule = utillib_vector_at(rules_vector, rule_id);
    struct utillib_symbol const *LHS = utillib_rule_lhs(rule);
    size_t LHS_index = utillib_rule_index_non_terminal_index(
        rule_index, utillib_symbol_value(LHS));
    for (size_t symbol_id = 0; symbol_id < symbols_size; ++symbol_id) {
      if (utillib_ll1_set_contains(FIRST, symbol_id)) {
        size_t symbol_index =
            utillib_rule_index_terminal_index(rule_index, symbol_id);
        utillib_vector2_set(table, LHS_index, symbol_index,
                            (utillib_element_t)rule);
      }
    }
    if (utillib_ll1_set_flag(FIRST)) {
      struct utillib_ll1_set const *FOLLOW =
          utillib_vector_at(&self->FOLLOW, LHS_index);
      for (size_t symbol_id = 0; symbol_id < symbols_size; ++symbol_id) {
        if (utillib_ll1_set_contains(FOLLOW, symbol_id)) {
          size_t symbol_index =
              utillib_rule_index_terminal_index(rule_index, symbol_id);
          utillib_vector2_set(table, LHS_index, symbol_index,
                              (utillib_element_t)UTILLIB_RULE_NULL);
        }
      }
    }
  }
}

/**
 * \function utillib_ll1_builder_check
 * Checks for LL(1) confirmness of the input based on the sets
 * built.
 */
int utillib_ll1_builder_check(struct utillib_ll1_builder *self) {}

