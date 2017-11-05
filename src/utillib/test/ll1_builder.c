#include "symbol_rule.h"
#include <utillib/ll1_builder.h>
#include <utillib/test.h>

static struct utillib_rule_index rule_index;
static const size_t test_rules_FIRST[][10] = {
        [SYM_E] = {SYM_LP, SYM_I, UT_SYM_NULL},
        [SYM_EP] = {SYM_PLUS, UT_SYM_EPS, UT_SYM_NULL},
        [SYM_T] = {SYM_LP, SYM_I, UT_SYM_NULL},
        [SYM_TP] = {SYM_MUL, UT_SYM_EPS, UT_SYM_NULL},
        [SYM_F] = {SYM_LP, SYM_I, UT_SYM_NULL},
};

static const size_t test_rules_FOLLOW[][10] = {
        [SYM_E] = {SYM_RP, UT_SYM_EOF, UT_SYM_NULL},
        [SYM_EP] = {SYM_RP, UT_SYM_EOF, UT_SYM_NULL},
        [SYM_T] = {SYM_PLUS, UT_SYM_EOF, UT_SYM_NULL},
        [SYM_TP] = {SYM_PLUS, SYM_RP, UT_SYM_EOF, UT_SYM_NULL},
        [SYM_F] = {SYM_PLUS, SYM_MUL, SYM_RP, UT_SYM_EOF, UT_SYM_NULL},
};

UTILLIB_TEST_CONST(test_rules_set_LEN, 5);

UTILLIB_TEST_SET_UP() {
  utillib_rule_index_init(&rule_index, test_symbols, test_rules);
  utillib_ll1_builder_init(UT_FIXTURE, &rule_index);
}

UTILLIB_TEST_TEAR_DOWN() { utillib_rule_index_destroy(&rule_index); }

UTILLIB_TEST_AUX(ll1_builder_set_correct,
                 struct utillib_vector const *actual_sets,
                 size_t const expected_sets[][10]) {
  struct utillib_ll1_builder const * self=UT_FIXTURE;
  struct utillib_rule_index const * rule_index=self->rule_index;
  UTILLIB_VECTOR_FOREACH(struct utillib_symbol const*, symbol,
      utillib_rule_index_non_terminals(rule_index)) {
    size_t value=utillib_symbol_value(symbol);
    size_t index=utillib_rule_index_non_terminal_index(rule_index, value);
    struct utillib_ll1_set const *set = utillib_vector_at(actual_sets, index);
    for (size_t j = 0; expected_sets[value][j] != UT_SYM_NULL; ++j) {
      size_t sym = expected_sets[value][j];
      if (sym == UT_SYM_EPS)
        UTILLIB_TEST_ASSERT(utillib_ll1_set_flag(set));
      else
        UTILLIB_TEST_ASSERT(utillib_ll1_set_contains(set, sym));
    }
  }
}

UTILLIB_TEST(ll1_builder_FIRST_correct) {
  struct utillib_ll1_builder const *self = UT_FIXTURE;
  UTILLIB_TEST_AUX_INVOKE(ll1_builder_set_correct,&self->FIRST, test_rules_FIRST);
}

UTILLIB_TEST(ll1_builder_FOLLOW_correct) {
  struct utillib_ll1_builder const *self = UT_FIXTURE;
  UTILLIB_TEST_AUX_INVOKE(ll1_builder_set_correct, &self->FOLLOW, test_rules_FOLLOW);
}

UTILLIB_TEST(ll1_builder_build_table) {
  utillib_ll1_builder_init(UT_FIXTURE, &rule_index);
  struct utillib_vector2 table;
  utillib_ll1_builder_build_table(UT_FIXTURE, &table);
}

UTILLIB_TEST(ll1_builder_json) {
  utillib_json_value_t * val=utillib_ll1_builder_json_object_create(UT_FIXTURE, sizeof (struct utillib_ll1_builder));
  utillib_json_pretty_print(val, stderr);
  utillib_json_value_destroy(val);
}

UTILLIB_TEST_DEFINE(Utillib_LL1Builder) {
  UTILLIB_TEST_BEGIN(Utillib_LL1Builder)
  UTILLIB_TEST_SKIP(ll1_builder_json)
  UTILLIB_TEST_RUN(ll1_builder_FIRST_correct)
  UTILLIB_TEST_SKIP(ll1_builder_FOLLOW_correct)
  UTILLIB_TEST_SKIP(ll1_builder_build_table)
  UTILLIB_TEST_END(Utillib_LL1Builder)
  UTILLIB_TEST_FIXTURE(struct utillib_ll1_builder);
  UTILLIB_TEST_RETURN(Utillib_LL1Builder)
}
