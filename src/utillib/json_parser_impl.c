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

UTILLIB_SYMBOL_BEGIN(utillib_json_symbols)
UTILLIB_SYMBOL_NON_TERMINAL(JSON_SYM_VAL)
UTILLIB_SYMBOL_NON_TERMINAL(JSON_SYM_OBJ)
UTILLIB_SYMBOL_NON_TERMINAL(JSON_SYM_ARR)
UTILLIB_SYMBOL_NON_TERMINAL(JSON_SYM_STRVAL)
UTILLIB_SYMBOL_NON_TERMINAL(JSON_SYM_STRVAL_LS)
UTILLIB_SYMBOL_NON_TERMINAL(JSON_SYM_VAL_)
UTILLIB_SYMBOL_NON_TERMINAL(JSON_SYM_VAL_LS)

UTILLIB_SYMBOL_TERMINAL(JSON_SYM_STR)
UTILLIB_SYMBOL_TERMINAL(JSON_SYM_NUM)
UTILLIB_SYMBOL_TERMINAL(JSON_SYM_TRUE)
UTILLIB_SYMBOL_TERMINAL(JSON_SYM_FALSE)
UTILLIB_SYMBOL_TERMINAL(JSON_SYM_NULL)
UTILLIB_SYMBOL_TERMINAL(JSON_SYM_LB)
UTILLIB_SYMBOL_TERMINAL(JSON_SYM_RB)
UTILLIB_SYMBOL_TERMINAL(JSON_SYM_LK)
UTILLIB_SYMBOL_TERMINAL(JSON_SYM_RK)
UTILLIB_SYMBOL_TERMINAL(JSON_SYM_COMMA)
UTILLIB_SYMBOL_TERMINAL(JSON_SYM_COLON)
UTILLIB_SYMBOL_END(utillib_json_symbols);

/*
 * Rules for JSON:
 * value := object | array | number | true | false | null | string
 * object := '{' [strval] {, strval } '}'
 * array := '[' [value], {, value } ']'
 * strval := string ':' value
 *
 * Reference:
 * https://www.json.org/json-zh.html
 *
 * Notes that `JSON_SYM_VAL_' := epsilon
 * and `JSON_SYM_STRVAL' := epsilon.
 * That is object and array can be empty.
 */

UTILLIB_RULE_BEGIN(utillib_json_rules)
UTILLIB_RULE_ELEM(JSON_SYM_VAL, JSON_SYM_STR)
UTILLIB_RULE_ELEM(JSON_SYM_VAL, JSON_SYM_NUM)
UTILLIB_RULE_ELEM(JSON_SYM_VAL, JSON_SYM_OBJ)
UTILLIB_RULE_ELEM(JSON_SYM_VAL, JSON_SYM_ARR)
UTILLIB_RULE_ELEM(JSON_SYM_VAL, JSON_SYM_TRUE)
UTILLIB_RULE_ELEM(JSON_SYM_VAL, JSON_SYM_FALSE)
UTILLIB_RULE_ELEM(JSON_SYM_VAL, JSON_SYM_NULL)
UTILLIB_RULE_ELEM(JSON_SYM_OBJ, JSON_SYM_LB, JSON_SYM_STRVAL,
                  JSON_SYM_STRVAL_LS, JSON_SYM_RB)
UTILLIB_RULE_ELEM(JSON_SYM_ARR, JSON_SYM_LK, JSON_SYM_VAL_, JSON_SYM_VAL_LS,
                  JSON_SYM_RK)
UTILLIB_RULE_ELEM(JSON_SYM_VAL_, JSON_SYM_VAL)
UTILLIB_RULE_ELEM(JSON_SYM_VAL_, UT_SYM_EPS)
UTILLIB_RULE_ELEM(JSON_SYM_VAL_LS, JSON_SYM_COMMA, JSON_SYM_VAL,
                  JSON_SYM_VAL_LS)
UTILLIB_RULE_ELEM(JSON_SYM_STRVAL, JSON_SYM_STR, JSON_SYM_COLON, JSON_SYM_VAL)
UTILLIB_RULE_ELEM(JSON_SYM_STRVAL, UT_SYM_EPS)
UTILLIB_RULE_ELEM(JSON_SYM_STRVAL_LS, JSON_SYM_COMMA, JSON_SYM_STRVAL,
                  JSON_SYM_STRVAL_LS)
UTILLIB_RULE_ELEM(JSON_SYM_VAL_LS, UT_SYM_EPS)
UTILLIB_RULE_ELEM(JSON_SYM_STRVAL_LS, UT_SYM_EPS)
UTILLIB_RULE_END(utillib_json_rules);
