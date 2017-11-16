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

#include <utillib/json_parser.h>
#include <utillib/ll1_generator.h>
#include <utillib/json_parser_impl.h>
#include <utillib/test.h>

static struct utillib_json_parser_factory parser_factory;

UTILLIB_TEST_AUX(json_parser_dump_all)
{
  struct utillib_ll1_generator gen;
  utillib_ll1_generator_init_from_code(&gen, utillib_json_symbols,
      utillib_json_rules);
  utillib_ll1_generator_dump_all(&gen);
  utillib_ll1_generator_destroy(&gen);
}

UTILLIB_TEST_SET_UP() {
  if (!utillib_symbol_check(utillib_json_symbols, utillib_json_symbol_kind_N)) {
    UTILLIB_TEST_ABORT("utillib_json_symbols definition error");
  }
  utillib_json_parser_factory_init(&parser_factory);
}

UTILLIB_TEST_TEAR_DOWN() {
  utillib_json_parser_factory_destroy(&parser_factory);
}

UTILLIB_TEST(json_parser_parse) {
}


UTILLIB_TEST_DEFINE(Utillib_JSON_Parser) {
  UTILLIB_TEST_BEGIN(Utillib_JSON_Parser)
  UTILLIB_TEST_RUN(json_parser_parse)
  UTILLIB_TEST_END(Utillib_JSON_Parser)
  UTILLIB_TEST_FIXTURE(struct utillib_json_parser)
  UTILLIB_TEST_RETURN(Utillib_JSON_Parser)
}
