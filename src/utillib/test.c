#include "test.h"
#include "argp.h" // for argp
#include "color.h"
#include "print.h"
#include <stddef.h> // for size_t
#include <stdio.h>  // for stderr
#define COLOR_STRING(C, S) COLOR_STRING_UNBOLD(C, S)

static const char *GREEN_BANG = COLOR_STRING(GREEN, "[==========]");
static const char *GREEN_DASH = COLOR_STRING(GREEN, "[----------]");
static const char *RED_DASH = COLOR_STRING(RED, "[----------]");
static const char *GREEN_RUN = COLOR_STRING(GREEN, "[ RUN      ]");
static const char *GREEN_OK = COLOR_STRING(GREEN, "[       OK ]");
static const char *RED_SUCK = COLOR_STRING(RED, "[ BAD      ]");
static const char *GREEN_PASS = COLOR_STRING(GREEN, "[  PASSED  ]");
static const char *RED_FAILED = COLOR_STRING(RED, "[  FAILED  ]");

UTILLIB_ARGP_OPTION_BEGIN()
UTILLIB_ARGP_OPTION_ELEM("--include-test", 'I', "FUNC",
                         "each -IFUNC includes a test function to run")
UTILLIB_ARGP_OPTION_ELEM("--exclude-test", 'E', "FUNC",
                         "each -EFUNC excludes a test function")
UTILLIB_ARGP_OPTION_END()

static void utillib_test_env_destroy(utillib_test_env_t *self) {
  utillib_unordered_map_destroy(&self->tst_cases_map);
  utillib_vector_destroy(&self->tst_cases_vec);
}

static void set_mode(utillib_test_env_t *self, char *test_name, int mode) {
  utillib_pair_t *pair =
      utillib_unordered_map_find(&self->tst_cases_map, test_name);
  utillib_test_entry_t *test;
  if (pair == NULL) {
    fprintf(stderr, "%s waring: \"%s\" deos not exist\n", RED_DASH, test_name);
    return;
  }
  test = UTILLIB_PAIR_SECOND(pair);
  test->tst_mode = mode;
}

static utillib_argp_error_t utillib_test_parser(int key, char *arg,
                                                utillib_argp_state *state) {
  utillib_test_env_t *self = UTILLIB_ARGP_STATE_INPUT(state);
  switch (key) {
  case 'I':
    if (self->tst_mode == RUN_EXCL) {
      goto MIXING_IE;
    }
    self->tst_mode = RUN_INCL;
    set_mode(self, arg, RUN_INCL);
    break;
  case 'E':
    if (self->tst_mode == RUN_INCL) {
      goto MIXING_IE;
    }
    self->tst_mode = RUN_EXCL;
    set_mode(self, arg, RUN_EXCL);
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
MIXING_IE:
  utillib_test_env_destroy(self);
  argp_failure(state, key, EXIT_FAILURE, "mixing -E with -I options");
}

static void print_global_setup(utillib_test_env_t *self) {
  fprintf(stderr, "Running %s from %s\n", self->tst_case_name,
          self->tst_filename);
  fprintf(stderr, "%s Global test environment setup.\n", GREEN_BANG);
}

static void report_test_status(utillib_test_env_t *self, char const *bang,
                               utillib_test_entry_t *test) {
  fprintf(stderr, "%s %s.%s\n", bang, self->tst_case_name, test->tst_func_name);
}

static void print_global_teardown(utillib_test_env_t *self) {
  fprintf(stderr, "%s Summary\n", GREEN_DASH);
  UTILLIB_VECTOR_FOREACH(utillib_test_entry_t *, test, &self->tst_cases_vec) {
    if (test->tst_success) {
      report_test_status(self, GREEN_PASS, test);
    } else {
      report_test_status(self, RED_FAILED, test);
    }
  }
  fprintf(stderr, "%s Global test environment teardown.\n", GREEN_BANG);
  fprintf(stderr, "Run %lu tests, pass %lu, failed. %lu\n", self->tst_ntests,
          self->tst_nfailed, self->tst_nsucc);
}

static void run_single_test(utillib_test_env_t *self,
                            utillib_test_entry_t *test) {
  fprintf(stderr, "%s %s.%s\n", GREEN_RUN, self->tst_case_name,
          test->tst_func_name);
  size_t failcnt = self->tst_failcnt;
  test->tst_func(self);
  if (failcnt < self->tst_failcnt) {
    test->tst_success = false;
    ++self->tst_nsucc;
    report_test_status(self, RED_SUCK, test);
  } else {
    test->tst_success = true;
    ++self->tst_nfailed;
    report_test_status(self, GREEN_OK, test);
  }
}

static void run_all_test(utillib_test_env_t *self) {
  int mode = self->tst_mode;
  UTILLIB_VECTOR_FOREACH(utillib_test_entry_t *, test, &self->tst_cases_vec) {
    run_single_test(self, test);
  }
}
static void register_test(utillib_test_env_t *self) {
  utillib_unordered_map_init(&self->tst_cases_map,
                             utillib_unordered_map_const_charp_ft());
  utillib_test_entry_t *p = self->tst_cases;
  for (; p->tst_func != NULL; ++p) {
    utillib_unordered_map_emplace(&self->tst_cases_map, p->tst_func_name, p);
  }
  self->tst_ntests = p - self->tst_cases;
}

void utillib_test_env_init(utillib_test_env_t *self) {
  utillib_vector_init(&self->tst_cases_vec);
  register_test(self);
  utillib_vector_reserve(&self->tst_cases_vec, self->tst_ntests);
  self->tst_mode = RUN_ALL;
}

static void fill_test_torun(utillib_test_env_t *self) {
  for (utillib_test_entry_t *p = self->tst_cases; p->tst_func != NULL; ++p) {
    if (p->tst_mode >= self->tst_mode) {
      utillib_vector_push_back(&self->tst_cases_vec, p);
    }
  }
}

int utillib_test_main(utillib_test_env_t *self, int argc, char **argv) {
  test_argp_parse(self, argc, argv);
  fill_test_torun(self);
  print_global_setup(self);
  run_all_test(self);
  print_global_teardown(self);
  utillib_test_env_destroy(self);
  return 0;
}
