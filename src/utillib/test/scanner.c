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

#include <utillib/scanner.h>
#include <utillib/test.h>
#include "symbol_rule.h"

static struct utillib_symbol_scanner symbol_scanner;

UTILLIB_TEST(symbol_scanner_lookahead) 
{
  utillib_symbol_scanner_init(&symbol_scanner, test_symbols_input, test_symbols);
  for (size_t const *psym=test_symbols_input; *psym!=UT_SYM_NULL; ++psym) {
    UTILLIB_TEST_EXPECT_EQ(*psym, utillib_symbol_scanner_lookahead(&symbol_scanner));
    struct utillib_symbol const * symbol=utillib_symbol_scanner_getsymbol(&symbol_scanner);
    UTILLIB_TEST_EXPECT_EQ(*psym, utillib_symbol_value(symbol));
    utillib_symbol_scanner_shiftaway(&symbol_scanner);
  }
  UTILLIB_TEST_ASSERT(utillib_symbol_scanner_reacheof(&symbol_scanner));
}

UTILLIB_TEST_DEFINE(Utillib_Scanner) 
{
  UTILLIB_TEST_BEGIN(Utillib_Scanner)
    UTILLIB_TEST_RUN(symbol_scanner_lookahead)
  UTILLIB_TEST_END(Utillib_Scanner)

  UTILLIB_TEST_RETURN(Utillib_Scanner)
}


