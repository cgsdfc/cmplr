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

#include "ll1_sample_1.h"
#include "ll1_sample_1_table.c"

#include <utillib/ll1_builder.h>
#include <utillib/ll1_parser.h>
#include <utillib/scanner.h>
#include <utillib/test.h>

static struct utillib_ll1_factory ll1_factory;
static struct utillib_symbol_scanner symbol_scanner;
static struct ll1_sample_1_semantic semantic;

UTILLIB_TEST_SET_UP() {
  utillib_ll1_factory_init(&ll1_factory,ll1_sample_1_symbols, ll1_sample_1_rules); 
  utillib_ll1_factory_parser_init(&ll1_factory, 
      UT_FIXTURE, 
      &semantic,
      &ll1_sample_1_parser_op);
}

UTILLIB_TEST_TEAR_DOWN() {
  utillib_ll1_parser_destroy(UT_FIXTURE);
  utillib_ll1_factory_destroy(&ll1_factory);
}

UTILLIB_TEST(ll1_parser_parse) {
  utillib_symbol_scanner_init(&symbol_scanner, ll1_sample_1_input_1, ll1_sample_1_symbols);
  utillib_ll1_parser_parse(UT_FIXTURE, &symbol_scanner, &utillib_symbol_scanner_op);
}

/**
 * \test rule_index_load_table
 * Tests if the loaded table (from generated source)
 * is the same with the directly built one.
 * To achieves this, the generated source must be 
 * compiled into this test.
 * Notes since the built one and generated/loaded one
 * share the same rule index, comparing their elements
 * as pointer suffices.
 * 
 * XXX Use feature of build system to do this cleaner
 */
#if 0
UTILLIB_TEST(rule_index_load_table)
{
  struct utillib_vector2 loaded_table;
  utillib_rule_index_load_table(&rule_index, &loaded_table, 
      ll1_parser_table);
  UTILLIB_TEST_ASSERT_EQ(loaded_table.nrow, ll1_table.nrow);
  UTILLIB_TEST_ASSERT_EQ(loaded_table.ncol, ll1_table.ncol);

  for (int i=0; i<ll1_table.nrow; ++i) {
    for (int j=0; j<ll1_table.ncol; ++j) {
      void const * actual=utillib_vector2_at(&loaded_table, i, j);
      void const * expected=utillib_vector2_at(&ll1_table, i , j);
      UTILLIB_TEST_ASSERT_EQ(actual, expected);
    }
  }
}
#endif 

UTILLIB_TEST_DEFINE(Utillib_LL1Parser) {
  UTILLIB_TEST_BEGIN(Utillib_LL1Parser)
  UTILLIB_TEST_RUN(ll1_parser_parse)
  UTILLIB_TEST_END(Utillib_LL1Parser)
  UTILLIB_TEST_FIXTURE(struct utillib_ll1_parser)
  UTILLIB_TEST_RETURN(Utillib_LL1Parser)
}
