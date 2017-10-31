#include "recursive/error.h"
#include "utillib/to_base.h"

#include <stdio.h>
#include <stdlib.h>

UTILLIB_MAKE_ERROR_TAG_DEFINE(expected_error_tag, ERROR_LV_ERROR,
			      utillib_error_base_destroy_trivial);

utillib_error_base *make_expected_error(int expected, int construct)
{
	UTILLIB_CREATE_BASE(expected_error, err,
			    make_expected_error_tag());
	err->expected = expected;
	err->construct = construct;
	return UTILLIB_TO_BASE(utillib_error_base, err);
}

void expected_error_print(FILE * file, expected_error * e)
{
	fprintf(file, "expected %d %d", e->expected, e->construct);
}

void die(const char *msg)
{
	utillib_die(msg);
}
