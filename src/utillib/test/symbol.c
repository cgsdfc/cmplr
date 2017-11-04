#include <utillib/test.h>
#include "symbol_rule.h"

/**
 * \test specaial_symbol
 * Tests the value of specail symbols
 * were set correctly.
 */
UTILLIB_TEST(specaial_symbol_value) {
  UTILLIB_TEST_ASSERT_EQ(UTILLIB_SYMBOL_EOF->value, UT_SYM_EOF);
  UTILLIB_TEST_ASSERT_EQ(UTILLIB_SYMBOL_ERR->value, UT_SYM_ERR);
  UTILLIB_TEST_ASSERT_EQ(UTILLIB_SYMBOL_EPS->value, UT_SYM_EPS);
}

UTILLIB_TEST(json_object_create) {
 struct utillib_symbol symbol={ .kind=UT_SYMBOL_TERMINAL, .value=32, .name="an-arbitrary-symbol"};
 utillib_json_value_t *val=utillib_symbol_json_object_create(&symbol, 0);
 utillib_json_pretty_print(val, stderr);
 utillib_json_value_destroy(val);
}

UTILLIB_TEST(json_array_create) {
  utillib_json_value_t *val=utillib_symbol_json_array_create(
      (void*) test_symbols,
      sizeof test_symbols);
  utillib_json_pretty_print(val, stderr);
  utillib_json_value_destroy(val);
}

UTILLIB_TEST_DEFINE(Utillib_Symbol) {
  UTILLIB_TEST_BEGIN(Utillib_Symbol)
    UTILLIB_TEST_RUN(specaial_symbol_value)
    UTILLIB_TEST_RUN(json_object_create)
    UTILLIB_TEST_RUN(json_array_create)
  UTILLIB_TEST_END(Utillib_Symbol)
  UTILLIB_TEST_RETURN(Utillib_Symbol)
}


