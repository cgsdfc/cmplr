#include <utillib/test.h>
#include <utillib/symbol.h>

/**
 * \test specaial_symbol
 * Tests the value of specail symbols
 * were set correctly.
 */
UTILLIB_TEST(specaial_symbol_value) {
  UTILLIB_TEST_ASSERT_EQ(UTILLIB_SYMBOL_EOF->value, UT_SYM_EOF);
  UTILLIB_TEST_ASSERT_EQ(UTILLIB_SYMBOL_ERR->value, UT_SYM_ERR);
  UTILLIB_TEST_ASSERT_EQ(UTILLIB_SYMBOL_EPS->value, UT_SYM_EPS);
  utillib_json_value_t *val=utillib_symbol_json_object_create(UTILLIB_SYMBOL_EOF, 0);
  utillib_json_pretty_print(val, stderr);
  utillib_json_value_destroy(val);
  struct utillib_symbol * symbol=UTILLIB_SYMBOL_ERR;
  val=utillib_symbol_json_object_pointer_create(&symbol, 0);
  utillib_json_pretty_print(val, stderr);
  utillib_json_value_destroy(val);
}

UTILLIB_TEST_DEFINE(Utillib_Symbol) {
  UTILLIB_TEST_BEGIN(Utillib_Symbol)
    UTILLIB_TEST_RUN(specaial_symbol_value)
  UTILLIB_TEST_END(Utillib_Symbol)
  UTILLIB_TEST_RETURN(Utillib_Symbol)
}

