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

UTILLIB_TEST_TEAR_DOWN() { utillib_bitset_destroy(UT_FIXTURE); }

UTILLIB_TEST(bitset_init) {
  struct utillib_bitset *bitset = UT_FIXTURE;
  utillib_bitset_init(bitset, 10);
  size_t expected_size = 1 + 10 / sizeof *bitset->bits;
  UTILLIB_TEST_EXPECT_EQ(bitset->size, expected_size);
  UTILLIB_TEST_MESSAGE("bitset to hold %lu elements is of size %lu", 10,
                       expected_size);
}

UTILLIB_TEST(bitset_contains_and_insert) {
  UTILLIB_TEST_CONST(N, 32);
  utillib_bitset_init(UT_FIXTURE, N);
  for (size_t i = 0; i < N; ++i) {
    utillib_bitset_insert(UT_FIXTURE, i);
    bool Expected = utillib_bitset_contains(UT_FIXTURE, i);
    UTILLIB_TEST_ASSERT(Expected);
  }
}

UTILLIB_TEST(bitset_remove) {
  utillib_bitset_init(UT_FIXTURE, 4);
  utillib_bitset_insert(UT_FIXTURE, 2);
  UTILLIB_TEST_ASSERT_TRUE(utillib_bitset_contains(UT_FIXTURE, 2));
  utillib_bitset_remove(UT_FIXTURE, 2);
  UTILLIB_TEST_ASSERT_FALSE(utillib_bitset_contains(UT_FIXTURE, 2));
}

UTILLIB_TEST_AUX(fill_with_odd_number, size_t N) {
  UTILLIB_TEST_MESSAGE("Fill in some odd numbers in range %lu", N);

  utillib_bitset_init(UT_FIXTURE, N);
  for (size_t i = 0; i < N; ++i) {
    if (i & 1)
      utillib_bitset_insert(UT_FIXTURE, i);
  }
}

UTILLIB_TEST(bitset_json_array) {
  UTILLIB_TEST_AUX_INVOKE(fill_with_odd_number, 10);
  struct utillib_json_value *val =
      utillib_bitset_json_array_create(UT_FIXTURE);
  utillib_json_pretty_print(val);
  utillib_json_value_destroy(val);
}

UTILLIB_TEST(bitset_json_empty) {
  struct utillib_bitset *EMPTY = UT_FIXTURE;
  utillib_bitset_init(EMPTY, 10);
  struct utillib_json_value *val =
      utillib_bitset_json_array_create(EMPTY);
  utillib_json_pretty_print(val);
  utillib_json_value_destroy(val);
}

UTILLIB_TEST_AUX(bitset_init2, struct utillib_bitset *A,
                 struct utillib_bitset *B, size_t N) {
  utillib_bitset_init(A, N);
  utillib_bitset_init(B, N);
}

UTILLIB_TEST_AUX(bitset_destroy2, struct utillib_bitset *A,
                 struct utillib_bitset *B) {
  utillib_bitset_destroy(A);
  utillib_bitset_destroy(B);
}

UTILLIB_TEST(bitset_equal) {
  UTILLIB_TEST_CONST(N, 100);
  struct utillib_bitset b0, b1;
  UTILLIB_TEST_AUX_INVOKE(bitset_init2, &b0, &b1, N);
  UTILLIB_TEST_ASSERT(utillib_bitset_equal(&b0, &b1));
  for (size_t i = 0; i < N; i += 3) {
    utillib_bitset_insert(&b0, i);
    utillib_bitset_insert(&b1, i);
  }
  UTILLIB_TEST_EXPECT(utillib_bitset_equal(&b0, &b1));
  UTILLIB_TEST_AUX_INVOKE(bitset_destroy2, &b0, &b1);
}

UTILLIB_TEST(bitset_is_interset) {
  UTILLIB_TEST_CONST(N, 100);
  struct utillib_bitset b0, b1;
  UTILLIB_TEST_AUX_INVOKE(bitset_init2, &b0, &b1, N);
  for (size_t i = 0; i < N; ++i) {
    if (i % 2)
      utillib_bitset_insert(&b0, i);
    else
      utillib_bitset_insert(&b1, i);
  }
  UTILLIB_TEST_EXPECT_FALSE(utillib_bitset_is_intersect(&b0, &b1));
  UTILLIB_TEST_AUX_INVOKE(bitset_destroy2, &b0, &b1);
}

UTILLIB_TEST_DEFINE(Utillib_Bitset) {
  UTILLIB_TEST_BEGIN(Utillib_Bitset)
  UTILLIB_TEST_RUN(bitset_init)
  UTILLIB_TEST_RUN(bitset_contains_and_insert)
  UTILLIB_TEST_RUN(bitset_remove)
  UTILLIB_TEST_RUN(bitset_equal)
  UTILLIB_TEST_RUN(bitset_is_interset)
  UTILLIB_TEST_RUN(bitset_json_array)
  UTILLIB_TEST_RUN(bitset_json_empty)
  UTILLIB_TEST_END(Utillib_Bitset)
  UTILLIB_TEST_FIXTURE(struct utillib_bitset)
  UTILLIB_TEST_RETURN(Utillib_Bitset)
}
