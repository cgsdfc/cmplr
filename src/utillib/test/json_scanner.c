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

#include <utillib/json_scanner.h>
#include <utillib/test.h>

UTILLIB_TEST_SET_UP() {}

UTILLIB_TEST_TEAR_DOWN() { utillib_json_scanner_destroy(UT_FIXTURE); }

UTILLIB_TEST_AUX(json_scanner_recognize, size_t const *kinds,
                 char const *input) 
{
  utillib_json_scanner_init(UT_FIXTURE, input);
  for (size_t const *expected_code = kinds; *expected_code != UT_SYM_EOF;
       ++expected_code) {
    size_t actual_code = utillib_json_scanner_lookahead(UT_FIXTURE);
    UTILLIB_TEST_ASSERT_EQ(actual_code, *expected_code);
    struct utillib_json_value *value=utillib_json_scanner_semantic(UT_FIXTURE);
    if (value) {
      utillib_json_pretty_print(value, stderr);
      utillib_json_value_destroy(value);
    }
    utillib_json_scanner_shiftaway(UT_FIXTURE);
  }
}

UTILLIB_TEST(json_scanner_keywords) 
{
  char const * keywords="null true false false true true";
  size_t const expected[]={
    JSON_SYM_NULL, JSON_SYM_TRUE, JSON_SYM_FALSE, 
    JSON_SYM_FALSE, JSON_SYM_TRUE, JSON_SYM_TRUE,
    UT_SYM_EOF
  };
  UTILLIB_TEST_AUX_INVOKE(json_scanner_recognize, expected, keywords);
}

UTILLIB_TEST(json_scanner_punctuation)
{
  char const * input="\n \n { [ , : ] }  ";
  size_t const expected[]={
    JSON_SYM_LB, JSON_SYM_LK, JSON_SYM_COMMA,
    JSON_SYM_COLON, JSON_SYM_RK, JSON_SYM_RB,
    UT_SYM_EOF
  };
  UTILLIB_TEST_AUX_INVOKE(json_scanner_recognize, expected, input);
}

UTILLIB_TEST(json_scanner_number)
{
  char const * integer=" 1234 ";

  size_t const expected_long[]={
    JSON_SYM_LONG,
    UT_SYM_EOF
  };

  size_t const expected_real[]={
    JSON_SYM_REAL,
    UT_SYM_EOF
  };

  UTILLIB_TEST_AUX_INVOKE(json_scanner_recognize, expected_long, integer);

  char const * int_frac=" 1.2333 ";
  UTILLIB_TEST_AUX_INVOKE(json_scanner_recognize, expected_real, int_frac);

  char const * exp_only = " 232354e10 ";
  UTILLIB_TEST_AUX_INVOKE(json_scanner_recognize, expected_real, exp_only);

  char const * int_frac_exp = " 3223.333e4 ";
  UTILLIB_TEST_AUX_INVOKE(json_scanner_recognize, expected_real, int_frac_exp);

  char const * singed_int = " -1234 ";
  UTILLIB_TEST_AUX_INVOKE(json_scanner_recognize, expected_long, singed_int);

  char const * sign_real = " -134.13421";
  UTILLIB_TEST_AUX_INVOKE(json_scanner_recognize, expected_real, sign_real);

  char const * sign_expt_real = " 1234e-3" ;
  UTILLIB_TEST_AUX_INVOKE(json_scanner_recognize, expected_real, sign_expt_real);

  char const * sign_expt_real_frac = " 1234.134e-3" ;
  UTILLIB_TEST_AUX_INVOKE(json_scanner_recognize, expected_real, sign_expt_real_frac);

}

UTILLIB_TEST(json_scanner_string) 
{
  size_t const expected_escaped[]={
    JSON_SYM_STR,
    UT_SYM_EOF
  };
  char const * escaped=" \"\\\" \\/ \\b \\f \\n \\r \\t \" ";
  UTILLIB_TEST_AUX_INVOKE(json_scanner_recognize, expected_escaped, escaped);

  char const * strings="\"He\" \"is\" \"screwn\"";
  size_t const expected[]={
    JSON_SYM_STR, JSON_SYM_STR, JSON_SYM_STR,
    UT_SYM_EOF
  };
  UTILLIB_TEST_AUX_INVOKE(json_scanner_recognize, expected, strings);
  char const * empty="  \"\"  ";
  size_t const expected_empty[]= {
    JSON_SYM_STR, 
    UT_SYM_EOF
  };
  UTILLIB_TEST_AUX_INVOKE(json_scanner_recognize, expected_empty, empty);

}

UTILLIB_TEST_DEFINE(Utillib_JSON_Scanner) {
  UTILLIB_TEST_BEGIN(Utillib_JSON_Scanner)
  UTILLIB_TEST_RUN(json_scanner_string)
  UTILLIB_TEST_RUN(json_scanner_number)
  UTILLIB_TEST_RUN(json_scanner_keywords)
  UTILLIB_TEST_RUN(json_scanner_punctuation)
  UTILLIB_TEST_END(Utillib_JSON_Scanner)
  UTILLIB_TEST_FIXTURE(struct utillib_json_scanner)
  UTILLIB_TEST_RETURN(Utillib_JSON_Scanner)
}

