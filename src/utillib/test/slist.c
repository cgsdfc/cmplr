#include <utillib/slist.h>
#include <utillib/test.h>

UTILLIB_TEST(iterator_init_null) {
  utillib_slist_iterator iter;
  utillib_slist_iterator_init_null(&iter);
  UTILLIB_TEST_EXPECT(utillib_slist_iterator_has_next(&iter) == false);
}

UTILLIB_TEST_BEGIN(Utillib_Slist)
UTILLIB_TEST_RUN(iterator_init_null)
UTILLIB_TEST_END(Utillib_Slist)
