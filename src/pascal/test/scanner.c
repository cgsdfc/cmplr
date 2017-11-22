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

#include "pascal/scanner.h"
#include "pascal/symbols.h"
#include <stdlib.h>         // free
#include <utillib/print.h>  // utillib_static_sprintf
#include <utillib/symbol.h> // UT_SYM_EOF
#include <utillib/test.h>

static struct utillib_string string_as_code;

UTILLIB_TEST_SET_UP() { utillib_string_init(&string_as_code); }
UTILLIB_TEST_TEAR_DOWN() {
  pascal_scanner_destroy(UT_FIXTURE);
  utillib_string_destroy(&string_as_code);
}

UTILLIB_TEST_AUX(scanner_build_keywords_str, struct utillib_string *str) {
  utillib_string_append(str, "    ");
  for (int i = 0; i < pascal_keywords_size; ++i) {
    utillib_string_append(str, pascal_keywords[i].key);
    /* Adds space or newline */
    utillib_string_append(str, i % 2 ? " " : "\n");
  }
}

UTILLIB_TEST(scanner_recognize_keywords) {
  UTILLIB_TEST_AUX_INVOKE(scanner_build_keywords_str, &string_as_code);
  FILE *fp = utillib_string_fmemopen(&string_as_code);
  pascal_scanner_init(UT_FIXTURE, fp);
  for (int i = 0; i < pascal_keywords_size; ++i) {
    struct utillib_keyword_pair const *expected = &pascal_keywords[i];
    size_t actual_value = pascal_scanner_lookahead(UT_FIXTURE);
    UTILLIB_TEST_ASSERT_EQ(actual_value, expected->value);
    pascal_scanner_shiftaway(UT_FIXTURE);
  }
  fclose(fp);
}

UTILLIB_TEST_AUX(scanner_setup_scanner, char const *input, FILE **fp) {
  utillib_string_init_c_str(&string_as_code, input);
  *fp = utillib_string_fmemopen(&string_as_code);
  pascal_scanner_init(UT_FIXTURE, *fp);
}

UTILLIB_TEST_AUX(scanner_recognize_one_kind, size_t token_kind,
                 char const *input) {
  FILE *fp;
  UTILLIB_TEST_AUX_INVOKE(scanner_setup_scanner, input, &fp);
  size_t code;
  while ((code = pascal_scanner_lookahead(UT_FIXTURE)) != UT_SYM_EOF) {
    UTILLIB_TEST_ASSERT_EQ(code, token_kind);
    pascal_scanner_shiftaway(UT_FIXTURE);
  }
  fclose(fp);
}

UTILLIB_TEST(scanner_recognize_iden) {
  UTILLIB_TEST_AUX_INVOKE(
      scanner_recognize_one_kind, SYM_IDEN,
      "IDENTIFIER identifier _cat cat_dog eat_10_apples\n"
      "_ __ ___ ____ _1_2_3_4_ _a_b_c_d_\n"
      "Here is some real life examples\n"
      "But IF we have only identifiers no sentence can be conveyed\n");
}

UTILLIB_TEST(scanner_recognize_uint) {
  UTILLIB_TEST_AUX_INVOKE(scanner_recognize_one_kind, SYM_UINT,
                          "1 2 3 4 5 6 7 8 9 0\n"
                          "00 000 0000 00000 0000000\n"
                          "1024 0124 8921 0012300\n");
}

UTILLIB_TEST_AUX(scanner_recognize_many_kind, size_t const *kinds,
                 char const *input) {
  FILE *fp;
  UTILLIB_TEST_AUX_INVOKE(scanner_setup_scanner, input, &fp);
  for (size_t const *expected_code = kinds; *expected_code != UT_SYM_NULL;
       ++expected_code) {
    size_t actual_code = pascal_scanner_lookahead(UT_FIXTURE);
    UTILLIB_TEST_ASSERT_EQ(actual_code, *expected_code);
    pascal_scanner_shiftaway(UT_FIXTURE);
  }
  fclose(fp);
}

UTILLIB_TEST(scanner_recognize_operator) {
  size_t const op_code[] = {SYM_CEQ,   SYM_LT,  SYM_GT,  SYM_NE,
                            SYM_EQ,    SYM_DIV, SYM_MUL, SYM_ADD,
                            SYM_MINUS, SYM_GE,  SYM_LE,  UT_SYM_NULL};
  char const *input = ":=   <    >  <> \n = / * + - >= <= \n";
  UTILLIB_TEST_AUX_INVOKE(scanner_recognize_many_kind, op_code, input);
}

UTILLIB_TEST(scanner_recognize_delimit) {
  size_t const delimit_code[] = {SYM_LP, SYM_RP, SYM_SEMI, SYM_COMMA,
                                 UT_SYM_NULL};
  char const *input = " ( ) ; , ";
  UTILLIB_TEST_AUX_INVOKE(scanner_recognize_many_kind, delimit_code, input);
}

UTILLIB_TEST(scanner_error_bad_char) {
  char const *input = "# $ % ^ & | \" ! } { ";
  FILE *fp;
  UTILLIB_TEST_AUX_INVOKE(scanner_setup_scanner, input, &fp);
  size_t code = pascal_scanner_lookahead(UT_FIXTURE);
  UTILLIB_TEST_ASSERT_EQ(code, UT_SYM_EOF);
  size_t errnum = pascal_scanner_check(UT_FIXTURE);
  UTILLIB_TEST_ASSERT_GT(errnum, 0);
  fclose(fp);
}

UTILLIB_TEST(scanner_error_bad_ceq) {
  char const *input = "  :: ";
  FILE *fp;
  UTILLIB_TEST_AUX_INVOKE(scanner_setup_scanner, input, &fp);
  size_t code = pascal_scanner_lookahead(UT_FIXTURE);
  UTILLIB_TEST_ASSERT_EQ(code, UT_SYM_EOF);
  size_t errnum = pascal_scanner_check(UT_FIXTURE);
  UTILLIB_TEST_ASSERT_EQ(errnum, 1);
  fclose(fp);
}

UTILLIB_TEST_AUX(scanner_token_pretty_print) {
  void const *value = pascal_scanner_semantic(UT_FIXTURE);
  size_t code = pascal_scanner_lookahead(UT_FIXTURE);
  printf("%15s: %lu: ", pascal_symbol_kind_tostring(code), code);
  switch (code) {
  case SYM_IDEN:
    printf("`%s'\n", (char const *)value);
    free((void *)value);
    break;
  case SYM_UINT:
    printf("`%lu'\n", (size_t)value);
    break;
  default:
    puts("null");
    break;
  }
}

UTILLIB_TEST_AUX(scanner_read_file, char const *full_path) {
  FILE *file = fopen(full_path, "r");
  UTILLIB_TEST_ASSERT(file);
  pascal_scanner_init(UT_FIXTURE, file);
  size_t code;
  while ((code = pascal_scanner_lookahead(UT_FIXTURE)) != UT_SYM_EOF) {
    UTILLIB_TEST_ASSERT_GT(code, 0);
    UTILLIB_TEST_AUX_INVOKE(scanner_token_pretty_print);
    pascal_scanner_shiftaway(UT_FIXTURE);
  }
  fclose(file);
}

UTILLIB_TEST(scanner_read_file_primes) {
  UTILLIB_TEST_AUX_INVOKE(
      scanner_read_file,
      UTILLIB_TEST_PATH_CAT(PASCAL_TEST_DATA_DIR, "primes.pas"));
}

UTILLIB_TEST(scanner_read_file_cock_rabbit) {
  UTILLIB_TEST_AUX_INVOKE(
      scanner_read_file,
      UTILLIB_TEST_PATH_CAT(PASCAL_TEST_DATA_DIR, "cock_rabbit.pas"));
}

UTILLIB_TEST(scanner_read_file_gcd) {
  UTILLIB_TEST_AUX_INVOKE(
      scanner_read_file,
      UTILLIB_TEST_PATH_CAT(PASCAL_TEST_DATA_DIR, "gcd.pas"));
}

UTILLIB_TEST_DEFINE(Pascal_Scanner) {
  UTILLIB_TEST_BEGIN(Pascal_Scanner)
  UTILLIB_TEST_RUN(scanner_recognize_keywords)
  UTILLIB_TEST_RUN(scanner_recognize_iden)
  UTILLIB_TEST_RUN(scanner_recognize_uint)
  UTILLIB_TEST_RUN(scanner_recognize_operator)
  UTILLIB_TEST_RUN(scanner_recognize_delimit)
  UTILLIB_TEST_RUN(scanner_error_bad_char)
  UTILLIB_TEST_RUN(scanner_error_bad_ceq)
  UTILLIB_TEST_RUN(scanner_read_file_primes)
  UTILLIB_TEST_RUN(scanner_read_file_cock_rabbit)
  UTILLIB_TEST_RUN(scanner_read_file_gcd)
  UTILLIB_TEST_END(Pascal_Scanner)
  UTILLIB_TEST_FIXTURE(struct pascal_scanner)
  UTILLIB_TEST_RETURN(Pascal_Scanner)
}
