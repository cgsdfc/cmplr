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
#ifndef UTILLIB_JSON_IMPL_H
#define UTILLIB_JSON_IMPL_H

#include "symbol.h"
#include "scanner.h"
#include "string.h"
#include "enum.h"
#include "rule.h"

UTILLIB_ENUM_BEGIN(utillib_json_symbol_kind)
  UTILLIB_ENUM_ELEM_INIT(UT_JSON_SYM_VAL, 1)
  UTILLIB_ENUM_ELEM(UT_JSON_SYM_OBJ)
  UTILLIB_ENUM_ELEM(UT_JSON_SYM_ARR)
  UTILLIB_ENUM_ELEM(UT_JSON_SYM_STRVAL)
  UTILLIB_ENUM_ELEM(UT_JSON_SYM_STRVAL_LS)
  UTILLIB_ENUM_ELEM(UT_JSON_SYM_VAL_)
  UTILLIB_ENUM_ELEM(UT_JSON_SYM_VAL_LS)

  UTILLIB_ENUM_ELEM(UT_JSON_SYM_STR)
  UTILLIB_ENUM_ELEM(UT_JSON_SYM_NUM)
  UTILLIB_ENUM_ELEM(UT_JSON_SYM_KW_TRUE)
  UTILLIB_ENUM_ELEM(UT_JSON_SYM_KW_FALSE)
  UTILLIB_ENUM_ELEM(UT_JSON_SYM_KW_NULL)
  UTILLIB_ENUM_ELEM(UT_JSON_SYM_LB)
  UTILLIB_ENUM_ELEM(UT_JSON_SYM_RB)
  UTILLIB_ENUM_ELEM(UT_JSON_SYM_LK)
  UTILLIB_ENUM_ELEM(UT_JSON_SYM_RK)
  UTILLIB_ENUM_ELEM(UT_JSON_SYM_COMMA)
  UTILLIB_ENUM_ELEM(UT_JSON_SYM_COLON)
UTILLIB_ENUM_END(utillib_json_symbol_kind);

extern struct utillib_symbol const utillib_json_symbols[];
extern struct utillib_rule_literal const utillib_json_rules[];
extern const struct utillib_scanner_op utillib_json_scanner_op;

struct utillib_json_scanner {
  char const * str;
  struct utillib_string buffer;
  size_t code;
};

void utillib_json_scanner_init(struct utillib_json_scanner *self);
size_t utillib_json_scanner_lookahead(struct utillib_json_scanner *self);
void utillib_json_scanner_shiftaway(struct utillib_json_scanner *self);
void const * utillib_json_scanner_semantic(struct utillib_json_scanner *self);
void utillib_json_scanner_destroy(struct utillib_json_scanner *self);


#endif /* UTILLIB_JSON_IMPL_H */

