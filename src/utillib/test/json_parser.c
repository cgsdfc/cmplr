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
#include <utillib/json_parser_impl.h>
#include <utillib/ll1_generator.h>
#include <utillib/test.h>

/*
 * This file is out of date!
 * Now json_parser needs **string**
 * as input rather than utillib_symbol_scanner!
 */

static struct utillib_ll1_factory 
parser_factory;

UTILLIB_TEST_AUX(json_parser_dump_all) {
  struct utillib_ll1_generator gen;
  utillib_ll1_generator_init_from_code(&gen, utillib_json_symbols,
                                       utillib_json_rules);
  utillib_ll1_generator_dump_all(&gen);
  utillib_ll1_generator_destroy(&gen);
}

UTILLIB_TEST_SET_UP() {
  utillib_json_parser_factory_init(&parser_factory);
  utillib_json_parser_init(UT_FIXTURE, &parser_factory);
}

UTILLIB_TEST_TEAR_DOWN() {
  utillib_ll1_factory_destroy(&parser_factory);
  utillib_ll1_parser_destroy(UT_FIXTURE);
}

UTILLIB_TEST(json_parser_parse_null) {
  size_t const input[] = {JSON_SYM_NULL, UT_SYM_EOF};
  UTILLIB_TEST_ASSERT(utillib_json_parser_parse_dbg(UT_FIXTURE, input));
}

UTILLIB_TEST(json_parser_parse_str) {
  size_t const input[] = {JSON_SYM_STR, UT_SYM_EOF};
  UTILLIB_TEST_ASSERT(utillib_json_parser_parse_dbg(UT_FIXTURE, input));
}

UTILLIB_TEST(json_parser_parse_number) {
  size_t const input[] = {JSON_SYM_REAL, UT_SYM_EOF};
  UTILLIB_TEST_ASSERT(utillib_json_parser_parse_dbg(UT_FIXTURE, input));
}

UTILLIB_TEST(json_parser_parse_bool) {
  size_t const input_1[] = {JSON_SYM_TRUE, UT_SYM_EOF};
  UTILLIB_TEST_ASSERT(utillib_json_parser_parse_dbg(UT_FIXTURE, input_1));
}

UTILLIB_TEST(json_parser_parse_array) {
  /*
   * [ true, "string", null ]
   */
  size_t const input[] = {JSON_SYM_LK,  JSON_SYM_TRUE,  JSON_SYM_COMMA,
                          JSON_SYM_STR, JSON_SYM_COMMA, JSON_SYM_NULL,
                          JSON_SYM_RK,  UT_SYM_EOF};
  UTILLIB_TEST_ASSERT(utillib_json_parser_parse_dbg(UT_FIXTURE, input));
}

/* Empty array is a special case we are interested in */

UTILLIB_TEST(json_parser_empty_array) {
  size_t const input[] = {JSON_SYM_LK, JSON_SYM_RK, UT_SYM_EOF};
  UTILLIB_TEST_ASSERT(utillib_json_parser_parse_dbg(UT_FIXTURE, input));
}

UTILLIB_TEST(json_parser_parse_object) {
  size_t const input[] = {JSON_SYM_LB,    JSON_SYM_STR,   JSON_SYM_COLON,
                          JSON_SYM_NULL,  JSON_SYM_COMMA, JSON_SYM_STR,
                          JSON_SYM_COLON, JSON_SYM_REAL,  JSON_SYM_COMMA,
                          JSON_SYM_STR,   JSON_SYM_COLON, JSON_SYM_TRUE,
                          JSON_SYM_RB,    UT_SYM_EOF};
  UTILLIB_TEST_ASSERT(utillib_json_parser_parse_dbg(UT_FIXTURE, input));
}

UTILLIB_TEST(json_parser_empty_object) {
  size_t const input[] = {JSON_SYM_LB, JSON_SYM_RB, UT_SYM_EOF};
  UTILLIB_TEST_ASSERT(utillib_json_parser_parse_dbg(UT_FIXTURE, input));
}

UTILLIB_TEST(json_parser_one_elem_array) {
  /* [ null ] */
  size_t const input[] = {JSON_SYM_LK, JSON_SYM_NULL, JSON_SYM_RK, UT_SYM_EOF};
  UTILLIB_TEST_ASSERT(utillib_json_parser_parse_dbg(UT_FIXTURE, input));
}

UTILLIB_TEST(json_parser_nested_array) {
  /* [ [null], [ null, null], [null, null, null] ] */
  size_t const input[] = {
      JSON_SYM_LK,    JSON_SYM_LK,    JSON_SYM_NULL,  JSON_SYM_RK,
      JSON_SYM_COMMA, JSON_SYM_LK,    JSON_SYM_NULL,  JSON_SYM_COMMA,
      JSON_SYM_NULL,  JSON_SYM_RK,    JSON_SYM_COMMA, JSON_SYM_LK,
      JSON_SYM_NULL,  JSON_SYM_COMMA, JSON_SYM_NULL,  JSON_SYM_COMMA,
      JSON_SYM_NULL,  JSON_SYM_RK,    JSON_SYM_RK,    UT_SYM_EOF};
  UTILLIB_TEST_ASSERT(utillib_json_parser_parse_dbg(UT_FIXTURE, input));
}

UTILLIB_TEST(json_parser_nested_object) {
  /* { "string": { }, "string":{ "string":null,
   * "string": null }, "string": { "string":null,
   * "string":null, "string":null } }
   * }
   */
  size_t const input[] = {
      JSON_SYM_LB,    JSON_SYM_STR,   JSON_SYM_COLON, JSON_SYM_LB,
      JSON_SYM_RB,    JSON_SYM_COMMA, JSON_SYM_STR,   JSON_SYM_COLON,
      JSON_SYM_LB,    JSON_SYM_STR,   JSON_SYM_COLON, JSON_SYM_NULL,
      JSON_SYM_COMMA, JSON_SYM_STR,   JSON_SYM_COLON, JSON_SYM_NULL,
      JSON_SYM_RB,    JSON_SYM_COMMA, JSON_SYM_STR,   JSON_SYM_COLON,
      JSON_SYM_LB,    JSON_SYM_STR,   JSON_SYM_COLON, JSON_SYM_NULL,
      JSON_SYM_COMMA, JSON_SYM_STR,   JSON_SYM_COLON, JSON_SYM_NULL,
      JSON_SYM_COMMA, JSON_SYM_STR,   JSON_SYM_COLON, JSON_SYM_NULL,
      JSON_SYM_RB,    JSON_SYM_RB,    UT_SYM_EOF};
  UTILLIB_TEST_ASSERT(utillib_json_parser_parse_dbg(UT_FIXTURE, input));
}

UTILLIB_TEST(json_parser_deep_nested_array) {
  size_t const input[] = {JSON_SYM_LK, JSON_SYM_LK, JSON_SYM_LK, JSON_SYM_RK,
                          JSON_SYM_RK, JSON_SYM_RK, UT_SYM_EOF};
  UTILLIB_TEST_ASSERT(utillib_json_parser_parse_dbg(UT_FIXTURE, input));
}

UTILLIB_TEST(json_parser_scanner) {}

UTILLIB_TEST_DEFINE(Utillib_JSON_Parser) {
  UTILLIB_TEST_BEGIN(Utillib_JSON_Parser)
  UTILLIB_TEST_SKIP(json_parser_parse_str)
  UTILLIB_TEST_SKIP(json_parser_parse_null)
  UTILLIB_TEST_SKIP(json_parser_parse_number)
  UTILLIB_TEST_SKIP(json_parser_parse_bool)
  UTILLIB_TEST_SKIP(json_parser_parse_array)
  UTILLIB_TEST_SKIP(json_parser_empty_array)
  UTILLIB_TEST_SKIP(json_parser_parse_object)
  UTILLIB_TEST_SKIP(json_parser_empty_object)
  UTILLIB_TEST_SKIP(json_parser_nested_array)
  UTILLIB_TEST_SKIP(json_parser_nested_object)
  UTILLIB_TEST_RUN(json_parser_deep_nested_array)
  UTILLIB_TEST_END(Utillib_JSON_Parser)
  UTILLIB_TEST_FIXTURE(struct utillib_json_parser)
  UTILLIB_TEST_RETURN(Utillib_JSON_Parser)
}
