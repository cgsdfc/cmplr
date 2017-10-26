#include <utillib/vector.h>
#include <utillib/test.h>

static void setup(utillib_vector *self) {
  utillib_vector_init(self);
}
static void teardown(utillib_vector *self) {
  utillib_vector_destroy(self);
}

UTILLIB_TEST(vector_init) {
  UTILLIB_TEST_ASSERT(utillib_vector_empty(FIXTURE));
}

UTILLIB_TEST(push_back) {
  int N=10;
  for (int i=0;i<N;++i) {
    utillib_vector_push_back(FIXTURE, 
        ( utillib_element_t ) 10);
  }
  UTILLIB_TEST_ASSERT_EQ(N, utillib_vector_size(FIXTURE));
}

UTILLIB_TEST_DEFINE(Utillib_Vector) {
  static utillib_vector static_vector;
  UTILLIB_TEST_BEGIN(Utillib_Vector)
  UTILLIB_TEST_SKIP(vector_init)
  UTILLIB_TEST_RUN(push_back)
  UTILLIB_TEST_END(Utillib_Vector)

  UTILLIB_TEST_PROPERTIES(
      UT_FIXTURE, &static_vector,
      UT_SETUP, setup,
      UT_TEARDOWN, teardown)

  UTILLIB_TEST_RETURN()
}
