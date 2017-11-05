#include "symbol_rule.h"
#include <utillib/ll1_builder.h>
#include <utillib/test.h>

static struct utillib_rule_index rule_index;



UTILLIB_TEST_SET_UP() {
  utillib_rule_index_init(&rule_index, test_symbols, test_rules);
  utillib_ll1_builder_init(UT_FIXTURE, &rule_index);
}

UTILLIB_TEST_TEAR_DOWN() { 
  utillib_ll1_builder_destroy(UT_FIXTURE); 
  utillib_rule_index_destroy(&rule_index);
}

UTILLIB_TEST_AUX(build_expected_set, struct utillib_ll1_set * set, 
    size_t symbols_size,
    size_t const expected_sets[])
{
  utillib_ll1_set_init(set, symbols_size);
  for (size_t j = 0; expected_sets[j] != UT_SYM_NULL; ++j) {
    size_t sym = expected_sets[j];
      if (sym == UT_SYM_EPS)
        utillib_ll1_set_flag(set)=true;
      else
        utillib_ll1_set_insert(set, sym);
  }
}


/**
 * \test ll1_builder_set_correct
 * Tests that actual_sets have at many elements
 * as expected_sets.
 * \param actual_sets The set from `utillib_ll1_builder'.
 * \param expected_sets The correct data.
 */

UTILLIB_TEST_AUX(ll1_builder_set_correct,
                 struct utillib_vector const *actual_sets,
                 size_t const expected_sets[][10]) 
{
  struct utillib_ll1_builder const * self=UT_FIXTURE;
  struct utillib_rule_index const * rule_index=self->rule_index;
  size_t symbols_size=utillib_rule_index_symbols_size(rule_index);
  UTILLIB_VECTOR_FOREACH(struct utillib_symbol const*, symbol,
      utillib_rule_index_non_terminals(rule_index)) {
    size_t value=utillib_symbol_value(symbol);
    size_t index=utillib_rule_index_non_terminal_index(rule_index, value);
    struct utillib_ll1_set const *actual_set = utillib_vector_at(actual_sets, index);
    struct utillib_ll1_set expected_set;
    UTILLIB_TEST_AUX_INVOKE(build_expected_set, &expected_set, symbols_size, expected_sets[value]);
    UTILLIB_TEST_EXPECT(utillib_ll1_set_equal(actual_set, &expected_set));
    utillib_ll1_set_destroy(&expected_set);
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
  struct utillib_vector2 table;
  utillib_ll1_builder_build_table(UT_FIXTURE, &table);
  utillib_vector2_destroy(&table);
}

UTILLIB_TEST(ll1_builder_json) {
  utillib_json_value_t * val=utillib_ll1_builder_json_object_create(UT_FIXTURE, sizeof (struct utillib_ll1_builder));
  utillib_json_pretty_print(val, stderr);
  utillib_json_value_destroy(val);
}

UTILLIB_TEST_DEFINE(Utillib_LL1Builder) {
  UTILLIB_TEST_BEGIN(Utillib_LL1Builder)
  UTILLIB_TEST_RUN(ll1_builder_json)
  UTILLIB_TEST_RUN(ll1_builder_FIRST_correct)
  UTILLIB_TEST_RUN(ll1_builder_FOLLOW_correct)
  UTILLIB_TEST_RUN(ll1_builder_build_table)
  UTILLIB_TEST_END(Utillib_LL1Builder)
  UTILLIB_TEST_FIXTURE(struct utillib_ll1_builder);
  UTILLIB_TEST_RETURN(Utillib_LL1Builder)
}
