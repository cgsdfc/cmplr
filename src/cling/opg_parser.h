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
#ifndef CLING_OPG_PARSER
#define CLING_OPG_PARSER
#include "symbols.h"

#include <utillib/vector.h>
#include <utillib/scanner.h>
#include <utillib/json.h>

#include <stddef.h>

/**
 * \struct cling_opg_parser
 * This parser will parse every
 * occurance of EXPRESSION.
 */

struct cling_opg_parser {
  struct utillib_vector stack;
  struct utillib_vector opstack;
  size_t eof_symbol;
  size_t last_error;
};

void cling_opg_parser_init(struct cling_opg_parser *self,
    size_t eof_symbol);

void cling_opg_parser_destroy(struct cling_opg_parser *self);

struct utillib_json_value *
cling_opg_parser_parse(struct cling_opg_parser *self, 
                       struct utillib_token_scanner *input);

void cling_opg_parser_reinit(struct cling_opg_parser *self);

/* ＜因子＞  ::= ＜标识符＞｜＜标识符＞‘[’＜表达式＞‘]’｜＜整数＞|＜字符＞｜ */
/* ＜有返回值函数调用语句＞|‘(’＜表达式＞‘)’ */
#endif /* CLING_OPG_PARSER */
