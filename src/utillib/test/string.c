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

UTILLIB_TEST_SET_UP() {
  utillib_string_init(UT_FIXTURE);
}
UTILLIB_TEST_TEAR_DOWN() {
  utillib_string_destroy(UT_FIXTURE);
}

UTILLIB_TEST(init) {}
UTILLIB_TEST(init_c_str) {}
UTILLIB_TEST(size) {}
UTILLIB_TEST(empty) {}
UTILLIB_TEST(richcmp) {}
UTILLIB_TEST(clear) {}
UTILLIB_TEST(erase_last) {}
UTILLIB_TEST(replace_last) {}
UTILLIB_TEST(reserve) {}
UTILLIB_TEST(append) {}
UTILLIB_TEST(append_char) {}

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

