#ifndef RECURSIVE_ERROR_H
#define RECURSIVE_ERROR_H
#include <stddef.h>
#include "utillib/error.h"

typedef struct expected_error
{
  utillib_error_base base;
  int expected;
  int construct;
} expected_error;

UTILLIB_MAKE_ERROR_TAG_DECLARE (expected_error_tag);
utillib_error_base *make_expected_error (int, int);
void die (const char *);
#endif // RECURSIVE_ERROR_H
