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

#include <utillib/test.h>
#include <utillib/string.h>
#include <string.h>

UTILLIB_TEST_SET_UP() {
  utillib_string_init(UT_FIXTURE);
}
UTILLIB_TEST_TEAR_DOWN() {
  utillib_string_destroy(UT_FIXTURE);
}

UTILLIB_TEST(init) {
  /* Initilizes to be empty with zero capacity and size */
  UTILLIB_TEST_ASSERT(utillib_string_empty(UT_FIXTURE));
  UTILLIB_TEST_ASSERT_EQ(0, utillib_string_capacity(UT_FIXTURE));
  UTILLIB_TEST_ASSERT_EQ(0, utillib_string_size(UT_FIXTURE));
  UTILLIB_TEST_ASSERT_STREQ("", utillib_string_c_str(UT_FIXTURE));
}

UTILLIB_TEST(init_c_str) {
  /* Once not empty, `size+1 <= capacity' holds */
  UTILLIB_TEST_STR(str, "A null-terminated string.");
  size_t LEN=strlen(str);
  utillib_string_init_c_str(UT_FIXTURE, str);
  const  char * content=utillib_string_c_str(UT_FIXTURE);
  size_t capacity=utillib_string_capacity(UT_FIXTURE);

  UTILLIB_TEST_MESSAGE("The content is `%s'", content); 
  UTILLIB_TEST_MESSAGE("The capacity is `%lu', size is `%lu'", capacity, LEN);
  UTILLIB_TEST_ASSERT_EQ(LEN, utillib_string_size(UT_FIXTURE));
  UTILLIB_TEST_ASSERT_LE(LEN+1, capacity);
  UTILLIB_TEST_ASSERT_STREQ(str, content);
}

UTILLIB_TEST(size) {
  UTILLIB_TEST_STR(empty_str, "");
  utillib_string_init_c_str(UT_FIXTURE, empty_str);
  UTILLIB_TEST_ASSERT(utillib_string_size(UT_FIXTURE) == 0);
}

UTILLIB_TEST(empty) {
  UTILLIB_TEST_ASSERT(utillib_string_empty(UT_FIXTURE));
  utillib_string_append(UT_FIXTURE, "");
  UTILLIB_TEST_ASSERT(utillib_string_empty(UT_FIXTURE));
  UTILLIB_TEST_ASSERT_STREQ(utillib_string_c_str(UT_FIXTURE), "");
  utillib_string_append(UT_FIXTURE, "1");
  UTILLIB_TEST_ASSERT_FALSE(utillib_string_empty(UT_FIXTURE));
  utillib_string_erase_last(UT_FIXTURE);
  UTILLIB_TEST_ASSERT_STREQ(utillib_string_c_str(UT_FIXTURE), "");
  UTILLIB_TEST_ASSERT(utillib_string_empty(UT_FIXTURE));
}

UTILLIB_TEST(clear) {
  utillib_string_append(UT_FIXTURE, "before first clear");
  utillib_string_clear(UT_FIXTURE);
  UTILLIB_TEST_ASSERT(utillib_string_empty(UT_FIXTURE));
  UTILLIB_TEST_ASSERT_STREQ("", utillib_string_c_str(UT_FIXTURE));
  if (0 == utillib_string_capacity(UT_FIXTURE)) {
    UTILLIB_TEST_ABORT("The current implementation of string should not free memory when doing clear!");
  }

  utillib_string_destroy(UT_FIXTURE);

  utillib_string_init_c_str(UT_FIXTURE, "before second clear");
  utillib_string_clear(UT_FIXTURE);
  UTILLIB_TEST_ASSERT(utillib_string_empty(UT_FIXTURE));
  UTILLIB_TEST_ASSERT_STREQ("", utillib_string_c_str(UT_FIXTURE));


}
UTILLIB_TEST(erase_last) {
  utillib_string_append(UT_FIXTURE, "1234");
  UTILLIB_TEST_EXPECT_EQ(4, utillib_string_size(UT_FIXTURE));
  UTILLIB_TEST_ASSERT_STREQ("1234", utillib_string_c_str(UT_FIXTURE));

  utillib_string_erase_last(UT_FIXTURE);
  UTILLIB_TEST_EXPECT_EQ(3, utillib_string_size(UT_FIXTURE));
  UTILLIB_TEST_ASSERT_STREQ("123", utillib_string_c_str(UT_FIXTURE));

  utillib_string_erase_last(UT_FIXTURE);
  UTILLIB_TEST_EXPECT_EQ(2, utillib_string_size(UT_FIXTURE));
  UTILLIB_TEST_ASSERT_STREQ("12", utillib_string_c_str(UT_FIXTURE));

  utillib_string_erase_last(UT_FIXTURE);
  UTILLIB_TEST_EXPECT_EQ(1, utillib_string_size(UT_FIXTURE));
  UTILLIB_TEST_ASSERT_STREQ("1", utillib_string_c_str(UT_FIXTURE));

  utillib_string_erase_last(UT_FIXTURE);
  UTILLIB_TEST_EXPECT_EQ(0, utillib_string_size(UT_FIXTURE));
  UTILLIB_TEST_ASSERT_STREQ("", utillib_string_c_str(UT_FIXTURE));

}
UTILLIB_TEST(replace_last) {
  utillib_string_append(UT_FIXTURE, "abcd");
  UTILLIB_TEST_ASSERT_STREQ("abcd", utillib_string_c_str(UT_FIXTURE));

  utillib_string_replace_last(UT_FIXTURE, '@');
  UTILLIB_TEST_ASSERT_EQ(4, utillib_string_size(UT_FIXTURE));
  UTILLIB_TEST_ASSERT_STREQ("abc@", utillib_string_c_str(UT_FIXTURE));

  utillib_string_replace_last(UT_FIXTURE, '#');
  UTILLIB_TEST_ASSERT_EQ(4, utillib_string_size(UT_FIXTURE));
  UTILLIB_TEST_ASSERT_STREQ("abc#", utillib_string_c_str(UT_FIXTURE));

  utillib_string_erase_last(UT_FIXTURE);
  utillib_string_replace_last(UT_FIXTURE, '#');
  UTILLIB_TEST_ASSERT_EQ(3, utillib_string_size(UT_FIXTURE));
  UTILLIB_TEST_ASSERT_STREQ("ab#", utillib_string_c_str(UT_FIXTURE));

}
UTILLIB_TEST(reserve) {
  UTILLIB_TEST_ASSERT_EQ(utillib_string_capacity(UT_FIXTURE), 0);
  utillib_string_reserve(UT_FIXTURE, 1);
  UTILLIB_TEST_ASSERT_EQ(utillib_string_capacity(UT_FIXTURE), 1);

  utillib_string_reserve(UT_FIXTURE, 10);
  UTILLIB_TEST_ASSERT_EQ(utillib_string_capacity(UT_FIXTURE), 10);
  for (int i=0;i<10;i++) {
    utillib_string_append_char(UT_FIXTURE, '$');
  }
  UTILLIB_TEST_ASSERT_EQ(utillib_string_size(UT_FIXTURE), 10);
  UTILLIB_TEST_MESSAGE("Now capacity is `%lu'", utillib_string_capacity(UT_FIXTURE));
  UTILLIB_TEST_MESSAGE("Now content is `%s'", utillib_string_c_str(UT_FIXTURE));
  /* There is a logical error here, we cannot ask string to ensure */ 
  /* size+1<=capacity while forbit it to do reserve if a reserve */
  /* has been issued and the following append deos not exceed this value */
  UTILLIB_TEST_ASSERT_EQ(utillib_string_capacity(UT_FIXTURE), 10);

  /* free up the memory */
  utillib_string_destroy(UT_FIXTURE);
  utillib_string_init(UT_FIXTURE);

  for (int i=0;i<20;i++) {
    UTILLIB_TEST_ASSERT_GE(
        utillib_string_capacity(UT_FIXTURE),
        utillib_string_size(UT_FIXTURE)+1
    );
    utillib_string_append_char(UT_FIXTURE, '$');
    UTILLIB_TEST_MESSAGE("Now capacity is `%lu'",
        utillib_string_capacity(UT_FIXTURE));
    }
}

UTILLIB_TEST(append) {}
UTILLIB_TEST(append_char) {}

UTILLIB_TEST(richcmp) {}
/**
 * Test Register
 */
UTILLIB_TEST_DEFINE(Utillib_String) {
  UTILLIB_TEST_BEGIN(Utillib_String)
  UTILLIB_TEST_RUN(init) 
  UTILLIB_TEST_RUN(init_c_str) 
  UTILLIB_TEST_RUN(size) 
  UTILLIB_TEST_RUN(empty) 
  UTILLIB_TEST_RUN(richcmp) 
  UTILLIB_TEST_RUN(clear) 
  UTILLIB_TEST_RUN(erase_last) 
  UTILLIB_TEST_RUN(replace_last) 
  UTILLIB_TEST_RUN(reserve) 
  UTILLIB_TEST_RUN(append) 
  UTILLIB_TEST_RUN(append_char) 
  UTILLIB_TEST_END(Utillib_String)

  UTILLIB_TEST_FIXTURE(utillib_string);
  UTILLIB_TEST_RETURN(Utillib_String);
}

