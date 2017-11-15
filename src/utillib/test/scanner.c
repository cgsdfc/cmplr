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

#include "symbol_rule.h"
#include <utillib/scanner.h>
#include <utillib/test.h>

static struct utillib_symbol_scanner symbol_scanner;
enum {

  SYM_KW_AUTO,
  SYM_KW_BREAK,
  SYM_KW_CASE,
  SYM_KW_CHAR,
  SYM_KW_CONST,
  SYM_KW_CONTINUE,
  SYM_KW_DEFAULT,
  SYM_KW_UNSIGNED,
  SYM_KW_VOID,
  SYM_KW_VOLATILE,
  SYM_KW_WHILE,

};

/* Copied from K&R book */
static const struct utillib_keyword_pair keywords[] = {
  {"auto",    SYM_KW_AUTO} ,    {"break",    SYM_KW_BREAK} ,
  { "case",    SYM_KW_CASE} ,   {"char",     SYM_KW_CHAR} ,
  { "const",   SYM_KW_CONST} ,  {"continue", SYM_KW_CONTINUE} ,
  { "default", SYM_KW_DEFAULT} ,{"unsigned", SYM_KW_UNSIGNED} ,
  { "void",    SYM_KW_VOID} ,   {"volatile", SYM_KW_VOLATILE} ,
  { "while",   SYM_KW_WHILE} ,
};

static const size_t keywords_size = sizeof keywords / sizeof keywords[0];

UTILLIB_TEST(symbol_scanner_lookahead) {
  utillib_symbol_scanner_init(&symbol_scanner, test_symbols_input,
                              test_symbols);
  for (size_t const *psym = test_symbols_input; *psym != UT_SYM_NULL; ++psym) {
    UTILLIB_TEST_EXPECT_EQ(*psym,
                           utillib_symbol_scanner_lookahead(&symbol_scanner));
    struct utillib_symbol const *symbol =
        utillib_symbol_scanner_semantic(&symbol_scanner);
    UTILLIB_TEST_EXPECT_EQ(*psym, utillib_symbol_value(symbol));
    utillib_symbol_scanner_shiftaway(&symbol_scanner);
  }
  UTILLIB_TEST_ASSERT(utillib_symbol_scanner_reacheof(&symbol_scanner));
}

UTILLIB_TEST(keyword_bearch) {
  for (int i = 0; i < keywords_size; ++i) {
    char const *key = keywords[i].key;
    size_t code = utillib_keyword_bsearch(key, keywords, keywords_size);
    UTILLIB_TEST_EXPECT_EQ(code, keywords[i].value);
  }

  for (int i = keywords_size - 1; i >= 0; --i) {
    char const *key = keywords[i].key;
    size_t code = utillib_keyword_bsearch(key, keywords, keywords_size);
    UTILLIB_TEST_EXPECT_EQ(code, keywords[i].value);
  }

  size_t code = utillib_keyword_bsearch("banana", keywords, keywords_size);
  UTILLIB_TEST_ASSERT(code == UT_SYM_NULL);
}

UTILLIB_TEST_DEFINE(Utillib_Scanner) {
  UTILLIB_TEST_BEGIN(Utillib_Scanner)
  UTILLIB_TEST_RUN(symbol_scanner_lookahead)
  UTILLIB_TEST_RUN(keyword_bearch)
  UTILLIB_TEST_END(Utillib_Scanner)
  UTILLIB_TEST_RETURN(Utillib_Scanner)
}
