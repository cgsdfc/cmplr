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

UTILLIB_TEST_DECLARE(Utillib_Test);
UTILLIB_TEST_DECLARE(Utillib_Vector);
UTILLIB_TEST_DECLARE(Utillib_Slist);
UTILLIB_TEST_DECLARE(Utillib_String);
UTILLIB_TEST_DECLARE(Utillib_JSON);
UTILLIB_TEST_DECLARE(Utillib_Rule);
UTILLIB_TEST_DECLARE(Utillib_Symbol);
UTILLIB_TEST_DECLARE(Utillib_Bitset);
UTILLIB_TEST_DECLARE(Utillib_Print);
UTILLIB_TEST_DECLARE(Utillib_LL1Parser);
UTILLIB_TEST_DECLARE(Utillib_LL1Builder);
UTILLIB_TEST_DECLARE(Utillib_Scanner);
UTILLIB_TEST_DECLARE(Utillib_Vector2);
UTILLIB_TEST_DECLARE(Utillib_LL1Generator);
UTILLIB_TEST_DECLARE(Utillib_JSON_Parser);
UTILLIB_TEST_DECLARE(Utillib_JSON_Scanner);
UTILLIB_TEST_DECLARE(Utillib_HashMap);
UTILLIB_TEST_DECLARE(Utillib_Graph);

int main(int argc, char **argv) {
  UTILLIB_TEST_RUN_ALL(argc, argv, Utillib_HashMap);
}
