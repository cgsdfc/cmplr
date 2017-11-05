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
#include <utillib/vector2.h>
#include <utillib/test.h>

UTILLIB_TEST_SET_UP() {}

UTILLIB_TEST_TEAR_DOWN() {
  utillib_vector2_destroy(UT_FIXTURE);
}
UTILLIB_TEST(vector2_at) {}
UTILLIB_TEST(vector2_set) {}


UTILLIB_TEST_DEFINE(Utillib_Vector2) {
  UTILLIB_TEST_BEGIN(Utillib_Vector)
  UTILLIB_TEST_RUN(vector2_at)
  UTILLIB_TEST_RUN(vector2_set)
  UTILLIB_TEST_END(Utillib_Vector2)
  UTILLIB_TEST_FIXTURE(struct utillib_vector2);
  UTILLIB_TEST_RETURN(Utillib_Vector2)
}

