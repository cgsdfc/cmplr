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

/**
 * \file utillib/test.c
 * Tests Utillib.Test.
 */

UTILLIB_TEST_DECLARE(Utillib_Test);

UTILLIB_TEST_CONST (deliberily_failure, 1);

UTILLIB_TEST(message) {
  UTILLIB_TEST_MESSAGE("A plain message");
  UTILLIB_TEST_MESSAGE("A message with format `%s', `%d', `%c'", "string", 4, 'c');
}

UTILLIB_TEST(assertion_failed) {
  UTILLIB_TEST_MESSAGE("Assertion failure should terminate the current test");
  UTILLIB_TEST_ASSERT(false && deliberily_failure);
  UTILLIB_TEST_MESSAGE("should not reach here!!");
}

UTILLIB_TEST(assertion_passed) {
  UTILLIB_TEST_ASSERT(true);
  UTILLIB_TEST_ASSERT(1 > 0);
  UTILLIB_TEST_ASSERT_TRUE("string is true");
  UTILLIB_TEST_ASSERT_FALSE(NULL && "NULL should be false");
  UTILLIB_TEST_ASSERT_EQ(1, 1);
  UTILLIB_TEST_ASSERT_NE(1, 0);
  UTILLIB_TEST_ASSERT_GE(2, 1);
  UTILLIB_TEST_ASSERT_LE(1, 2);
  UTILLIB_TEST_ASSERT_LT(-1,2);
  UTILLIB_TEST_ASSERT_GT(2, -1);
}

UTILLIB_TEST(expect_passed) {
  UTILLIB_TEST_EXPECT(true);
  UTILLIB_TEST_EXPECT(1 > 0);
  UTILLIB_TEST_EXPECT_TRUE("string is true");
  UTILLIB_TEST_EXPECT_FALSE(NULL && "NULL should be false");
  UTILLIB_TEST_EXPECT_EQ(1, 1);
  UTILLIB_TEST_EXPECT_NE(1, 0);
  UTILLIB_TEST_EXPECT_GE(2, 1);
  UTILLIB_TEST_EXPECT_LE(1, 2);
  UTILLIB_TEST_EXPECT_LT(-1,2);
  UTILLIB_TEST_EXPECT_GT(2, -1);
}

UTILLIB_TEST(expect_failed) {
  UTILLIB_TEST_MESSAGE("Expect failure should not terminate the current test");
  UTILLIB_TEST_EXPECT(false && deliberily_failure);
  UTILLIB_TEST_EXPECT_EQ(1, 0);
}

UTILLIB_TEST(should_be_skip) {}

/** \test statistics_correct
 * Must be called at the end of all tests
 * and before `abort_failed' to check 
 * for correct statistics.
*/

UTILLIB_TEST(statistics_correct) {
  // sort of reflection.
  utillib_test_env_t * env=Utillib_Test();
  UTILLIB_TEST_ASSERT_EQ(env->nrun+env->nskipped, env->ntests);
  UTILLIB_TEST_ASSERT_EQ(env->nfailure+env->nsuccess, env->nrun);
  UTILLIB_TEST_ASSERT_EQ(env->nrun, 5);
  UTILLIB_TEST_ASSERT_EQ(env->nfailure, 2);
  UTILLIB_TEST_ASSERT_EQ(env->ntests, 6);
  UTILLIB_TEST_ASSERT_EQ(env->nskipped, 1);
  UTILLIB_TEST_ASSERT_EQ(env->nsuccess, 3);
}

UTILLIB_TEST(abort_failed) {
  UTILLIB_TEST_ABORT("Say, fatal error, ENOMEM!");
}

UTILLIB_TEST(should_be_skip_by_abort) {
  UTILLIB_TEST_ASSERT(false && "should not reach here");
}

UTILLIB_TEST_SET_UP() {
  puts("setup function called");
}
UTILLIB_TEST_TEAR_DOWN() {
  puts("tear down function called");
}

UTILLIB_TEST_DEFINE(Utillib_Test) {
  UTILLIB_TEST_BEGIN(Utillib_Test)
  UTILLIB_TEST_RUN(message)
  UTILLIB_TEST_RUN(assertion_failed)
  UTILLIB_TEST_RUN(assertion_passed)
  UTILLIB_TEST_RUN(expect_passed)
  UTILLIB_TEST_RUN(expect_failed)
  UTILLIB_TEST_SKIP(should_be_skip)
  UTILLIB_TEST_RUN(statistics_correct)
  UTILLIB_TEST_RUN(abort_failed)
  UTILLIB_TEST_RUN(should_be_skip_by_abort)
  UTILLIB_TEST_END(Utillib_Test)
  struct test_fixture {};
  UTILLIB_TEST_FIXTURE(struct test_fixture);
  UTILLIB_TEST_RETURN(Utillib_Test)
}

