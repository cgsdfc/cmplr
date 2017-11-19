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

#include "test.h"
#include "test_impl.h"
#include "flags.h"
#include "print.h"
#include <stdarg.h> // for va_list
#include <string.h>
#include <time.h> // for time

/**
 * Argp options.
 */
UTILLIB_ARGP_OPTION_BEGIN()
UTILLIB_ARGP_OPTION_ELEM("json", 'j', "FILE",
                         "Writes the test result to a JSON file.")
UTILLIB_ARGP_OPTION_END()

UTILLIB_ARGP_OPTION_REGISTER(NULL, "Utillib.Test Runner")

static utillib_argp_error_t
UTILLIB_ARGP_STATIC_PARSER(int key, char *text, utillib_argp_state *state) {
  struct utillib_test_suite_t *self = UTILLIB_ARGP_STATE_INPUT(state);
  switch (key) {
  case 'j':
    self->json_output = text;
    return 0;
  default:
    return ARGP_ERR_UNKNOWN;
  }
}


/**
 * \function utillib_test_predicate_init
 * Initializes a `struct utillib_test_predicate_t' for `UTILLIB_TEST_ASSERT'
 * or such to use.
 * \param result The result of a boolean expression evaluated when passed in.
 * \param line The line where the predicate took place and self got initialized.
 * \param expr_str The stringized expression.
 * \param severity The severity of the failure.
 */
void utillib_test_predicate_init(struct utillib_test_predicate_t *self,
                                 bool result, size_t line, char const *expr_str,
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

static void predicate_output(struct utillib_test_entry_t *self,
                             struct utillib_test_predicate_t *predicate) {
  fprintf(stderr, "In `%s':%lu:\n", self->func_name, predicate->line);
  fprintf(stderr, "\t%s: `%s'.\n",
          utillib_test_severity_kind_tostring(predicate->severity),
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

bool utillib_test_predicate(struct utillib_test_entry_t *self,
                            struct utillib_test_predicate_t *predicate) {
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
                               struct utillib_test_entry_t *entry) {
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
static void utillib_test_case(struct utillib_test_entry_t *self,
                              struct utillib_test_env_t *env) {
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

static int utillib_test_run_test(struct utillib_test_entry_t *self,
                                 struct utillib_test_env_t *env) {
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
static void utillib_test_setup(struct utillib_test_env_t *self) {
  filename_status_output(self->filename);
  status_output(GREEN_BANG, "Test suite `%s' sets up.", self->case_name);
}

static void utillib_test_report_failure(struct utillib_test_env_t *self) {
  for (struct utillib_test_entry_t const *test = self->cases;
       test->func != NULL; ++test) {
    if (test->status != UT_STATUS_RUN)
      continue;
    if (test->succeeded)
      continue;
    status_output(RED_DASH, "Failed `%s'", test->func_name);
  }
}

/**
 * \function utillib_test_summary
 * Displays statistics of a test suite after its execution reached end.
 */
static void utillib_test_summary(struct utillib_test_env_t *self) {
  status_output(GREEN_BANG, "Test suite `%s' tears down.", self->case_name);
  utillib_test_report_failure(self);
  status_output(GREEN_DASH, "Summary: Total %lu tests, Run %lu, Skipped %lu.",
                self->ntests, self->nrun, self->nskipped);
  status_output(GREEN_DASH, "Summary: Run %lu tests, Passed %lu, Failed %lu.",
                self->nrun, self->nsuccess, self->nfailure);
}

/**
 * \function utillib_test_run_suite
 * Controls the whole process of a test suite:
 * runs all its tests and stops at the first abort.
 * Returns the number of failed tests.
 */
int utillib_test_run_suite(struct utillib_test_env_t *self) {
  utillib_test_setup(self);
  for (struct utillib_test_entry_t *test = self->cases; test->func != NULL;
       ++test) {
    if (US_ABORT == utillib_test_run_test(test, self)) {
      break;
    }
  }
  utillib_test_summary(self);
  return self->nfailure;
}

/**
 * \function utillib_test_suite_destroy
 * Destructor of `struct utillib_test_suite_t'.
 */
void utillib_test_suite_destroy(struct utillib_test_suite_t *self) {
  utillib_vector_destroy(&self->tests);
}

/**
 * \function utillib_test_suite_init
 * Loads multiple testing environments by calling the function
 * pointers passed in via `...' variadic arguments.
 * \param ... Multiple `utillib_test_getenv_func_t' pointers.
 */

void utillib_test_suite_init(struct utillib_test_suite_t *self, ...) {
  static const size_t init_capacity = 8;
  utillib_vector_init(&self->tests);
  utillib_vector_reserve(&self->tests, init_capacity);
  va_list ap;
  va_start(ap, self);
  while (true) {
    struct utillib_test_env_t *(*getenv_func)(void) = va_arg(ap, void *);
    if (!getenv_func) {
      break;
    }
    utillib_vector_push_back(&self->tests, getenv_func());
  }
}

/**
 * \function utillib_test_env_set_fixture
 * Sets fixture, setup and teardown for the test suite.
 * Do not call it directly or more than once.
 * Call it via `UTILLIB_TEST_FIXTURE'.
 */
void utillib_test_env_set_fixture(struct utillib_test_env_t *self,
                                  utillib_test_fixture_t fixture,
                                  void (*setup)(void *),
                                  void (*teardown)(void *)) {
  self->fixture = fixture;
  self->setup_func = setup;
  self->teardown_func = teardown;
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


/**
 * \function test_suite_print_json
 * Implements output to JSON format.
 */
static void test_suite_print_json(struct utillib_test_suite_t *self) {
  if (!self->json_output) {
    /* no output requirement */
    return;
  }
  FILE *file = fopen(self->json_output, "w");
  if (!file) {
    status_output(RED_DASH, "warning: `%s': %s", strerror(errno),
                  self->json_output);
    return;
  }
  struct utillib_json_value *val = utillib_test_suite_json_object_create(self);
  utillib_json_pretty_print(val, file);
  utillib_json_value_destroy(val);
  fclose(file);
}

static void
utillib_test_suite_report_failure(struct utillib_test_suite_t *self) {
  UTILLIB_VECTOR_FOREACH(struct utillib_test_env_t *, env, &self->tests) {
    utillib_test_report_failure(env);
  }
}

/**
 * \function utillib_test_suite_run_all
 * Runs all the tests and Returns the number of failed tests.
 */
int utillib_test_suite_run_all(struct utillib_test_suite_t *self, int argc,
                               char **argv) {
  UTILLIB_ARGP_PARSE(argc, argv, self);
  size_t test_failure = 0;
  size_t test_suites = utillib_vector_size(&self->tests);
  filename_status_output(self->filename);
  status_output(GREEN_BANG, "%lu test suites found.", test_suites);
  status_output(GREEN_BANG, "Global testing environment sets up.");
  fputs("\n", stderr);
  UTILLIB_VECTOR_FOREACH(struct utillib_test_env_t *, env, &self->tests) {
    test_failure += utillib_test_run_suite(env);
    fputs("\n", stderr);
  }
  status_output(GREEN_BANG, "Global testing environment tears down.");
  utillib_test_suite_report_failure(self);
  test_suite_print_json(self);
  return test_failure;
}
