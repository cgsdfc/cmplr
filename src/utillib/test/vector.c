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
#include <utillib/vector.h>

static void pop_back_Ntimes(struct utillib_vector *self, size_t N) {
  for (int i = 0; i < N; ++i) {
    utillib_vector_pop_back(self);
  }
}

static void push_back_Ntimes(struct utillib_vector *self, size_t N) {
  for (int i = 0; i < N; ++i) {
    utillib_vector_push_back(self, utillib_test_dummy());
  }
}

UTILLIB_TEST_SET_UP() { utillib_vector_init(UT_FIXTURE); }

UTILLIB_TEST_TEAR_DOWN() { utillib_vector_destroy(UT_FIXTURE); }

/**
 * Initialized to be empty with zero capacity.
 */
UTILLIB_TEST(init) {
  UTILLIB_TEST_ASSERT(utillib_vector_empty(UT_FIXTURE));
  UTILLIB_TEST_ASSERT(utillib_vector_capacity(UT_FIXTURE) == 0);
}

UTILLIB_TEST(push_back) {
  int N = 10;
  push_back_Ntimes(UT_FIXTURE, N);
  UTILLIB_TEST_ASSERT_EQ(N, utillib_vector_size(UT_FIXTURE));
  UTILLIB_TEST_ASSERT_GE(utillib_vector_capacity(UT_FIXTURE),
                         utillib_vector_size(UT_FIXTURE));
}

UTILLIB_TEST(pop_back) {
  int N = 10;
  push_back_Ntimes(UT_FIXTURE, N);
  for (int i = 0; i < N; ++i) {
    utillib_vector_pop_back(UT_FIXTURE);
    size_t size = utillib_vector_size(UT_FIXTURE);
    UTILLIB_TEST_ASSERT_EQ(size, N - i - 1);
  }
}

UTILLIB_TEST(front) {
  utillib_vector_push_back(UT_FIXTURE, utillib_test_dummy());
  UTILLIB_TEST_ASSERT_EQ(utillib_vector_front(UT_FIXTURE),
                         utillib_test_dummy());
}

UTILLIB_TEST(back) {
  int N = 20;
  for (size_t i = 0; i < N; ++i) {
    utillib_vector_push_back(UT_FIXTURE, (utillib_element_t)i);
    UTILLIB_TEST_EXPECT_EQ(utillib_vector_back(UT_FIXTURE),
                           (utillib_element_t)i);
  }
}

UTILLIB_TEST(empty) {}

/**
 * \test clear
 * \effect size becomes zero, capacity is unchanged.
 */
UTILLIB_TEST(clear) {
  push_back_Ntimes(UT_FIXTURE, 100);
  size_t old_capacity = utillib_vector_capacity(UT_FIXTURE);
  utillib_vector_clear(UT_FIXTURE);
  UTILLIB_TEST_ASSERT_EQ(0, utillib_vector_size(UT_FIXTURE));
  UTILLIB_TEST_ASSERT_EQ(old_capacity, utillib_vector_capacity(UT_FIXTURE));
}

/**
 * \test capacity
 * Ensures that capacity will increase as the size increases.
 */
UTILLIB_TEST(capacity) {
  size_t size, capacity, N;

  UTILLIB_TEST_ASSERT(utillib_vector_empty(UT_FIXTURE));
  push_back_Ntimes(UT_FIXTURE, 1);
  UTILLIB_TEST_ASSERT_EQ(1, (size = utillib_vector_size(UT_FIXTURE)));
  UTILLIB_TEST_ASSERT_EQ(2, (capacity = utillib_vector_capacity(UT_FIXTURE)));

  push_back_Ntimes(UT_FIXTURE, (N = 2));
  UTILLIB_TEST_ASSERT_EQ(3, utillib_vector_size(UT_FIXTURE));
  UTILLIB_TEST_ASSERT_EQ((capacity + 1) << 1,
                         utillib_vector_capacity(UT_FIXTURE));
  UTILLIB_TEST_MESSAGE("Before: size=%lu, capacity=%lu; After push_back %lu "
                       "elements, size=%lu, capacity=%lu.",
                       size, capacity, utillib_vector_size(UT_FIXTURE), N,
                       utillib_vector_capacity(UT_FIXTURE));
}

UTILLIB_TEST(reserve) {
  utillib_vector_reserve(UT_FIXTURE, 10); 
}

UTILLIB_TEST(foreach) {
  int N = 100;
  size_t i = 0;
  for (i = 0; i < N; ++i) {
    utillib_vector_push_back(UT_FIXTURE, (utillib_element_t)i);
  }
  i = 0;
  UTILLIB_VECTOR_FOREACH(size_t, elem, (struct utillib_vector *)UT_FIXTURE) {
    UTILLIB_TEST_ASSERT_EQ(elem, i);
    i++;
  }
}

UTILLIB_TEST_DEFINE(Utillib_Vector) {
  UTILLIB_TEST_BEGIN(Utillib_Vector)
  UTILLIB_TEST_RUN(init)
  UTILLIB_TEST_RUN(push_back)
  UTILLIB_TEST_RUN(pop_back)
  UTILLIB_TEST_RUN(foreach)
  UTILLIB_TEST_RUN(front)
  UTILLIB_TEST_RUN(back)
  UTILLIB_TEST_RUN(capacity)
  UTILLIB_TEST_RUN(reserve)
  UTILLIB_TEST_END(Utillib_Vector)
  UTILLIB_TEST_FIXTURE(struct utillib_vector);
  UTILLIB_TEST_RETURN(Utillib_Vector)
}
