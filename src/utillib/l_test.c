#include "argp.h"		// for argp
#include "color.h"
#include "print.h"
#include "test.h"
#include <stddef.h>		// for size_t
#include <stdio.h>		// for stderr
#define COLOR_STRING(C, S) COLOR_STRING_UNBOLD(C, S)

UTILLIB_ETAB_BEGIN(utillib_test_status_t)
    UTILLIB_ETAB_ELEM(UT_STATUS_SKIP)
    UTILLIB_ETAB_ELEM(UT_STATUS_RUN)
    UTILLIB_ETAB_END(utillib_test_status_t)

static const char *GREEN_BANG = COLOR_STRING(GREEN, "[==========]");
static const char *GREEN_DASH = COLOR_STRING(GREEN, "[----------]");
static const char *RED_DASH = COLOR_STRING(RED, "[----------]");
static const char *GREEN_RUN = COLOR_STRING(GREEN, "[ RUN      ]");
static const char *GREEN_OK = COLOR_STRING(GREEN, "[       OK ]");
static const char *RED_SUCK = COLOR_STRING(RED, "[ BAD      ]");
static const char *GREEN_PASS = COLOR_STRING(GREEN, "[  PASSED  ]");
static const char *RED_FAILED = COLOR_STRING(RED, "[  FAILED  ]");
__attribute__ ((__weak__))
void utillib_test_global_setup(void)
{
}

__attribute__ ((__weak__))
void utillib_test_global_teardown(void)
{
}

static void utillib_test_env_destroy(utillib_test_env_t * self)
{
	utillib_unordered_map_destroy(&self->cases_map);
	utillib_vector_destroy(&self->cases_vec);
}

static void print_global_setup(utillib_test_env_t * self)
{
	fprintf(stderr, "Running %s from %s\n", self->case_name,
		self->filename);
	fprintf(stderr, "%s Global test environment setup.\n", GREEN_BANG);
	utillib_test_global_setup();
}

static void report_test_status(utillib_test_env_t * self, char const *bang,
			       utillib_test_entry_t * test)
{
	fprintf(stderr, "%s %s.%s\n", bang, self->case_name,
		test->func_name);
}

static void print_summary(utillib_test_env_t * self)
{
	size_t ntests = utillib_vector_size(&self->cases_vec);
	if (!ntests) {
		fprintf(stderr, "%s No test was run.\n", GREEN_DASH);
		return;
	}
	fprintf(stderr, "%s Summary\n", GREEN_DASH);
	UTILLIB_VECTOR_FOREACH(utillib_test_entry_t *, test,
			       &self->cases_vec) {
		if (test->success) {
			report_test_status(self, GREEN_PASS, test);
		} else {
			report_test_status(self, RED_FAILED, test);
		}
	}
	fprintf(stderr, "Run %lu tests, pass %lu, failed %lu.\n", ntests,
		self->nfailed, self->nsucc);
}

static void print_global_teardown(utillib_test_env_t * self)
{
	print_summary(self);
	fprintf(stderr, "%s Global test environment teardown.\n",
		GREEN_BANG);
	utillib_test_global_teardown();
}

static void run_single_test(utillib_test_env_t * self,
			    utillib_test_entry_t * test)
{
	fprintf(stderr, "%s %s.%s\n", GREEN_RUN, self->case_name,
		test->func_name);
	size_t failcnt = self->failcnt;
	test->func(self);
	if (failcnt < self->failcnt) {
		test->success = false;
		++self->nsucc;
		report_test_status(self, RED_SUCK, test);
	} else {
		test->success = true;
		++self->nfailed;
		report_test_status(self, GREEN_OK, test);
	}
}

static void run_all_test(utillib_test_env_t * self)
{
	int mode = self->mode;
	UTILLIB_VECTOR_FOREACH(utillib_test_entry_t *, test,
			       &self->cases_vec) {
		run_single_test(self, test);
	}
}

static void register_test(utillib_test_env_t * self)
{
	utillib_unordered_map_init(&self->cases_map,
				   utillib_unordered_map_const_charp_ft());
	utillib_test_entry_t *p = self->cases;
	for (; p->func != NULL; ++p) {
		utillib_unordered_map_emplace(&self->cases_map,
					      p->func_name, p);
	}
	self->ntests = p - self->cases;
}

void utillib_test_env_init(utillib_test_env_t * self)
{
	register_test(self);
}

static void fill_test_torun(utillib_test_env_t * self)
{
	for (utillib_test_entry_t * p = self->cases; p->func != NULL; ++p) {
		if (p->mode >= 0) {
			utillib_vector_push_back(&self->cases_vec, p);
		}
	}
}

int utillib_test_main(utillib_test_env_t * self)
{
	fill_test_torun(self);
	print_global_setup(self);
	run_all_test(self);
	print_global_teardown(self);
	utillib_test_env_destroy(self);
	return 0;
}

UTILLIB_ETAB_BEGIN(utillib_test_severity_t)
    UTILLIB_ETAB_ELEM_INIT(US_EXPECT, "Expected")
    UTILLIB_ETAB_ELEM_INIT(US_ASSERT, "Assertion Failed")
    UTILLIB_ETAB_ELEM_INIT(US_ABORT, "Abort")
    UTILLIB_ETAB_BEGIN(utillib_test_severity_t)

void utillib_test_predicate_init(utillib_test_predicate_t * self,
				 bool result, size_t line,
				 char const *expr_str, int severity)
{
	self->result = result;
	self->line = line;
	self->expr_str = expr_str;
	self->severity = severity;
}

static void predicate_output(utillib_test_entry_t * entry,
			     utillib_test_predicate_t * predicate)
{
	fprintf(stderr, "In %s:%lu:\n", entry->func_name, predicate->line);
	fprintf(stderr, "\t%s:%s\n",
		utillib_test_severity_t_to_string(predicate->severity),
		predicate->expr_str);
}

bool utillib_test_predicate(utillib_test_entry_t * self,
			    utillib_test_predicate_t * predicate)
{
	if (predicate->result) {
		return true;
	}
	switch (predicate->severity) {
	case US_EXPECT:
		++self->nexpect_failed;
		break;
	case US_ASSERT:
		++self->nassert_failed;
		break;
	case US_ABORT:
		assert(self->nabort == 0
		       && "A test cannot be abort twice");
		self->nabort++;
		break;
	}
	predicate_output(self, predicate);
	return false;
}
