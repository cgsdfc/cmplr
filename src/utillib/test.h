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

/**
 * \file utillib/test.h
 * A light weight testing framework inspired by GTest.
 * <para> This framework closely follows the style of GTest and wants
 * those who are already familiar to the compactness of GTest feel home. It
 * provides
 * a way to define test cases in a per-function basic and rich assertions,
 * comparation macros to check against pre/post conditions. What's more,
 * it groups tests around the unit of code under test(UUT) and supports
 * runnig multiple UUT all together.
 * </para>
 * <para> However, several restrictions exist to distinguish it from GTest
 * due to language limitation. They are:
 *
 * 1. No automactically registered tests. You have to register those tests
 * under the unit by yourselves via the macros `UTILLIB_TEST_DEFINE',
 * `UTILLIB_TEST_BEGIN',
 * `UTILLIB_TEST_RUN/SKIP' and so on. This is due to the lack of static
 * construction and
 * my unwillingness to use `__attribute__((__constructor__))'.
 *
 * 2. Without the support of template, the output of assertion failure is less
 * comprehensive
 * as it will only show you the text representation of the expression as
 * ASSERT(3) without
 * their values. For the same reason, you cannot stream arbitrary data into an
 * assertion to let
 * them shown when they failed. However, you can use the `boolean condition &&
 * "your message to show if failed"' convention to force a string to show up
 * together
 * with the condition.
 *
 * 3. About fixture, it does provide a way to use fixture in a sense of local
 * static variable.
 * That means you can put common initializing and destruction code in 2 places:
 * `setup' and `teardown'
 * related to exactly one global object. It sounds pretty dangerous, but in
 * fact, a properly initialized
 * object should validate all the operations on it and a proper destruction
 * should release any resource
 * it is holding making it prepared for a next initialization. Any well defined
 * code can satify the above
 * requirements and their author can free themselves from constantly writing
 * initializing and destruction
 * code. Therefore, simply call `UTILLIB_TEST_FIXTURE' can register a `setup' to
 * be called before the entry
 * of every test function and a `teardown' to be called after the exit of every
 * test function.
 * This deviates from GTest which creates a fresh object as fixture every time
 * your test is run.
 *
 * 4. It does not provide the features not documented in this file, such as dead
 * test.
 *
 * </para>
 * <para>
 * Future work: Maybe it is useful to generate different formats(json, xml,
 * html) of output based on
 * the test results, but this would necessarily make the code longer and take
 * much memory(writing files,
 * recording every assertion failures).
 * It may also be nice if a GUI based runner is implemented.
 */

#include "enum.h"
#include "types.h"
#include "vector.h"
#include <stdbool.h>
#include <stdio.h>  // for FILE*
#include <string.h> // for strcmp
#include <time.h>   // for time_t

/**
 * \macro UTILLIB_TEST_SETUP
 */
#define UTILLIB_TEST_SET_UP()                                                  \
  static void utillib_test_setup(utillib_test_fixture_t UT_FIXTURE)
#define UTILLIB_TEST_TEAR_DOWN()                                               \
  static void utillib_test_teardown(utillib_test_fixture_t UT_FIXTURE)

/**
 * \macro UTILLIB_TEST_CONST
 * Defines a constant scalar.
 */
#define UTILLIB_TEST_CONST(NAME, VALUE) const size_t NAME = (VALUE);

/**
 * \macro UTILLIB_TEST_STR
 * Defines a const C string.
 */
#define UTILLIB_TEST_STR(NAME, VALUE) const char *NAME = VALUE;

/**
 * \enum utillib_test_suite_t
 * Describe the status of a test: run it or skip it.
 * \value UT_STATUS_SKIP Skip this test.
 * \value UT_STATUS_RUN Run this test.
 */

UTILLIB_ENUM_BEGIN(utillib_test_status_t)
UTILLIB_ENUM_ELEM(UT_STATUS_SKIP)
UTILLIB_ENUM_ELEM(UT_STATUS_RUN)
UTILLIB_ENUM_END(utillib_test_status_t);

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
UTILLIB_ENUM_END(utillib_test_severity_t);

/**
 * \macro UTILLIB_TEST_DECLARE
 * Declares a function through which the test group
 * of a certain module can be initialized and obtained.
 * \param NAME the name of the function.
 * Use this in your header when your tests split multiple
 * files and the file containing `main' needs a declaraton.
 */
#define UTILLIB_TEST_DECLARE(NAME) utillib_test_env_t *NAME(void);
/**
 * \macro UTILLIB_TEST_RUN_ALL_ARG
 * Initilizes the test suite with the all the tests to run
 * and run them. Passed in command line arguments.
 * It should be called exactly once.
 * No checking is done.
 * \param ARGC argc.
 * \param ARGV argv.
 * \param ... The tests function pointers.
 */
#define UTILLIB_TEST_RUN_ALL_ARG(ARGC, ARGV, ...)                              \
  do {                                                                         \
    static utillib_test_suite_t static_suite = {.filename = __FILE__};         \
    utillib_test_suite_init(&static_suite, ##__VA_ARGS__, NULL);               \
    size_t rc = utillib_test_suite_run_all(&static_suite, (ARGC), (ARGV));     \
    utillib_test_suite_destroy(&static_suite);                                 \
    return rc;                                                                 \
  } while (0)
/**
 * \macro UTILLIB_TEST_RUN_ALL
 * Same as above but takes no command line arguments.
 * And uses their defaults.
 */
#define UTILLIB_TEST_RUN_ALL(...) UTILLIB_TEST_RUN_ALL_ARG(0, 0, __VA_ARGS__)
/**
 * \macro UTILLIB_TEST_LEN
 * Shortcut for getting the length of a constant array.
 */
#define UTILLIB_TEST_LEN(ARRAY) ((sizeof ARRAY) / (sizeof ARRAY[0]))
/**
 * \macro UTILLIB_TEST_AUX
 * Defines a helper function for common testing logic.
 * Thanks to `__VA_ARGS__', this function can take
 * whatever arguments it likes.
 */
#define UTILLIB_TEST_AUX(NAME, ...)                                            \
  static void NAME(utillib_test_entry_t *_UTILLIB_TEST_ENTRY_SELF,             \
                   utillib_test_fixture_t UT_FIXTURE, ##__VA_ARGS__)
/*
 * \macro UTILLIB_TEST_AUX_INVOKE
 * For calling helpers defined by `UTILLIB_TEST_AUX'.
 */
#define UTILLIB_TEST_AUX_INVOKE(NAME, ...)                                     \
  NAME(_UTILLIB_TEST_ENTRY_SELF, UT_FIXTURE, ##__VA_ARGS__);
/**
 * \macro UTILLIB_TEST_FIXTURE
 * Requires to use fixture.
 * \param TYPE The type of the fixture.
 */
#define UTILLIB_TEST_FIXTURE(TYPE)                                             \
  static TYPE UT_FIXTURE;                                                      \
  utillib_test_env_set_fixture(&static_test_env, &UT_FIXTURE,                  \
                               (utillib_test_setup), (utillib_test_teardown));
/**
 * \macro UT_FIXTURE
 * The identifier of the possibily defined fixture.
 * Use it only when you defined `UTILLIB_TEST_FIXTURE' properly.
 */
#define UT_FIXTURE _utillib_test_fixture

/**
 * \macro _UTILLIB_TEST_ENTRY_SELF
 * The name of the `self' pointer passed into a test.
 */
#define _UTILLIB_TEST_ENTRY_SELF _utillib_test_entry_self

/**
 * \macro UTILLIB_TEST
 * Defines a single test function with the fix signature.
 * \param NAME The name of the test function.
 */
#define UTILLIB_TEST(NAME)                                                     \
  static void NAME(utillib_test_entry_t *_UTILLIB_TEST_ENTRY_SELF,             \
                   utillib_test_fixture_t UT_FIXTURE)
/**
 * \macro UTILLIB_TEST_DEFINE
 * Defines a function whose purpose is to register a group of
 * test functions under the common name `NAME'.
 *
 * Side node: A major benefit of these function-wrapped local static definition
 * of variable is that you can avoid exposing details of the variable
 * to global scope and pretty much the same benefit of using singleton.
 * You get the pointer to the variable via a function call.
 */
#define UTILLIB_TEST_DEFINE(NAME) utillib_test_env_t *NAME(void)
/**
 * \macro UTILLIB_TEST_BEGIN
 * Defines a static array of `utillib_test_entry_t'.
 */
#define UTILLIB_TEST_BEGIN(NAME)                                               \
  static utillib_test_entry_t static_test_entries[] = {
/**
 * \macro UTILLIB_TEST_END
 * Ends the static `utillib_test_entry_t' array and
 * Defines a static `utillib_test_env_t' to hold it.
 */
#define UTILLIB_TEST_END(NAME)                                                 \
  { 0 }                                                                        \
  }                                                                            \
  ;                                                                            \
  static utillib_test_env_t static_test_env = {                                \
      .cases = static_test_entries, .case_name = #NAME, .filename = __FILE__};
/*
 * \macro UTILLIB_TEST_RETURN
 * Returns the wrapped static `static_test_env'.
 * Must be the last statement of the `UTILLIB_TEST_DEFINE'
 * function body.
 */
#define UTILLIB_TEST_RETURN(NAME) return &static_test_env;
/**
 * \macro _UTILLIB_TEST_ENTRY
 * Initilizer of a `utillib_test_entry_t' structure.
 * \param FUNC The identifier of the test function.
 * Only valid C identifier is acceptable.
 * \param STATUS whether to skip or to run this function.
 */
#define _UTILLIB_TEST_ENTRY(FUNC, STATUS)                                      \
  {                                                                            \
      .func = (FUNC), .func_name = #FUNC, .status = (STATUS),                  \
  },
/**
 * \macro UTILLIB_TEST_RUN
 * Derives from `_UTILLIB_TEST_ENTRY' and set `status' to UT_STATUS_RUN.
 */
#define UTILLIB_TEST_RUN(FUNC) _UTILLIB_TEST_ENTRY(FUNC, UT_STATUS_RUN)
/**
 * \macro UTILLIB_TEST_SKIP
 * Derives from `_UTILLIB_TEST_ENTRY' and set `status' to UT_STATUS_SKIP.
 */
#define UTILLIB_TEST_SKIP(FUNC) _UTILLIB_TEST_ENTRY(FUNC, UT_STATUS_SKIP)
/**
 * \macro _UTILLIB_INIT_PRED
 * Initilizes a `utillib_test_predicate_t' with all its fields.
 * Since we cannot use brace-initializing with runtime variables, we
 * call `utillib_test_predicate_init' here.
 * \param PRED The predicate to be initialized.
 * \param EXPR The expression to evaluated to yeild the `result' field.
 * \param MSG The message to display when `EXPR' evaluated to false.
 * \param SEVERITY The level of this failure which may change the control
 * flow of testing. It also has effect on the message displayed.
 */
#define _UTILLIB_INIT_PRED(PRED, EXPR, MSG, SEVERITY)                          \
  utillib_test_predicate_init(PRED, EXPR, __LINE__, MSG, SEVERITY)
/**
 * \macro UTILLIB_TEST_MESSAGE
 * Prints a message to stderr with line number, function name and newline.
 */
#define UTILLIB_TEST_MESSAGE(FMT, ...)                                         \
  utillib_test_message(__func__, __LINE__, (FMT), ##__VA_ARGS__);
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
    _UTILLIB_INIT_PRED(&predicate, (EXPR), #EXPR, US_ASSERT);                  \
    if (utillib_test_predicate(_UTILLIB_TEST_ENTRY_SELF, &predicate)) {        \
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
    _UTILLIB_INIT_PRED(&predicate, (EXPR), #EXPR, US_EXPECT);                  \
    (void)utillib_test_predicate(_UTILLIB_TEST_ENTRY_SELF, &predicate);        \
  } while (0)
/**
 * \macro UTILLIB_TEST_ABORT
 * Aborts the current test and the rest of tests in the same group.
 * This is used when a fatal error was detected such as short of memory.
 */
#define UTILLIB_TEST_ABORT(MSG)                                                \
  do {                                                                         \
    utillib_test_predicate_t predicate;                                        \
    _UTILLIB_INIT_PRED(&predicate, false, (MSG), US_ABORT);                    \
    utillib_test_predicate(_UTILLIB_TEST_ENTRY_SELF, &predicate);              \
    return;                                                                    \
  } while (0)
/**
 * Convenient macros for different comparation operators
 */
#define UTILLIB_TEST_EXPECT_TRUE(LHS) UTILLIB_TEST_EXPECT(LHS)
#define UTILLIB_TEST_EXPECT_FALSE(LHS) UTILLIB_TEST_EXPECT(!(LHS))
#define UTILLIB_TEST_EXPECT_EQ(LHS, RHS) UTILLIB_TEST_EXPECT(LHS == RHS)
#define UTILLIB_TEST_EXPECT_NE(LHS, RHS) UTILLIB_TEST_EXPECT(LHS != RHS)
#define UTILLIB_TEST_EXPECT_GE(LHS, RHS) UTILLIB_TEST_EXPECT(LHS >= RHS)
#define UTILLIB_TEST_EXPECT_LE(LHS, RHS) UTILLIB_TEST_EXPECT(LHS <= RHS)
#define UTILLIB_TEST_EXPECT_LT(LHS, RHS) UTILLIB_TEST_EXPECT(LHS < RHS)
#define UTILLIB_TEST_EXPECT_GT(LHS, RHS) UTILLIB_TEST_EXPECT(LHS > RHS)
/**
 * Assetions.
 */
#define UTILLIB_TEST_ASSERT_TRUE(LHS) UTILLIB_TEST_ASSERT(LHS)
#define UTILLIB_TEST_ASSERT_FALSE(LHS) UTILLIB_TEST_ASSERT(!(LHS))
#define UTILLIB_TEST_ASSERT_EQ(LHS, RHS) UTILLIB_TEST_ASSERT(LHS == RHS)
#define UTILLIB_TEST_ASSERT_NE(LHS, RHS) UTILLIB_TEST_ASSERT(LHS != RHS)
#define UTILLIB_TEST_ASSERT_GE(LHS, RHS) UTILLIB_TEST_ASSERT(LHS >= RHS)
#define UTILLIB_TEST_ASSERT_LE(LHS, RHS) UTILLIB_TEST_ASSERT(LHS <= RHS)
#define UTILLIB_TEST_ASSERT_LT(LHS, RHS) UTILLIB_TEST_ASSERT(LHS < RHS)
#define UTILLIB_TEST_ASSERT_GT(LHS, RHS) UTILLIB_TEST_ASSERT(LHS > RHS)
/**
 * String comparation.
 */
#define UTILLIB_TEST_ASSERT_STREQ(LHS, RHS)                                    \
  UTILLIB_TEST_ASSERT(strcmp((LHS), (RHS)) == 0)
#define UTILLIB_TEST_ASSERT_STRNE(LHS, RHS)                                    \
  UTILLIB_TEST_ASSERT(strcmp((LHS), (RHS)) != 0)
#define UTILLIB_TEST_EXPECT_STREQ(LHS, RHS)                                    \
  UTILLIB_TEST_EXPECT(strcmp((LHS), (RHS)) == 0)
#define UTILLIB_TEST_EXPECT_STRNE(LHS, RHS)                                    \
  UTILLIB_TEST_EXPECT(strcmp((LHS), (RHS)) != 0)
/**
 * \struct utillib_test_predicate_t
 * Helps to detect failures from predicates called within test functions
 * and pretty display them.
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
  /* Time in seconds spent in this test */
  time_t duration;
} utillib_test_entry_t;

/**
 * \struct utillib_test_env_t
 * Data of the test runner, which is mostly
 * different counting metrics.
 * \invariant ntests = nrun + ntests.
 * \invariant nrun = nfailure + nsuccess.
 */

typedef struct utillib_test_env_t {
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
  /* The fixture associated with this env */
  utillib_test_fixture_t fixture;
  utillib_test_fixfunc_t *setup_func, *teardown_func;
  /* The sum of seconds spent in each test that was run */
  time_t total_duration;
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
  char const *filename;
  utillib_vector tests;
  char const *json_output;
} utillib_test_suite_t;

/**
 * \struct utillib_test_dummy_t
 * A place holder when the content of the element
 * is not a matter.
 */
typedef struct utillib_test_dummy_t {
} utillib_test_dummy_t;

/**
 * Initilizes a predicate.
 */
void utillib_test_predicate_init(utillib_test_predicate_t *, bool, size_t,
                                 char const *, int);
/**
 * Do pass-or-fail on a predicate.
 */
bool utillib_test_predicate(utillib_test_entry_t *, utillib_test_predicate_t *);

/**
 * Initilizes a test suite.
 */
void utillib_test_suite_init(utillib_test_suite_t *, ...);

/**
 * \Destructs the test suite.
 */
void utillib_test_suite_destroy(utillib_test_suite_t *);
/**
 * Runs all the test suites.
 */
int utillib_test_suite_run_all(utillib_test_suite_t *, int, char **);

/**
 * Sets fixture for this utillib_test_env_t.
 */
void utillib_test_env_set_fixture(utillib_test_env_t *, utillib_test_fixture_t,
                                  utillib_test_fixfunc_t *,
                                  utillib_test_fixfunc_t *);
/**
 * Returns a static `utillib_test_dummy_t'.
 */
utillib_test_dummy_t *utillib_test_dummy(void);

/**
 * Prints a message to stderr.
 */
void utillib_test_message(char const *, size_t, char const *, ...);
#endif // UTILLIB_TEST_H
