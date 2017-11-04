#include <utillib/test.h>
#include "symbol_rule.h"


UTILLIB_TEST_SET_UP() {
  utillib_rule_index_init(UT_FIXTURE, test_symbols, test_rules);
}
UTILLIB_TEST_TEAR_DOWN() {
  utillib_rule_index_destroy(UT_FIXTURE);
}

UTILLIB_TEST(rule_index_init) {
  struct utillib_rule_index * index=UT_FIXTURE;
  UTILLIB_TEST_ASSERT_EQ(utillib_rule_index_terminals_size(index), terminals_size);
  UTILLIB_TEST_ASSERT_EQ(utillib_rule_index_non_terminals_size(index), non_terminals_size);
  UTILLIB_TEST_ASSERT_EQ(utillib_rule_index_rules_size(index), rules_size);
}

UTILLIB_TEST(rule_index_json) {
  utillib_json_value_t *val=utillib_rule_index_json_object_create(UT_FIXTURE, 0);
  utillib_json_pretty_print(val, stderr);
  utillib_json_value_destroy(val);
}

UTILLIB_TEST_DEFINE(Utillib_Rule) {
  UTILLIB_TEST_BEGIN(Utillib_Rule)
    UTILLIB_TEST_RUN(rule_index_init)
    UTILLIB_TEST_RUN( rule_index_json )
  UTILLIB_TEST_END(Utillib_Rule)
  UTILLIB_TEST_FIXTURE(struct utillib_rule_index)
  UTILLIB_TEST_RETURN(Utillib_Rule)
}


