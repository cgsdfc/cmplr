#include <utillib/test.h>
#include <utillib/symbol.h>

UTILLIB_ENUM_BEGIN(test_symbol_kind)
  UTILLIB_ENUM_ELEM_INIT(SYM_E, 1)
  UTILLIB_ENUM_ELEM(SYM_EP)
  UTILLIB_ENUM_ELEM(SYM_T)
  UTILLIB_ENUM_ELEM(SYM_TP)
  UTILLIB_ENUM_ELEM(SYM_F)
  UTILLIB_ENUM_ELEM(SYM_I)
  UTILLIB_ENUM_ELEM(SYM_LP)
  UTILLIB_ENUM_ELEM(SYM_RP)
  UTILLIB_ENUM_ELEM(SYM_PLUS)
  UTILLIB_ENUM_ELEM(SYM_MUL)
UTILLIB_ENUM_END(test_symbol_kind)

UTILLIB_SYMBOL_BEGIN(test_symbols)
  UTILLIB_SYMBOL_NON_TERMINAL(SYM_E)
  UTILLIB_SYMBOL_NON_TERMINAL(SYM_EP)
  UTILLIB_SYMBOL_NON_TERMINAL(SYM_T)
  UTILLIB_SYMBOL_NON_TERMINAL(SYM_TP)
  UTILLIB_SYMBOL_NON_TERMINAL(SYM_F)
  UTILLIB_SYMBOL_TERMINAL(SYM_I)
  UTILLIB_SYMBOL_TERMINAL(SYM_LP)
  UTILLIB_SYMBOL_TERMINAL(SYM_RP)
  UTILLIB_SYMBOL_TERMINAL(SYM_PLUS)
  UTILLIB_SYMBOL_TERMINAL(SYM_MUL)
UTILLIB_SYMBOL_END(test_symbols)

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


