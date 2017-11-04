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
#include <utillib/bitset.h>
#include <utillib/test.h>
UTILLIB_TEST_SET_UP() {}
UTILLIB_TEST_TEAR_DOWN() {
  utillib_bitset_destroy(UT_FIXTURE);
}

UTILLIB_TEST(bitset_init) {
  struct utillib_bitset *bitset=UT_FIXTURE;
  utillib_bitset_init(bitset, 10);
  size_t expected_size=1 + 10 / sizeof *bitset->bits;
  UTILLIB_TEST_EXPECT_EQ(bitset->size, expected_size);
  UTILLIB_TEST_MESSAGE("bitset to hold %lu elements is of size %lu",
      10, expected_size);
}

UTILLIB_TEST(bitset_test_and_set) {
  UTILLIB_TEST_CONST(N, 32);
  utillib_bitset_init(UT_FIXTURE, N);
  for (size_t i=0;i<N; ++i) {
    utillib_bitset_set(UT_FIXTURE, i);
    bool Expected=utillib_bitset_test(UT_FIXTURE, i);
    UTILLIB_TEST_ASSERT(Expected);
  }
}
  
UTILLIB_TEST(bitset_reset) {
  utillib_bitset_init(UT_FIXTURE, 4);
  utillib_bitset_set(UT_FIXTURE, 2);
  UTILLIB_TEST_ASSERT_TRUE(utillib_bitset_test(UT_FIXTURE, 2));
  utillib_bitset_reset(UT_FIXTURE, 2);
  UTILLIB_TEST_ASSERT_FALSE(utillib_bitset_test(UT_FIXTURE, 2));
}

UTILLIB_TEST_DEFINE(Utillib_Bitset) {
  UTILLIB_TEST_BEGIN(Utillib_Bitset)
    UTILLIB_TEST_RUN(bitset_init)
    UTILLIB_TEST_RUN(bitset_test_and_set)
    UTILLIB_TEST_RUN(bitset_reset)
  UTILLIB_TEST_END(Utillib_Bitset)
  UTILLIB_TEST_FIXTURE(struct utillib_bitset)
  UTILLIB_TEST_RETURN(Utillib_Bitset)
}

