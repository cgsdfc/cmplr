#include <utillib/test.h>
#include <utillib/strref.h>
/**
 * \file utillib/test/strref.c
 * Test for Strref.
 */

static const char static_long_string[]="A long constant string to test Utillib.Strref";


UTILLIB_TEST(increase_refcnt) {
  struct utillib_strref strref;
  utillib_strref_init(&strref);
  char const * ref1=utillib_strref_incr(&strref, static_long_string);
}

