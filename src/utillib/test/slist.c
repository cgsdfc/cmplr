#include <utillib/slist.h>
#include <utillib/test.h>

UTILLIB_TEST(iterator_init_null) {
  utillib_slist_iterator iter;
  utillib_slist_iterator_init_null(&iter);
  UTILLIB_EXPECT(utillib_slist_iterator_has_next(&iter)==false);
}

UTILLIB_TEST_BEGIN(utillib_slist)
UTILLIB_TEST_ELEM(iterator_init_null)
UTILLIB_TEST_END(utillib_slist)

