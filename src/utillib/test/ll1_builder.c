#include <utillib/test.h>
#include <utillib/ll1_builder.h>
#include "symbol_rule.h"
static struct utillib_rule_index rule_index;

UTILLIB_TEST_SET_UP() {
  utillib_rule_index_init(&rule_index, test_symbols, test_rules);
}

UTILLIB_TEST_TEAR_DOWN() {
  utillib_rule_index_destroy(&rule_index);
}

UTILLIB_TEST(ll1_builder_init) {
 utillib_ll1_builder_init(UT_FIXTURE, &rule_index);

}
UTILLIB_TEST(ll1_builder_build_table) {}

UTILLIB_TEST_DEFINE(Utillib_LL1Builder) {
  UTILLIB_TEST_BEGIN(Utillib_LL1Builder)
  UTILLIB_TEST_RUN(ll1_builder_init)
  UTILLIB_TEST_RUN(ll1_builder_build_table)
  UTILLIB_TEST_END(Utillib_LL1Builder)
  UTILLIB_TEST_FIXTURE(struct utillib_ll1_builder);
  UTILLIB_TEST_RETURN(Utillib_LL1Builder)
}


