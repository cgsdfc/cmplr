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
/**
 * \file utillib/test.c
 * \brief Implements the test framework described
 * in utillib/test.h.
 * \usage How to use it to do unit test.
 * Supposed we have a file called vector.c that
 * defines a vector in the common sense. Here is
 * how you can write the tests:
 * <code>
 * In test.c
 * UTILLIB_SETUP() {
 *   utillib_vector_init(UTILLIB_TEST_FIXTURE);
 *  }
 * UTILLIB_TEARDOWN() {
 *   utillib_vector_destroy(UTILLIB_TEST_FIXTURE);
 * }
 * UTILLIB_TEST(InitialToBeEmpty) {
 *   utillib_vector * vector=UTILLIB_TEST_FIXTURE;
 *   UTILLIB_TEST_ASSERT(0 == utillib_vector_size(vector) &&
 * utillib_vector_empty(vector));
 * }
 *
 * UTILLIB_TEST(size) {
 *  ...
 * }
 * UTILLIB_TEST_BEGIN(Utillib_Vector)
 * UTILLIB_TEST_RUN(InitialToBeEmpty, "vector should be empty immediately after
 * calling `utillib_vector_init'")
 * UTILLIB_TEST_RUN(...)
 * UTILLIB_TEST_END(Utillib_Vector)
 *
 * // If you want to build `Utillib_Vector' as a part of bigger
 * // test suite, you should give it a header with a
 * `UTILLIB_TEST_DECLARE(Utillib_Vector)'
 * // along with other helper functions.
 * // Otherwise, you can just use the `UTILLIB_TEST_RUN_ALL(argc, argv)'
 * // in the `main' to turn it into a stand-alone test case.
 * </code>
 * <code>
 * In test.h
 * UTILLIB_TEST_DECLARE(Utillib_Vector);
 *
 */

#include "test.h"
#include "color.h"  // for COLOR_STRING_UNBOLD
#include <stdarg.h> // for va_list
#define COLOR_STRING(C, S) COLOR_STRING_UNBOLD(C, S)

/**
 * Colorful status bar for display.
 */

/* Two kinds of status bars with red and green */
static const char *GREEN_BANG = COLOR_STRING(GREEN, "[==========]");
static const char *GREEN_DASH = COLOR_STRING(GREEN, "[----------]");
static const char *RED_BANG = COLOR_STRING(RED, "[==========]");
static const char *RED_DASH = COLOR_STRING(RED, "[----------]");

/* different kinds of text indicator */
static const char *WHITE_SKIP = "[  SKIPPED ]";
static const char *GREEN_RUN = COLOR_STRING(GREEN, "[ RUN      ]");
static const char *GREEN_PASS = COLOR_STRING(GREEN, "[  PASSED  ]");
static const char *GREEN_OK = COLOR_STRING(GREEN, "[       OK ]");
static const char *RED_BAD = COLOR_STRING(RED, "[ BAD      ]");
static const char *RED_FAILED = COLOR_STRING(RED, "[  FAILED  ]");

/**
 * Strings associated with `utillib_test_severity_t'.
 */
UTILLIB_ETAB_BEGIN(utillib_test_severity_t)
UTILLIB_ETAB_ELEM_INIT(US_EXPECT, "Expected")
UTILLIB_ETAB_ELEM_INIT(US_ASSERT, "Assertion Failed")
UTILLIB_ETAB_ELEM_INIT(US_ABORT, "Abort")
UTILLIB_ETAB_END(utillib_test_severity_t)

/**
 * \function utillib_test_predicate_init
 * Initializes a `utillib_test_predicate_t' for `UTILLIB_TEST_ASSERT'
 * or such to use.
 * \param result The result of a boolean expression evaluated when passed in.
 * \param line The line where the predicate took place and self got initialized.
 * \param expr_str The stringized expression.
 * \param severity The severity of the failure.
 */

void utillib_test_predicate_init(utillib_test_predicate_t *self, bool result,
                                 size_t line, char const *expr_str,
                                 int severity) {
  self->result = result;
  self->line = line;
  self->expr_str = expr_str;
  self->severity = severity;
}

/**
 * \function predicate_output
 * Prints a message about the failure of a predicate to stderr.
 */

static void predicate_output(utillib_test_entry_t *self,
                             utillib_test_predicate_t *predicate) {
  fprintf(stderr, "In `%s':%lu:\n", self->func_name, predicate->line);
  fprintf(stderr, "\t%s:`%s'.\n",
          utillib_test_severity_t_tostring(predicate->severity),
          predicate->expr_str);
}

/**
 * \function utillib_test_predicate
 * Tests the result of `predicate' and if that failed, displays debug message
 * and count the failures.
 * \param predicate Provides the boolean expression to test against and
 * debug messages.
 * \return predicate->result.
 */

bool utillib_test_predicate(utillib_test_entry_t *self,
                            utillib_test_predicate_t *predicate) {
  if (predicate->result) {
    return true;
  }
  switch (predicate->severity) {
  case US_EXPECT:
    ++self->expect_failure;
    break;
  case US_ASSERT:
    ++self->assert_failure;
    break;
  case US_ABORT:
    ++self->abort_failure;
    break;
  }
  predicate_output(self, predicate);
  return false;
}

static void status_output(char const *status_str, char const *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  fputs(status_str, stderr);
  fputc(' ', stderr);
  vfprintf(stderr, fmt, ap);
  fputc('\n', stderr);
  va_end(ap);
}

static void case_status_output(char const *status_str,
                               utillib_test_env_t *env) {
  status_output(status_str, "%s.%s",
      env->case_name, env->cur_test->func_name);
}

static void filename_status_output(char const *filename) {
  fprintf(stderr, "file:`%s'\n", filename);
}

/**
 * \function utillib_test_run_test
 * Runs a single test defined by self and records statistics in
 * both self and env.
 * \param self Runs the test on self->func and records any failure.
 * \param env Counts the number of run tests, passed tests and failed tests.
 * \return value of `utillib_test_severity_t'. If the test was passed, returns
 * `US_SUCCESS'; else returns the kind of severity of the failure of the test.
 */

static int utillib_test_run_test(utillib_test_entry_t *self,
                                 utillib_test_env_t *env) {
  ++env->ntests;
  switch (self->status) {
  case UT_STATUS_RUN:
    ++env->nrun;
    /* displays a status bar */
    case_status_output(GREEN_RUN, env);
    self->func(self);
    if (0 ==
        self->abort_failure + self->assert_failure + self->expect_failure) {
      ++env->nsuccess;
      self->succeeded = true;
      /* displays a status bar */
      case_status_output(GREEN_OK, env);
      return US_SUCCESS;
    }
    ++env->nfailure;
    self->succeeded = false;
    /* displays a status bar */
    case_status_output(RED_BAD, env);
    if (self->abort_failure) {
      return US_ABORT;
    }
    return US_ASSERT;
  case UT_STATUS_SKIP:
    case_status_output(WHITE_SKIP, env);
    ++env->nskipped;
    return US_SUCCESS;
  }
}

static void utillib_test_setup(utillib_test_env_t *self) {
  filename_status_output(self->filename);
  status_output(GREEN_BANG, "Test suite `%s' sets up.", self->case_name);
}

static void utillib_test_summary(utillib_test_env_t *self) {
  status_output(GREEN_DASH, "Summary: Total %lu tests, Run %lu, Skipped %lu.",
                self->ntests, self->nrun, self->nskipped);
  status_output(GREEN_DASH, "Summary: Run %lu tests, Passed %lu, Failed %lu.",
                self->nrun, self->nsuccess, self->nfailure);
}

static void utillib_test_teardown(utillib_test_env_t *self) {
  status_output(GREEN_BANG, "Test suite `%s' tears down.", self->case_name);
}

static void utillib_test_abort(utillib_test_env_t *self) {
  status_output(RED_DASH, "Abort at `%s'", self->cur_test->func_name);
}

int utillib_test_run_suite(utillib_test_env_t *self) {
  utillib_test_setup(self);
  for (utillib_test_entry_t *test = self->cases; test->func != NULL; ++test) {
    self->cur_test = test;
    if (US_ABORT == utillib_test_run_test(test, self)) {
      utillib_test_abort(self);
      break;
    }
  }
  utillib_test_summary(self);
  utillib_test_teardown(self);
  return self->nfailure;
}

static void utillib_test_suite_destroy(utillib_test_suite_t *self) {
  utillib_vector_destroy(&self->tests);
}

static void utillib_test_suite_output_xml(utillib_test_suite_t *self) {}

static void utillib_test_suite_output_json(utillib_test_suite_t *self) {}

/**
 * \function utillib_test_suite_init
 * Loads multiple testing environments by calling the function
 * pointers passed in via `...' variadic arguments.
 * \param ... Multiple `utillib_test_getenv_func_t' pointers.
 */

void utillib_test_suite_init(utillib_test_suite_t *self, ...) {
  utillib_vector_init(&self->tests);
  va_list ap;
  va_start(ap, self);
  while (true) {
    utillib_test_getenv_func_t *getenv_func =
        va_arg(ap, utillib_test_getenv_func_t *);
    if (!getenv_func) {
      break;
    }
    utillib_vector_push_back(&self->tests, getenv_func());
  }
}

int utillib_test_suite_run_all(utillib_test_suite_t *self) {
  size_t test_failure = 0;
  size_t test_suites = utillib_vector_size(&self->tests);
  filename_status_output(self->filename);
  status_output(GREEN_BANG, "%lu test suites found...", test_suites);
  status_output(GREEN_BANG, "Global testing environment sets up.");
  fputs("\n", stderr);
  UTILLIB_VECTOR_FOREACH(utillib_test_env_t *, env, &self->tests) {
    test_failure += utillib_test_run_suite(env);
    fputs("\n", stderr);
  }
  utillib_test_suite_destroy(self);
  status_output(GREEN_BANG, "Global testing environment tears down.");
  return test_failure;
}
