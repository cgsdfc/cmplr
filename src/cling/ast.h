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
#ifndef CLING_AST_H
#define CLING_AST_H
#include <utillib/json.h>
#include "symbol_table.h"

/*
 * Everything about traversal the ast
 * is defined here.
 * Although our ast is actually json tree,
 * A lot can still be done on it.
 */
void cling_ast_insert_const(struct utillib_json_value * self,
    struct cling_symbol_table *symbols);

void cling_ast_insert_variable(struct utillib_json_value * self,
    struct cling_symbol_table *symbols);

void cling_ast_add_op(struct utillib_json_value * self, 
    size_t op);

void cling_ast_add_lhs(struct utillib_json_value * self, 
    struct utillib_json_value * lhs);

void cling_ast_add_rhs(struct utillib_json_value * self, 
    struct utillib_json_value * rhs);

#endif /* CLING_AST_H */

