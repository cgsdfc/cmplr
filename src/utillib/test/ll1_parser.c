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
#include <utillib/ll1_parser.h>
#include <utillib/ll1_builder.h>
#include <utillib/scanner.h>
#include <utillib/test.h>

static size_t INPUT_SELECT;
static struct utillib_rule_index rule_index;
static struct utillib_vector2 ll1_table;
static struct utillib_ll1_builder ll1_builder;
static struct utillib_symbol_scanner symbol_scanner;

UTILLIB_TEST_SET_UP() {
  switch (INPUT_SELECT) {
    case UT_LL1_INPUT0:
      utillib_rule_index_init(&rule_index, test_symbols, test_rules);
      break;
  }
  utillib_ll1_builder_init(&ll1_builder, &rule_index);
  utillib_ll1_builder_build_table(&ll1_builder, &ll1_table);
  utillib_ll1_parser_init(UT_FIXTURE, &rule_index, &ll1_table,
      NULL, NULL);
}

UTILLIB_TEST_TEAR_DOWN() {
  utillib_rule_index_destroy(&rule_index);
  utillib_ll1_builder_destroy(&ll1_builder);
  utillib_vector2_destroy(&ll1_table);
  utillib_ll1_parser_destroy(UT_FIXTURE);
}

UTILLIB_TEST(ll1_parser_parse) {
  utillib_symbol_scanner_init(&symbol_scanner, test_symbols_input, test_symbols);
  int result=utillib_ll1_parser_parse(UT_FIXTURE, &symbol_scanner, &utillib_symbol_scanner_op);
  UTILLIB_TEST_EXPECT_EQ(result, UT_LL1_PARSER_OK);
}

UTILLIB_TEST_DEFINE(Utillib_LL1Parser) 
{
  UTILLIB_TEST_BEGIN(Utillib_LL1Parser)
    UTILLIB_TEST_RUN(ll1_parser_parse)
  UTILLIB_TEST_END(Utillib_LL1Parser)
  UTILLIB_TEST_FIXTURE(struct utillib_ll1_parser)
  UTILLIB_TEST_RETURN(Utillib_LL1Parser)
}
