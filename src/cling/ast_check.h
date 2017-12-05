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

#ifndef CLING_AST_CHECK_H
#define CLING_AST_CHECK_H

#include "rd_parser.h"
#include "symbol_table.h"
#include <utillib/json.h>
/*
 * AST Semantic Checks
 */

enum cling_return_kind {
  CL_RET_GARBAGE = 1,
  CL_RET_DISCARD,
};

/*
 * Checks whether `name' exists and is assignable.
 * That is, not const, array and function.
 */
int cling_ast_check_assignable(char const *name,
                               struct cling_symbol_table const *symbol_table);

/*
 * Computes type of the expression and throws errors to `parser'
 * during computation.
 * Return negative if error ever happened.
 * Return CL_VOID, CL_INT or CL_CHAR if succeeded.
 */
int cling_ast_check_expression(struct utillib_json_value *self,
                               struct cling_rd_parser *parser,
                               struct utillib_token_scanner *input,
                               size_t context);

int cling_ast_check_returnness(
    struct utillib_json_value *self,
    struct cling_rd_parser *parser,
    struct utillib_token_scanner *input,
    size_t context,
    bool void_flag);


#endif /* CLING_AST_CHECK_H */
