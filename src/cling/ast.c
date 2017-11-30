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
#include "ast.h"
#include "symbols.h"

#include <utillib/json_foreach.h>
/**
 * \function cling_ast_insert_const
 * Inserts a single_const_decl into the symbol_table.
 * Assumes self is not null.
 * Assumes all the symbol in `self' are not redefined.
 * Inserts into the current scope.
 */

void cling_ast_insert_const(struct utillib_json_value * self,
    struct cling_symbol_table *symbols)
{
  struct utillib_json_value *type=utillib_json_object_at(self, "type");
  struct utillib_json_value *const_defs=utillib_json_object_at(self, "const_defs");
  int kind=CL_CONST | (type->as_size_t == SYM_KW_INT ? CL_INT : CL_CHAR);
  int retv;

  UTILLIB_JSON_ARRAY_FOREACH(obj, const_defs) {
    struct utillib_json_value * identifier=utillib_json_object_at(obj, "identifier");
    assert(identifier);
    cling_symbol_table_update(symbols, kind, identifier->as_ptr, obj);
   }
}

void cling_ast_insert_variable(struct utillib_json_value * self,
    struct cling_symbol_table *symbols)
{
  struct utillib_json_value * type=utillib_json_object_at(self, "type");
  struct utillib_json_value * var_defs=utillib_json_object_at(self, "var_defs");
  int kind=type->as_size_t == SYM_KW_INT?CL_INT: CL_CHAR;
  int retv;

  UTILLIB_JSON_ARRAY_FOREACH(obj, var_defs) {
    struct utillib_json_value * identifier=utillib_json_object_at(obj, "identifier");
    assert(identifier);
    cling_symbol_table_update(symbols, 
        utillib_json_object_at(obj, "extend")?kind|CL_ARRAY : kind ,
        identifier->as_ptr, obj);
  }
}

void cling_ast_add_op(struct utillib_json_value * self, 
    size_t op)
{
  utillib_json_object_push_back(self, "op",
      utillib_json_size_t_create(&op));
}

void cling_ast_add_lhs(struct utillib_json_value * self, 
    struct utillib_json_value * lhs)
{
  utillib_json_object_push_back(self, "lhs", lhs);
}

void cling_ast_add_rhs(struct utillib_json_value * self, 
    struct utillib_json_value * rhs)
{
  utillib_json_object_push_back(self, "rhs", rhs);
}

struct utillib_json_value *
cling_ast_constant(size_t code, void const *semantic)
{
  switch(code) {
  case SYM_CHAR:
  case SYM_UINT:
    return utillib_json_size_t_create(&semantic);
  default:
    assert(false);
  }
}

struct utillib_json_value *
cling_ast_statement(size_t type) 
{
  struct utillib_json_value * object=utillib_json_object_create_empty();
  utillib_json_object_push_back(object, "type",
      utillib_json_size_t_create(&type));
  return object;
}

void cling_ast_set_name(struct utillib_json_value *self, char const *name)
{
  utillib_json_object_push_back(self, "name",
      utillib_json_string_create(&name));
}

void cling_ast_set_type(struct utillib_json_value *self, char const *type)
{
  utillib_json_object_push_back(self, "type",
      utillib_json_size_t_create(&type));
}
