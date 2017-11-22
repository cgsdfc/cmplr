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

#ifndef PASCAL_JSON_AST_H
#define PASCAL_JSON_AST_H
#include <utillib/json.h>
/**
 * subprogram = {
 *  "const_decl": [],
 *  "var_decl": [],
 *  "proc_list": [],
 *  "comp_stmt": []
 * }
 *
 *
 */

struct utillib_json_value *pascal_json_ast_create(int kind);
int pascal_json_ast_kind(struct utillib_json_value const *self);
void pascal_json_ast_add_item(struct utillib_json_value *self,
                              struct utillib_json_value const *item);

#endif /* PASCAL_JSON_AST_H */
