#include <utillib/test.h>

UTILLIB_TEST(helloworld) {
  puts("before UTILLIB_TEST_ASSERT");
  UTILLIB_TEST_ABORT("Abort");
  puts("after UTILLIB_TEST_ASSERT");
}

UTILLIB_TEST_DEFINE(Utillib_Test) {
  UTILLIB_TEST_BEGIN(Utillib_Test)
  UTILLIB_TEST_RUN(helloworld)
  UTILLIB_TEST_END(Utillib_Test)
  UTILLIB_TEST_RETURN(Utillib_Test)
}

