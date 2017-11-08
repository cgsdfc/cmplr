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
#include <scanner/match.h>
#include <utillib/test.h>
SCANNER_MATCH_ENTRY_BEGIN(main_match)
SCANNER_MATCH_ENTRY(scanner_match_identifier, 1)
SCANNER_MATCH_ENTRY(scanner_match_string_double, 2)
SCANNER_MATCH_ENTRY(scanner_match_any_char, 0)
SCANNER_MATCH_ENTRY_END(main_match)

UTILLIB_TEST(Match) {
  char const *filename = "./t.c";
  FILE *file = fopen(filename, "r");
  scanner_t scan;
  scanner_init(&scan, file, filename, main_match);
  int r = scanner_read_all(SCANNER_TO_BASE(&scan), stdout);
  scanner_destroy(&scan);
}
