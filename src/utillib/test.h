#ifndef UTILLIB_TEST_H
#define UTILLIB_TEST_H
#include "enum.h"
#include "unordered_map.h"
#include <stdbool.h>

/** \file utillib/test.h
 * Light weight test runner.
 * \feature Program options used to control which tests to run
 * and which ones to skip.
 * Currently there are 2 options -- --include-test, --exclude-test.
 * If only -I were specified, utillib_test will only those tests.
 * If only -E were specified, utillib_test will run all the tests
 * **EXCEPT** those being excluded.
 * If both -E and -I were specified, their intersection will be run.
 * If none of these options were given, it runs all the tests.
 */

/* define these to override default setup and teardown behaviour */
#define UTILLIB_TEST_GLOBAL_SETUP() void utillib_test_global_setup()
#define UTILLIB_TEST_GLOBAL_TEARDOWN() void utillib_test_global_teardown()

#define UTILLIB_TEST(NAME) static void NAME(utillib_test_env_t *self)
#define UTILLIB_TEST_BEGIN(NAME)                                               \
  int main(int ARGC, char **ARGV) {                                            \
  static utillib_test_entry_t NAME[] = {
#define UTILLIB_TEST_ELEM(FUNC)                                                \
  {.tst_func = (FUNC), .tst_func_name = #FUNC, .tst_mode = RUN_ALL},
#define UTILLIB_TEST_END(NAME)                                                 \
  UTILLIB_TEST_ELEM(0)                                                         \
  }                                                                            \
  ;                                                                            \
  static utillib_test_env_t static_test_env = {                                \
      .tst_cases = (NAME), .tst_case_name = #NAME, .tst_filename = __FILE__};  \
  utillib_test_env_init(&static_test_env);                                     \
  return utillib_test_main(&static_test_env, ARGC, ARGV);                      \
  }

#define UTILLIB_ASSERT(EXPR)                                                   \
  do {                                                                         \
    bool _result = (EXPR);                                                     \
    if (!_result) {                                                            \
      utillib_assertion_fail(#EXPR, __LINE__, __FILE__);                       \
      return;                                                                  \
    }                                                                          \
    while (0)

/* foward declaration */
typedef struct utillib_test_env_t utillib_test_env_t;
typedef void(utillib_test_func_t)(utillib_test_env_t *);

UTILLIB_ENUM_BEGIN(utillib_test_mode)
UTILLIB_ENUM_ELEM_INIT(RUN_ALL, 0)
UTILLIB_ENUM_ELEM_INIT(RUN_INCL, 1)
UTILLIB_ENUM_ELEM_INIT(RUN_EXCL, -1)
UTILLIB_ENUM_END(utillib_test_mode)

typedef struct utillib_test_entry_t {
  /* function pointer to this test */
  utillib_test_func_t *tst_func;

  /* name of the test function */
  char const *tst_func_name;

  /* passed or failed */
  bool tst_success;

  /* whether it will be run */
  int tst_mode;
} utillib_test_entry_t;

typedef struct utillib_test_env_t {
  /* table that registers all the tests */
  utillib_unordered_map tst_cases_map;

  /* the test to run really */
  utillib_vector tst_cases_vec;

  /* function pointers to tests */
  utillib_test_entry_t *tst_cases;

  /* the size of tst_cases, terminated by NULL */
  size_t tst_ntests;

  /* the name of the file that defines these tests */
  char const *tst_filename;

  /* the name of the array that hold the test function pointers */
  char const *tst_case_name;

  size_t tst_failcnt;

  size_t tst_nfailed;

  size_t tst_nsucc;
  int tst_mode;
} utillib_test_env_t;

int utillib_test_main(utillib_test_env_t *, int, char **);
void utillib_test_env_init(utillib_test_env_t *);
#endif // UTILLIB_TEST_H
