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
#include <utillib/test.h>
#include "cling/symbol_table.h"

UTILLIB_TEST_SET_UP(){
  cling_symbol_table_init(UT_FIXTURE);
}
UTILLIB_TEST_TEAR_DOWN() {
  cling_symbol_table_destroy(UT_FIXTURE);
}

UTILLIB_TEST(symbol_table_insert) {
  cling_symbol_table_insert(UT_FIXTURE, CL_INT|CL_CONST,
      "x", utillib_json_null_create());
  struct utillib_json_value *val=cling_symbol_table_json_object_create(UT_FIXTURE);
  utillib_json_pretty_print(val, stderr);
  
}

UTILLIB_TEST_DEFINE(Cling_SymbolTable) {
  UTILLIB_TEST_BEGIN(Cling_SymbolTable)
  UTILLIB_TEST_RUN(symbol_table_insert)
  UTILLIB_TEST_END(Cling_SymbolTable)
  UTILLIB_TEST_FIXTURE(struct cling_symbol_table)
  UTILLIB_TEST_RETURN(Cling_SymbolTable)
}

