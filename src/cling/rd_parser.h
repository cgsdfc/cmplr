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

#ifndef CLING_RD_PARSER_H
#define CLING_RD_PARSER_H
#include <setjmp.h>
#include <utillib/json.h>
#include <utillib/scanner.h>
#include <utillib/vector.h>

/**
 * \struct cling_rd_parser
 * A regular recursive-decent parser building
 * json AST as the result of parsing.
 * It is believed that separating parsing
 * from semantic analysis will simplify both.
 * That is, in the first place the parser tries
 * its best to build the tree which may have
 * branches as `null' since some grammatical errors.
 * Then, the semantic analyzor run on the tree and
 * find more specific errors.
 * After these 2 passes, the survival source will be
 * valid.
 * If either of these passes fatally failed, the program
 * must clean up, print errors and exit gracefully.
 *
 * Notes the routines of parser will return `&utillib_json_false'
 * if a non-fatal grammatical error took place during parsing
 * and returns `&utillib_json_null' if the construct is optional'.
 *
 */
struct cling_rd_parser {
  jmp_buf fatal_saver;
  struct utillib_vector elist;
};

void cling_rd_parser_init(struct cling_rd_parser *self);
void cling_rd_parser_destroy(struct cling_rd_parser *self);
struct utillib_json_value *
cling_rd_parser_parse(struct cling_rd_parser *self,
                      struct utillib_token_scanner *input);
void cling_rd_parser_report_errors(struct cling_rd_parser const *self);

#endif /* CLING_RD_PARSER_H */
