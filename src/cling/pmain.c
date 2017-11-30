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

#include "rd_parser.h"
#include "symbol_table.h"
#include "opg_parser.h"
#include "entity.h"
#include "scanner.h"

#include <stdlib.h>

static struct utillib_token_scanner cling_scanner;
static struct cling_rd_parser cling_parser;
static struct cling_symbol_table cling_symbol_table;
static struct utillib_json_value *json_ast;
static struct cling_entity_list cling_entities;
static struct cling_opg_parser opg_parser;
static struct utillib_json_value * json_value;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    abort();
  }
  FILE *file = fopen(argv[1], "r");
  cling_scanner_init(&cling_scanner, file);
  cling_symbol_table_init(&cling_symbol_table);
  cling_rd_parser_init(&cling_parser, &cling_symbol_table, &cling_entities);
  cling_opg_parser_init(&opg_parser, UT_SYM_EOF);

  json_ast = cling_rd_parser_parse(&cling_parser, &cling_scanner);
  if (!json_ast) {
    goto cleanup;
  }
  utillib_json_pretty_print(json_ast, stdout);
  /* json_value=cling_symbol_table_json_object_create(&cling_symbol_table); */
  /* utillib_json_pretty_print(json_value, stdout); */

cleanup:
  cling_rd_parser_report_errors(&cling_parser);
  if (json_ast)
    utillib_json_value_destroy(json_ast);
  if (json_value)
    utillib_json_value_destroy(json_value);

  cling_opg_parser_destroy(&opg_parser);
  cling_rd_parser_destroy(&cling_parser);
  cling_symbol_table_destroy(&cling_symbol_table);
  cling_scanner_destroy(&cling_scanner);
}
