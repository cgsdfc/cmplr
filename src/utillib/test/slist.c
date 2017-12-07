/*
   Cmplr Library
   Copyright (C) 2017-2018 Cong Feng <cgsdfc@126.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301 USA

*/
#include <utillib/slist.h>
#include <utillib/test.h>
#define strings_size 4

static char const *const strings[] = {"#0", /* 3 */
                                      "#1", /* 2 */
                                      "#2", /* 1 */
                                      "#3", /* 0 */
                                      NULL};

UTILLIB_TEST_SET_UP() { utillib_slist_init(UT_FIXTURE); }
UTILLIB_TEST_TEAR_DOWN() { utillib_slist_destroy(UT_FIXTURE); }

UTILLIB_TEST_AUX(slist_fill_strings) {
  for (char const **str = strings; *str != NULL; ++str) {
    utillib_slist_push_front(UT_FIXTURE, *str);
  }
}

UTILLIB_TEST(slist_push_front) {
  UTILLIB_TEST_AUX_INVOKE(slist_fill_strings);
  UTILLIB_TEST_ASSERT_EQ(utillib_slist_size(UT_FIXTURE), strings_size);
  UTILLIB_SLIST_FOREACH(char const *, str,
                        (struct utillib_slist const *)UT_FIXTURE) {
    puts(str);
  }
}

UTILLIB_TEST(slist_front_op) {
  char const *front = "Front letter";
  utillib_slist_push_front(UT_FIXTURE, front);
  char const *actual_front = utillib_slist_front(UT_FIXTURE);
  UTILLIB_TEST_ASSERT_EQ(actual_front, front);
  UTILLIB_TEST_ASSERT_EQ(utillib_slist_size(UT_FIXTURE), 1);
  utillib_slist_pop_front(UT_FIXTURE);
  UTILLIB_TEST_ASSERT_EQ(utillib_slist_size(UT_FIXTURE), 0);
}

UTILLIB_TEST(slist_empty) {
  UTILLIB_TEST_ASSERT(utillib_slist_empty(UT_FIXTURE));
}

UTILLIB_TEST(slist_erase) {
  char const *erased_val;
  /* Erase when empty. */
  for (int pos = 0; pos < 3; ++pos) {
    erased_val = utillib_slist_erase(UT_FIXTURE, pos);
    UTILLIB_TEST_ASSERT_EQ(erased_val, NULL);
  }

  UTILLIB_TEST_AUX_INVOKE(slist_fill_strings);

  /* Head special case */
  erased_val = utillib_slist_erase(UT_FIXTURE, 0);
  UTILLIB_TEST_ASSERT_EQ(erased_val, strings[strings_size - 1]);
  UTILLIB_TEST_ASSERT_EQ(utillib_slist_size(UT_FIXTURE), strings_size - 1);

  erased_val = utillib_slist_erase(UT_FIXTURE, 0);
  UTILLIB_TEST_ASSERT_EQ(erased_val, strings[strings_size - 2]);
  UTILLIB_TEST_ASSERT_EQ(utillib_slist_size(UT_FIXTURE), strings_size - 2);

  erased_val = utillib_slist_erase(UT_FIXTURE, 1);
  puts(erased_val);
  UTILLIB_TEST_ASSERT_EQ(utillib_slist_size(UT_FIXTURE), strings_size - 3);
}

/*
 * Tests some special case
 */
UTILLIB_TEST(slist_erase2) {
  char const *erased_val;
  UTILLIB_TEST_AUX_INVOKE(slist_fill_strings);

  erased_val = utillib_slist_erase(UT_FIXTURE, -1);
  UTILLIB_TEST_ASSERT_EQ(NULL, erased_val);

  erased_val = utillib_slist_erase(UT_FIXTURE, strings_size);
  UTILLIB_TEST_ASSERT_EQ(NULL, erased_val);

  /* #3 #2 #1 #0 */
  erased_val = utillib_slist_erase(UT_FIXTURE, 3);
  UTILLIB_TEST_ASSERT_EQ(erased_val, strings[0]);
  /* #3 #2 #1 */
  erased_val = utillib_slist_erase(UT_FIXTURE, 1);
  UTILLIB_TEST_ASSERT_EQ(erased_val, strings[2]);
  /* #3 #1 */
  erased_val = utillib_slist_erase(UT_FIXTURE, 0);
  UTILLIB_TEST_ASSERT_EQ(erased_val, strings[3]);
  /* #1 */
  UTILLIB_TEST_ASSERT_EQ(utillib_slist_front(UT_FIXTURE), strings[1]);
}

UTILLIB_TEST_DEFINE(Utillib_Slist) {
  UTILLIB_TEST_BEGIN(Utillib_Slist)
  UTILLIB_TEST_RUN(slist_push_front)
  UTILLIB_TEST_RUN(slist_front_op)
  UTILLIB_TEST_RUN(slist_empty)
  UTILLIB_TEST_RUN(slist_erase)
  UTILLIB_TEST_RUN(slist_erase2)
  UTILLIB_TEST_END(Utillib_Slist)
  UTILLIB_TEST_FIXTURE(struct utillib_slist)
  UTILLIB_TEST_RETURN(Utillib_Slist)
}
