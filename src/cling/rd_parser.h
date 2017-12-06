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

/**
 * A predictive recursive-decent parser.
 */
#include "error.h"

#include <utillib/json.h>
#include <utillib/scanner.h>
#include <utillib/vector.h>

#include <setjmp.h>

#define RD_PARSER_SKIP_MAX 4

struct cling_rd_parser {
  jmp_buf fatal_saver;
  struct utillib_vector elist;
  char const *curfunc;                  /* used to check returnness */
  int tars[RD_PARSER_SKIP_MAX];         /* used to skip to a list of targets */
  struct utillib_json_value *root;      /* the root of AST */
  struct cling_symbol_table *symbol_table;
};

void cling_rd_parser_init(struct cling_rd_parser *self,
                          struct cling_symbol_table *symbol_table);

void cling_rd_parser_destroy(struct cling_rd_parser *self);

struct utillib_json_value *
cling_rd_parser_parse(struct cling_rd_parser *self,
                      struct utillib_token_scanner *input);

void cling_rd_parser_report_errors(struct cling_rd_parser const *self);

bool cling_rd_parser_has_errors(struct cling_rd_parser const *self);

void rd_parser_error_push_back(struct cling_rd_parser *self,
                               struct cling_error *error);

#endif /* CLING_RD_PARSER_H */
