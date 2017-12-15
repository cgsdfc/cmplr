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

#include <cling/cling.h>

#include <stdlib.h>

static struct utillib_token_scanner cling_scanner;
static struct cling_rd_parser cling_parser;
static struct cling_symbol_table cling_symbol_table;
static struct utillib_json_value *json_ast;
static struct cling_ast_program program;
static struct cling_mips_program cling_mips;

static char const * append_ext(char const *fn, char const *ext) {
    static char buffer[64];
    snprintf(buffer, 64, "%s.%s", fn, ext);
    return buffer;
}

int main(int argc, char *argv[]) {
  FILE *file, *ir_file, *mips_file;

  if (argc != 2) {
    printf("Please supply a filename QAQ\n");
    exit(1);
  }

  file = fopen(argv[1], "r");
  cling_scanner_init(&cling_scanner, file);
  cling_symbol_table_init(&cling_symbol_table);
  cling_rd_parser_init(&cling_parser, &cling_symbol_table);

  json_ast = cling_rd_parser_parse(&cling_parser, &cling_scanner);
  if (cling_rd_parser_has_errors(&cling_parser)) {
    cling_rd_parser_report_errors(&cling_parser);
    goto cleanup;
  }

  cling_ast_program_init(&program);
  cling_ast_ir_emit_program(json_ast, &cling_symbol_table, &program);
  cling_mips_program_init(&cling_mips, &program);
  cling_mips_program_emit(&cling_mips, &program);

  ir_file=fopen(append_ext(argv[1], "ir.txt"), "w");
  if (!ir_file)
    goto cleanup;

  cling_ast_program_print(&program, ir_file);
  fclose(ir_file);

  mips_file=fopen(append_ext(argv[1], "mips.s"), "w");
  if (!mips_file)
  goto cleanup;

  cling_mips_program_print(&cling_mips, mips_file);
  fclose(mips_file);
  puts("done");

cleanup:
  if (json_ast)
    utillib_json_value_destroy(json_ast);
  cling_ast_program_destroy(&program);
  cling_rd_parser_destroy(&cling_parser);
  cling_symbol_table_destroy(&cling_symbol_table);
  cling_scanner_destroy(&cling_scanner);
  cling_mips_program_destroy(&cling_mips);
}
