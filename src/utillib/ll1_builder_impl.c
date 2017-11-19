#include "ll1_builder_impl.h"
#include "print.h"
#include <assert.h>
#include <stdlib.h>

/**
 * \file utillib/ll1_builder_impl.h
 */

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
struct utillib_ll1_set *
ll1_builder_FIRST_get(struct utillib_ll1_builder const *self,
                      struct utillib_symbol const *symbol) {
  assert(symbol->kind == UT_SYMBOL_NON_TERMINAL &&
         "Only non terminal symbols can have FIRST set");
  return &self->FIRST[utillib_rule_index_symbol_index(self->rule_index,
                                                      symbol)];
}

/**
 * \function ll1_builder_FOLLOW_get
 * Helper to get the `FOLLOW' for a particular non-terminal
 * symbol.
 */
struct utillib_ll1_set *
ll1_builder_FOLLOW_get(struct utillib_ll1_builder const *self,
                       struct utillib_symbol const *symbol) {
  assert(symbol->kind == UT_SYMBOL_NON_TERMINAL &&
         "Only non terminal symbols can have FOLLOW set");
  return &self->FOLLOW[utillib_rule_index_symbol_index(self->rule_index,
                                                       symbol)];
}

/**
 * \function ll1_builder_FIRST_RULE_get
 * Helper to get the `FIRST' for a particular rule.
 */
struct utillib_ll1_set *
ll1_builder_FIRST_RULE_get(struct utillib_ll1_builder const *self,
                           struct utillib_rule const *rule) {
  return &self->FIRST_RULE[rule->id];
}
/*
 * FIRST/FOLLOW SET interface
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

void utillib_ll1_set_destroy(struct utillib_ll1_set *self) {
  utillib_bitset_destroy(&self->bitset);
}

bool utillib_ll1_set_union_updated(struct utillib_ll1_set *self,
                                   struct utillib_ll1_set const *other) {
  return utillib_bitset_union_updated(&self->bitset, &other->bitset);
}

/**
 * \function utillib_ll1_set_insert_updated
 * Inserts the value of a terminal symbol into `self'.
 * \param symbol should be a termianl symbol.
 * \return Whether `self' was updated.
 */

bool utillib_ll1_set_insert_updated(struct utillib_ll1_set *self,
                                    struct utillib_symbol const *symbol) {
  assert(symbol->kind == UT_SYMBOL_TERMINAL &&
         "Only terminal symbol can be inserted");

  if (symbol == UTILLIB_SYMBOL_EOF || symbol == UTILLIB_SYMBOL_EPS) {
    if (self->flag)
      return false;
    self->flag = true;
    return true;
  }
  return utillib_bitset_insert_updated(&self->bitset, symbol->value);
}

bool utillib_ll1_set_contains(struct utillib_ll1_set const *self,
                              size_t value) {
  return utillib_bitset_contains(&self->bitset, value);
}

bool utillib_ll1_set_equal(struct utillib_ll1_set const *self,
                           struct utillib_ll1_set const *other) {
  return self->flag == other->flag &&
         utillib_bitset_equal(&self->bitset, &other->bitset);
}

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
 * JSON interfaces
 */

struct utillib_json_value_t *
ll1_builder_set_json_array_create(struct utillib_ll1_set const *self,
                                  struct utillib_rule_index const *rule_index,
                                  int kind) {
  struct utillib_vector const *terminals = &rule_index->terminals;
  struct utillib_json_value_t *array = utillib_json_array_create_empty();
  UTILLIB_VECTOR_FOREACH(struct utillib_symbol const *, symbol, terminals) {
    if (utillib_ll1_set_contains(self, symbol->value)) {
      utillib_json_array_push_back(array, utillib_symbol_json_string_create(symbol));
    }
  }
  if (self->flag) {
    char const *special = NULL;
    switch (kind) {
    case UT_LL1_FIRST:
      special = UTILLIB_SYMBOL_EPS->name;
      break;
    case UT_LL1_FOLLOW:
      special = UTILLIB_SYMBOL_EOF->name;
      break;
    }
    utillib_json_array_push_back(array, utillib_json_string_create(special));
  }
  return array;
}

struct utillib_json_value_t *
ll1_builder_set_json_object_create(struct utillib_ll1_builder const *self,
                                   int kind, struct utillib_symbol const *LHS) {
  struct utillib_json_value_t *object = utillib_json_object_create_empty();
  struct utillib_ll1_set const *set = NULL;

  switch (kind) {
  case UT_LL1_FIRST:
    utillib_json_object_push_back(object, "FIRST",
                                  utillib_symbol_json_string_create(LHS));
    set = ll1_builder_FIRST_get(self, LHS);
    break;
  case UT_LL1_FOLLOW:
    utillib_json_object_push_back(object, "FOLLOW",
                                  utillib_json_string_create(&LHS->name));
    set = ll1_builder_FOLLOW_get(self, LHS);
    break;
  }
  utillib_json_object_push_back(
      object, "elements",
      ll1_builder_set_json_array_create(set, self->rule_index, kind));
  return object;
}

struct utillib_json_value_t *
utillib_ll1_builder_json_object_create(struct utillib_ll1_builder const *self) {
  struct utillib_rule_index const *rule_index = self->rule_index;
  struct utillib_vector const *symbol_vector = &rule_index->non_terminals;
  struct utillib_json_value_t *array = utillib_json_array_create_empty();

  UTILLIB_VECTOR_FOREACH(struct utillib_symbol const *, symbol, symbol_vector) {
    utillib_json_array_push_back(
        array, ll1_builder_set_json_object_create(self, UT_LL1_FIRST, symbol));
    utillib_json_array_push_back(
        array, ll1_builder_set_json_object_create(self, UT_LL1_FOLLOW, symbol));
  }
  return array;
}

/*
 * Error handling.
 */

UTILLIB_ETAB_BEGIN(utillib_ll1_error_kind)
UTILLIB_ETAB_ELEM_INIT(UT_LL1_EFIRST, "FIRST/FIRST conflict")
UTILLIB_ETAB_ELEM_INIT(UT_LL1_EFOLLOW, "FIRST/FOLLOW conflict")
UTILLIB_ETAB_END(utillib_ll1_error_kind);

/**
 * \function ll1_builder_error_create_as_EFIRST
 * Creates an error telling the offending (overlapping)
 * FIRST and its corresponding rules.
 */
struct utillib_ll1_builder_error *
ll1_builder_error_create_as_EFIRST(struct utillib_rule_index const *rule_index,
                                   struct utillib_ll1_set const *lhs_FIRST,
                                   struct utillib_ll1_set const *rhs_FIRST,
                                   struct utillib_rule const *lhs_rule,
                                   struct utillib_rule const *rhs_rule) {
  struct utillib_ll1_builder_error *self = malloc(sizeof *self);
  self->values[0] = utillib_rule_json_object_create(lhs_rule);
  self->values[1] =
      ll1_builder_set_json_array_create(lhs_FIRST, rule_index, UT_LL1_FIRST);
  self->values[2] = utillib_rule_json_object_create(rhs_rule);
  self->values[3] =
      ll1_builder_set_json_array_create(rhs_FIRST, rule_index, UT_LL1_FIRST);
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
struct utillib_ll1_builder_error *
ll1_builder_error_create_as_EFOLLOW(struct utillib_rule_index const *rule_index,
                                    struct utillib_rule const *lhs_rule,
                                    struct utillib_ll1_set const *lhs_FIRST,
                                    struct utillib_symbol const *rhs_symbol,
                                    struct utillib_ll1_set const *rhs_FOLLOW) {
  struct utillib_ll1_builder_error *self = malloc(sizeof *self);
  self->kind = UT_LL1_EFOLLOW;
  self->values[0] = utillib_rule_json_object_create(lhs_rule);
  self->values[1] =
      ll1_builder_set_json_array_create(lhs_FIRST, rule_index, UT_LL1_FIRST);
  self->values[2] = utillib_symbol_json_string_create(rhs_symbol);
  self->values[3] =
      ll1_builder_set_json_array_create(rhs_FOLLOW, rule_index, UT_LL1_FOLLOW);
  return self;
}

void utillib_ll1_builder_error_destroy(struct utillib_ll1_builder_error *self) {
  for (int i = 0; i < UT_LL1_ERR_VAL_MAX; ++i)
    if (self->values[i])
      utillib_json_value_destroy(self->values[i]);
  free(self);
}

/**
 * \function ll1_builder_print_EFOLLOW
 * Prints formated error information about the `FIRST/FOLLOW' conflicts
 * of the grammar.
 * The layout should be:
 * Rule A, FIRST A, Symbol B, FOLLOW B
 */
static void
ll1_builder_print_EFOLLOW(struct utillib_json_value_t * const * errs) {
  struct utillib_string str;
  for (int i = 0; i < UT_LL1_ERR_VAL_MAX; ++i) {
    utillib_json_tostring(errs[i], &str);
    char const *msg = utillib_string_c_str(&str);
    switch (i) {
    case 0:
      utillib_error_printf("Rule: %s\n", msg);
      break;
    case 1:
      utillib_error_printf("The FIRST set of it is %s\n", msg);
      break;
    case 2:
      utillib_error_printf("The Symbol: %s\n", msg);
      break;
    case 3:
      utillib_error_printf("The FOLLOW set of it is %s\n", msg);
      break;
    }
  }
}

/**
 * \function ll1_builder_print_EFIRST
 * Prints formated error information about the `FIRST/FIRST' conflicts
 * of the grammar.
 * The layout of the fixed-size array `errs' should be:
 * Rule A, FIRST A, Rule B, FIRST B.
 */

static void ll1_builder_print_EFIRST(struct utillib_json_value_t *const *errs) {
  struct utillib_string str;
  for (int i = 0; i < UT_LL1_ERR_VAL_MAX; ++i) {
    utillib_json_tostring(errs[i], &str);
    char const *msg = utillib_string_c_str(&str);
    switch (i) {
    case 0:
    case 2:
      utillib_error_printf("Production in conflict is %s\n", msg);
      break;
    case 3:
    case 1:
      utillib_error_printf("FIRST set of which is %s\n", msg);
      break;
    }
    utillib_string_destroy(&str);
  }
}

void ll1_builder_print_error(struct utillib_ll1_builder_error const *error) {
  utillib_error_printf("ERROR: %s\n",
                       utillib_ll1_error_kind_tostring(error->kind));
  switch (error->kind) {
  case UT_LL1_EFIRST:
    ll1_builder_print_EFIRST(error->values);
    break;
  case UT_LL1_EFOLLOW:
    ll1_builder_print_EFOLLOW(error->values);
    break;
  }
  utillib_error_printf("\n");
}
