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
#ifndef UTILLIB_TEST_H
#define UTILLIB_TEST_H
#include "enum.h"
#include "typedef.h"
#include "unordered_map.h"
#include <stdbool.h>

/**
 * \enum utillib_test_suite_t
 * Describe the status of a test: run it or skip it.
 * \value UT_STATUS_SKIP Skip this test.
 * \value UT_STATUS_RUN Run this test.
 */

UTILLIB_ENUM_BEGIN(utillib_test_status_t)
UTILLIB_ENUM_ELEM(UT_STATUS_SKIP)
UTILLIB_ENUM_ELEM(UT_STATUS_RUN)
UTILLIB_ENUM_END(utillib_test_status_t)

/**
 * \enum utillib_test_severity_t
 * Describe the severity of the test predicates
 * causing different things to happen.
 * \value US_EXPECT if failed, prints a message and
 * carries on with the current test.
 * \value US_ASSERT if failed, prints a message and
 * skips to the next test.
 * \value US_ABORT if failed, aborts the current group
 * of test functions and goes on to next group.
 */

UTILLIB_ENUM_BEGIN(utillib_test_severity_t)
UTILLIB_ENUM_ELEM(US_SUCCESS)
UTILLIB_ENUM_ELEM(US_EXPECT)
UTILLIB_ENUM_ELEM(US_ASSERT)
UTILLIB_ENUM_ELEM(US_ABORT)
UTILLIB_ENUM_END(utillib_test_severity_t)

/**
 * \macro UTILLIB_TEST_DECLARE
 * Declares a function through which the test group
 * of a certain module can be initialized and obtained.
 * \param NAME the name of the function.
 */

#define UTILLIB_TEST_DECLARE(NAME) utillib_test_env_t *NAME(void);

#define UTILLIB_TEST_RUN_ALL_ARG(ARGC, ARGV, ...)                                  \
  do {                                                                         \
    static utillib_test_suite_t static_suite = {.filename = __FILE__};         \
    utillib_test_suite_init(&static_suite, ## __VA_ARGS__, NULL);               \
    return utillib_test_suite_run_all(&static_suite);                          \
  } while (0)

#define UTILLIB_TEST_RUN_ALL(...) UTILLIB_TEST_RUN_ALL_ARG(0,0, __VA_ARGS__)

/**
 * \macro UTILLIB_TEST_GLOBAL_SETUP
 * Defines global setup function to be called before
 * the whole test suite is run.
 */
#define UTILLIB_TEST_GLOBAL_SETUP() void utillib_test_global_setup()
#define UTILLIB_TEST_GLOBAL_TEARDOWN() void utillib_test_global_teardown()

/**
 * \macro UTILLIB_TEST
 * Defines a single test function with the fix signature.
 * \param NAME The name of the test function.
 */

#define UTILLIB_TEST(NAME) static void NAME(utillib_test_entry_t *self)

/**
 * \macro UTILLIB_TEST_BEGIN
 * Begins to register a group of test functions under
 * the common name `NAME'.
 * It actually defined a function that returns
 * a static `utillib_test_env_t'.
 * A major benefit of these function-wrapped local static definition
 * of variable is that you can avoid exposing details of the variable
 * to global scope and pretty much the same benefit of using singleton.
 * You get the pointer to the variable via a function call.
 */
#define UTILLIB_TEST_BEGIN(NAME)                                               \
  utillib_test_env_t *NAME(void) {                                             \
  static utillib_test_entry_t static_test_entries[] = {

/**
 * \macro UTILLIB_TEST_END
 * Initilizes the `static_test_entries' and returns it.
 */

#define UTILLIB_TEST_END(NAME)                                                 \
  { 0 }                                                                        \
  }                                                                            \
  ;                                                                            \
  static utillib_test_env_t static_test_env = {                                \
      .cases = static_test_entries, .case_name = #NAME, .filename = __FILE__}; \
  return &static_test_env;                                                     \
  }

/**
 * \macro UTILLIB_TEST_ENTRY
 * Initilizer of a `utillib_test_entry_t' structure.
 * \param FUNC The identifier of the test function.
 * Only valid C identifier is acceptable.
 * \param STATUS whether to skip or to run this function.
 */

#define UTILLIB_TEST_ENTRY(FUNC, STATUS)                                 \
  {.func = (FUNC), .func_name = #FUNC, .status = (STATUS), },

/**
 * \macro UTILLIB_TEST_RUN
 * Derives from `UTILLIB_TEST_ENTRY' and set `status' to UT_STATUS_RUN.
 */

#define UTILLIB_TEST_RUN(FUNC)                                           \
  UTILLIB_TEST_ENTRY(FUNC,UT_STATUS_RUN)

/**
 * \macro UTILLIB_TEST_SKIP
 * Derives from `UTILLIB_TEST_ENTRY' and set `status' to UT_STATUS_SKIP.
 */

#define UTILLIB_TEST_SKIP(FUNC)                                          \
  UTILLIB_TEST_ENTRY(FUNC, UT_STATUS_SKIP)

/**
 * \macro UTILLIB_INIT_PRED
 * Initilizes a `utillib_test_predicate_t' with all its fields.
 * Since we cannot use initializer with runtime variables, we
 * call `utillib_test_predicate_init' here.
 * \param PRED The predicate to be initialized.
 * \param EXPR The expression to evaluated to yeild the `result' field.
 * \param MSG The message to display when `EXPR' evaluated to false.
 * \param SEVERITY The level of this failure which may change the control
 * flow of testing. It also has effect on the message displayed.
 */

#define UTILLIB_INIT_PRED(PRED, EXPR, MSG, SEVERITY)                           \
  utillib_test_predicate_init(PRED, EXPR, __LINE__, MSG, SEVERITY)

/**
 * \macro UTILLIB_TEST_ASSERT
 * Assert `EXPR' to be true or prints `Assertion Failed...'
 * and terminates the current test function by `return' from it.
 * \param EXPR The EXPR to assert.
 * Notes that these predicates can only be legally called from within
 * a test function defined with `UTILLIB_TEST' since they require the
 * `self' pointer to the running `utillib_test_entry_t' object.
 */

#define UTILLIB_TEST_ASSERT(EXPR)                                              \
  do {                                                                         \
    utillib_test_predicate_t predicate;                                        \
    UTILLIB_INIT_PRED(&predicate, (EXPR), #EXPR, US_ASSERT);                   \
    if (utillib_test_predicate(self, &predicate)) {                            \
      break;                                                                   \
    }                                                                          \
    return;                                                                    \
  } while (0)

/**
 * \macro UTILLIB_TEST_EXPECT
 * Expects `EXPR' to be true or prints `Expected...'
 * and continues with the current test.
 */

#define UTILLIB_TEST_EXPECT(EXPR)                                              \
  do {                                                                         \
    utillib_test_predicate_t predicate;                                        \
    UTILLIB_INIT_PRED(&predicate, (EXPR), #EXPR, US_EXPECT);                   \
    (void)utillib_test_predicate(self, &predicate);                            \
  } while (0)

/**
 * \macro UTILLIB_TEST_ABORT
 * Aborts the current test and the rest of tests in the same group.
 * This is used when a fatal error was detected such as short of memory.
 */

#define UTILLIB_TEST_ABORT(MSG)                                                \
  do {                                                                         \
    utillib_test_predicate_t predicate;                                        \
    UTILLIB_INIT_PRED(&predicate, false, (MSG), US_ABORT);                     \
    utillib_test_predicate(self, &predicate);                                  \
    return;                                                                    \
  } while (0)

#define UTILLIB_TEST_EXPECT_EQ(LHS, RHS) UTILLIB_TEST_EXPECT(LHS == RHS)

#define UTILLIB_TEST_EXPECT_NE(LHS, RHS) UTILLIB_TEST_EXPECT(LHS != RHS)

#define UTILLIB_TEST_EXPECT_GE(LHS, RHS) UTILLIB_TEST_EXPECT(LHS >= RHS)

#define UTILLIB_TEST_EXPECT_LE(LHS, RHS) UTILLIB_TEST_EXPECT(LHS <= RHS)

#define UTILLIB_TEST_EXPECT_LT(LHS, RHS) UTILLIB_TEST_EXPECT(LHS < RHS)

#define UTILLIB_TEST_EXPECT_GT(LHS, RHS) UTILLIB_TEST_EXPECT(LHS > RHS)

#define UTILLIB_TEST_ASSERT_EQ(LHS, RHS) UTILLIB_TEST_ASSERT(LHS == RHS)

#define UTILLIB_TEST_ASSERT_NE(LHS, RHS) UTILLIB_TEST_ASSERT(LHS != RHS)

#define UTILLIB_TEST_ASSERT_GE(LHS, RHS) UTILLIB_TEST_ASSERT(LHS >= RHS)

#define UTILLIB_TEST_ASSERT_LE(LHS, RHS) UTILLIB_TEST_ASSERT(LHS <= RHS)

#define UTILLIB_TEST_ASSERT_LT(LHS, RHS) UTILLIB_TEST_ASSERT(LHS < RHS)

#define UTILLIB_TEST_ASSERT_GT(LHS, RHS) UTILLIB_TEST_ASSERT(LHS > RHS)

/**
 * \struct utillib_test_predicate_t
 * Helps to detect failures from predicates called within test functions
 * and display them prettily.
 */

typedef struct utillib_test_predicate_t {
  /* Records the result evaluated from predicate expression */
  bool result;
  /* The source line on which the predicate was invoked */
  size_t line;
  /* A string representation of the predicate expression */
  char const *expr_str;
  /* The severity of the failure if happened */
  int severity;
} utillib_test_predicate_t;

/**
 * \struct utillib_test_entry_t
 * Meta info wrapped around a test function pointer
 * so that the framework can respond to the execution
 * of the test and form comprehensive summary.
 */

typedef struct utillib_test_entry_t {
  /* Function pointer to this test */
  utillib_test_func_t *func;
  /* Name of the test function */
  char const *func_name;
  int status;
  /* Counters of Expects failure */
  size_t expect_failure;

  /* Should be zero or one */
  size_t assert_failure;
  size_t abort_failure;
  /* Flag indicating whether this test finished without any failure */
  bool succeeded;
} utillib_test_entry_t;

/**
 * \struct utillib_test_env_t
 * Data of the test runner, which is mostly
 * different counting metrics.
 * \invariant ntests = nrun + ntests.
 * \invariant nrun = nfailure + nsuccess.
 */

typedef struct utillib_test_env_t {
  /* The current activated test */
  utillib_test_entry_t *cur_test;
  /* A null-terminated array of `utillib_test_entry_t' */
  utillib_test_entry_t *cases;
  /* The name of file where this `utillib_test_env_t' was defined. */
  char const *filename;
  /* The module name for which these tests were written for */
  char const *case_name;
  /* The total number of `utillib_test_entry_t' in the array */
  size_t ntests;
  /* The number of tests that were skipped. */
  size_t nskipped;
  /* The number of tests that were run */
  size_t nrun;
  /* The number of tests that failed due any predicate failure */
  size_t nfailure;
  /* The number of golden tests that turned green */
  size_t nsuccess;
} utillib_test_env_t;

/**
 * \struct utillib_test_suite_t
 * Top controler of the whole test process.
 * It parses program options (unimplemented),
 * conducts all the tests via `utillib_test_env_t's
 * and generates report of different formats.
 * Possibily it will run a GUI interface but that is
 * unimplemented.
 */

typedef struct utillib_test_suite_t {
  char const * filename;
  utillib_vector tests;
} utillib_test_suite_t;

void utillib_test_predicate_init(utillib_test_predicate_t *, bool, size_t,
    char const *, int);
bool utillib_test_predicate(utillib_test_entry_t *, utillib_test_predicate_t *);

void utillib_test_suite_init(utillib_test_suite_t *, ...);
int utillib_test_suite_run_all(utillib_test_suite_t *);

#endif // UTILLIB_TEST_H
