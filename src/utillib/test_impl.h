#ifndef UTILLIB_TEST_IMPL_H
#define UTILLIB_TEST_IMPL_H

#include "json.h"
#include "enum.h"
#include "color.h" 
#include "test.h"

/**
 * Colorful status bar for display.
 */

/* Two kinds of status bars with red and green */
static const char *GREEN_BANG = COLOR_STRING_UNBOLD(GREEN, "[==========]");
static const char *GREEN_DASH = COLOR_STRING_UNBOLD(GREEN, "[----------]");
static const char *RED_BANG = COLOR_STRING_UNBOLD(RED, "[==========]");
static const char *RED_DASH = COLOR_STRING_UNBOLD(RED, "[----------]");

/* different kinds of text indicator */
static const char *WHITE_SKIP = "[  SKIPPED ]";
static const char *GREEN_RUN = COLOR_STRING_UNBOLD(GREEN, "[ RUN      ]");
static const char *GREEN_PASS = COLOR_STRING_UNBOLD(GREEN, "[  PASSED  ]");
static const char *GREEN_OK = COLOR_STRING_UNBOLD(GREEN, "[       OK ]");
static const char *RED_BAD = COLOR_STRING_UNBOLD(RED, "[ BAD      ]");
static const char *RED_FAILED = COLOR_STRING_UNBOLD(RED, "[  FAILED  ]");


struct utillib_json_value_t const *
utillib_test_suite_json_object_create(struct utillib_test_suite_t const *self);

#endif /* UTILLIB_TEST_IMPL_H */

