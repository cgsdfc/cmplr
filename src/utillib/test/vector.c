#include <utillib/vector.h>
#include <utillib/test.h>

UTILLIB_TEST(vector_init) {
  utillib_vector vector;
  utillib_vector_init(&vector);
}

UTILLIB_TEST_BEGIN(vector)
UTILLIB_TEST_ELEM(vector_init)
UTILLIB_TEST_END(vector)

