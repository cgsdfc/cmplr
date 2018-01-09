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
#ifndef CL_AST_BUILD_H
#define CL_AST_BUILD_H

#include <stddef.h>
#include <utillib/json.h>

/*
 * AST construction helpers.
 */

/*
 * Builds a json string from a c string.
 */
struct utillib_json_value *cling_ast_string(char const *rawstr);

struct utillib_json_value *cling_ast_factor(size_t code, char const *string);

void cling_ast_set_op(struct utillib_json_value *self, size_t op);

void cling_ast_set_opstr(struct utillib_json_value *self, size_t op);

void cling_ast_set_extend(struct utillib_json_value *self,
                struct utillib_json_value *extend);

void cling_ast_set_lhs(struct utillib_json_value *self,
                struct utillib_json_value *lhs);

void cling_ast_set_rhs(struct utillib_json_value *self,
                struct utillib_json_value *rhs);

struct utillib_json_value *cling_ast_statement(size_t type);

char *cling_ast_set_name(struct utillib_json_value *self, char const *name);
void cling_ast_set_type(struct utillib_json_value *self, size_t type);

#endif /* CL_AST_BUILD */
