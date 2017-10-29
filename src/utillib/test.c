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
 * defines a vector. Here is how you can write the tests:
 * <code>
 * // test_vector.c
 * #include "your_headers"
 * #include <utillib/test.h>
 *
 * UTILLIB_TEST(InitialToBeEmpty) {
 *   // get the fixture.
 *   utillib_vector * vector=FIXTURE;
 *   UTILLIB_TEST_ASSERT(0 == utillib_vector_size(vector) &&
 *   utillib_vector_empty(vector));
 * }
 *
 * UTILLIB_TEST(size) {
 *  // use `UT_FIXTURE' to access the fixture.
 *  ...
 * }
 *
 * static void SetUp(utillib_vector *fixture) {
 *   ...
 * }
 *
 * static void TearDown(utillib_vector *fixture) {
 *   ...
 * }
 *
 * UTILLIB_TEST_DEFINE(Utillib_Vector) {
 *    static utillib_vector fixture;
 *    // Registers our tests.
 *    UTILLIB_TEST_BEGIN(Utillib_Vector)
 *      UTILLIB_TEST_RUN(InitialToBeEmpty)
 *      UTILLIB_TEST_RUN(...)
 *      // more tests...
 *    UTILLIB_TEST_END(Utillib_Vector)
 *
 *    // Requires to use fixture.
 *    // You have the freedom to give them
 *    // more descriptive names.
 *    UTILLIB_TEST_FIXTURE(&fixture, SetUp, TearDown);
 *    // Must be the last statement in our definition.
 *    UTILLIB_TEST_RETURN();
 * }
 *
 * int main(void) {
 *  UTILLIB_TEST_RUN_ALL(Utillib_Vector);
 * }
 * </code>
 *
 * That's all. You are now set with your `Utillib_Vector' test and ready
 * to compile and run it. And some possible output may be:
 *
 * file:`test_vector.c'
 * [==========] 1 test suites found.
 * [==========] Global testing environment sets up.
 *
 * file:`test_vector.c'
 * [==========] Test suite `Utillib_Vector' sets up.
 * [  SKIPPED ] vector_init (0s).
 * [ RUN      ] push_back (0s).
 * [       OK ] push_back (0s).
 * [----------] Summary: Total 2 tests, Run 1, Skipped 1.
 * [----------] Summary: Run 1 tests, Passed 1, Failed 0.
 * [==========] Test suite `Utillib_Vector' tears down.
 *
 * [==========] Global testing environment tears down.
 */

#include "test.h"
#include "color.h"  // for COLOR_STRING_UNBOLD
#include <stdarg.h> // for va_list
#include <time.h>   // for time
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
  fprintf(stderr, "\t%s: `%s'.\n",
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

/**
 * \function status_output
 * Prints one line to stderr with a status bar.
 */

static void status_output(char const *status_str, char const *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  fputs(status_str, stderr);
  fputc(' ', stderr);
  vfprintf(stderr, fmt, ap);
  fputc('\n', stderr);
  va_end(ap);
}

/**
 * \function case_status_output
 * Prints the name of the test with a status bar.
 */
static void case_status_output(char const *status_str,
                               utillib_test_entry_t *entry) {
  status_output(status_str, "%s (%lds).", entry->func_name, entry->duration);
}

/**
 * \function filename_status_output
 * Informs the user with a filename.
 */
static void filename_status_output(char const *filename) {
  fprintf(stderr, "file:`%s'\n", filename);
}

/**
 * \function utillib_test_case
 * Invokes the function pointer in self, possibily after
 * calling its setup and does teardown after it.
 */
static void utillib_test_case(utillib_test_entry_t *self,
                              utillib_test_env_t *env) {
  time_t begin, end;
  if (env->setup_func) {
    env->setup_func(env->fixture);
  }
  time(&begin);
  self->func(self, env->fixture);
  time(&end);
  self->duration = end - begin;
  if (env->teardown_func) {
    env->teardown_func(env->fixture);
  }
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
  switch (self->status) {
  case UT_STATUS_RUN:
    /* displays a status bar */
    case_status_output(GREEN_RUN, self);
    utillib_test_case(self, env);
    ++env->nrun;
    ++env->ntests;
    env->total_duration += self->duration;
    if (0 ==
        self->abort_failure + self->assert_failure + self->expect_failure) {
      ++env->nsuccess;
      self->succeeded = true;
      /* displays a status bar */
      case_status_output(GREEN_OK, self);
      return US_SUCCESS;
    }
    ++env->nfailure;
    self->succeeded = false;
    /* displays a status bar */
    case_status_output(RED_BAD, self);
    if (self->abort_failure) {
      return US_ABORT;
    }
    return US_ASSERT;
  case UT_STATUS_SKIP:
    case_status_output(WHITE_SKIP, self);
    ++env->nskipped;
    ++env->ntests;
    return US_SUCCESS;
  }
}

/**
 * \function utillib_test_setup
 * Starts running a test suite by printing its name and
 * the file where it was defined.
 */
static void utillib_test_setup(utillib_test_env_t *self) {
  filename_status_output(self->filename);
  status_output(GREEN_BANG, "Test suite `%s' sets up.", self->case_name);
}

/**
 * \function utillib_test_summary
 * Displays statistics of a test suite after its execution reached end.
 */
static void utillib_test_summary(utillib_test_env_t *self) {
  status_output(GREEN_DASH, "Summary: Total %lu tests, Run %lu, Skipped %lu.",
                self->ntests, self->nrun, self->nskipped);
  status_output(GREEN_DASH, "Summary: Run %lu tests, Passed %lu, Failed %lu.",
                self->nrun, self->nsuccess, self->nfailure);
}

/**
 * \function utillib_test_teardown
 * Informs the user that all the test suites were run.
 */
static void utillib_test_teardown(utillib_test_env_t *self) {
  status_output(GREEN_BANG, "Test suite `%s' tears down.", self->case_name);
}

/**
 * \function utillib_test_run_suite
 * Controls the whole process of a test suite:
 * runs all its tests and stops at the first abort.
 * Returns the number of failed tests.
 */
int utillib_test_run_suite(utillib_test_env_t *self) {
  utillib_test_setup(self);
  for (utillib_test_entry_t *test = self->cases; test->func != NULL; ++test) {
    if (US_ABORT == utillib_test_run_test(test, self)) {
      break;
    }
  }
  utillib_test_summary(self);
  utillib_test_teardown(self);
  return self->nfailure;
}

/**
 * \function utillib_test_suite_destroy
 * Destructor of `utillib_test_suite_t'.
 */
static void utillib_test_suite_destroy(utillib_test_suite_t *self) {
  utillib_vector_destroy(&self->tests);
  if (self->xml_output) {
    fclose(self->xml_output);
  }
  if (self->json_output) {
    fclose(self->json_output);
  }
}

/**
 * \function utillib_test_suite_init
 * Loads multiple testing environments by calling the function
 * pointers passed in via `...' variadic arguments.
 * \param ... Multiple `utillib_test_getenv_func_t' pointers.
 */

void utillib_test_suite_init(utillib_test_suite_t *self, ...) {
  static const size_t init_capacity = 8;
  utillib_vector_init(&self->tests);
  utillib_vector_reserve(&self->tests, init_capacity);
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

/**
 * \function utillib_test_suite_run_all
 * Runs all the tests and Returns the number of failed tests.
 */
int utillib_test_suite_run_all(utillib_test_suite_t *self) {
  size_t test_failure = 0;
  size_t test_suites = utillib_vector_size(&self->tests);
  filename_status_output(self->filename);
  status_output(GREEN_BANG, "%lu test suites found.", test_suites);
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

/**
 * \function utillib_test_env_set_fixture
 * Sets fixture, setup and teardown for the test suite.
 * Do not call it directly or more than once.
 * Call it via `UTILLIB_TEST_FIXTURE'.
 */
void utillib_test_env_set_fixture(utillib_test_env_t *self,
                                  utillib_test_fixture_t fixture,
                                  utillib_test_fixfunc_t *setup,
                                  utillib_test_fixfunc_t *teardown) {
  self->fixture = fixture;
  self->setup_func = setup;
  self->teardown_func = teardown;
}

/**
 * \function utillib_test_dummy
 * Returns a pointer to a dummy object as a place holder for testing.
 */
utillib_test_dummy_t *utillib_test_dummy(void) {
  static utillib_test_dummy_t static_dummy;
  return &static_dummy;
}

/**
 * \function utillib_test_message
 * Prints a message to notify the user.
 */
void utillib_test_message(char const *func_name, size_t line, char const *fmt,
                          ...) {
  fprintf(stderr, "In `%s':%lu:\n", func_name, line);
  va_list ap;
  va_start(ap, fmt);
  fputs("\tMessage: ", stderr);
  vfprintf(stderr, fmt, ap);
  fputs(".\n", stderr);
  va_end(ap);
}

/* static void utillib_test_suite_output_xml(utillib_test_suite_t *self) { */
/*   fprintf(self->xml_output, */
/*           "<-- XML generated by Utillib.Test. Version %s -->\n", */
/*           UTILLIB_TEST_VERSION_STRING); */
/* } */

/* static char const *utillib_test_entry_tostring(utillib_test_entry_t *self) {
 */
/*   return utillib_static_sprintf("%s:%s", self->func_name, */
/*                                 self->succeeded ? "PASSED" : "FAILED"); */
/* } */

/* static void utillib_test_env_tostring(utillib_test_env_t *self) { */
/*   return utillib_static_sprintf("%s:%s %s:%s", "file", self->filename, */
/* } */

/* static void test_suite_object_print(utillib_test_suite_t *self) {} */

/* static void utillib_test_suite_output_json(utillib_test_suite_t *self) { */
/*   utillib_printer json; */
/*   utillib_printer_init(&json, self->json_output); */
/*   utillib_print_json_object(&json, test_suite_object_print, self); */
/* } */
