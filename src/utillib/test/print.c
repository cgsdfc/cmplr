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
#include <utillib/print.h>
#include <utillib/test.h>

UTILLIB_TEST(unbalanced_json) {

  char const * unbalanced="[[[XXX]]";
  utillib_printer_t printer;
  utillib_printer_init(&printer, stdout, 4);
  utillib_printer_print_json(&printer, unbalanced);
}


UTILLIB_TEST_DEFINE(Utillib_Print) {
  UTILLIB_TEST_BEGIN(Utillib_Print)
    UTILLIB_TEST_RUN(unbalanced_json)
  UTILLIB_TEST_END(Utillib_Print)
  UTILLIB_TEST_RETURN(Utillib_Print)
}
