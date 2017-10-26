#include <utillib/vector.h>
#include <utillib/test.h>

static void setup(utillib_vector *self) { }
static void teardown(utillib_vector *self) {}

UTILLIB_TEST(vector_init) {
  UTILLIB_TEST_ASSERT(utillib_vector_empty(FIXTURE));
}

UTILLIB_TEST(push_back) {
  

}

UTILLIB_TEST_DEFINE(Utillib_Vector) {
  static utillib_vector static_vector;
  UTILLIB_TEST_BEGIN(Utillib_Vector)
  UTILLIB_TEST_RUN(vector_init)
  UTILLIB_TEST_SKIP(push_back)
  UTILLIB_TEST_END(Utillib_Vector)

  UTILLIB_TEST_PROPERTIES(
      UT_FIXTURE, &static_vector,
      UT_SETUP, setup,
      UT_TEARDOWN, teardown)
  UTILLIB_TEST_RETURN()
}
