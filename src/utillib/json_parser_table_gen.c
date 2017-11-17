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
#include "json_parser_impl.h"
#include "ll1_generator.h"

int main(void) {
  struct utillib_ll1_generator gen;
  utillib_ll1_generator_init_from_code(&gen, utillib_json_symbols,
                                       utillib_json_rules);
  bool good = utillib_ll1_generator_generate(&gen, UTILLIB_SOURCE_DIR
                                             "json_parser_table.c");
  utillib_ll1_generator_destroy(&gen);
  return !good;
}
