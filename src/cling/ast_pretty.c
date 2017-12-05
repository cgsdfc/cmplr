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
#include "ast_pretty.h"
#include "symbols.h"

#include <utillib/json_foreach.h>

static void ast_pretty_expr(struct utillib_json_value const *self,
    struct utillib_string *string);

static void ast_pretty_factor(struct utillib_json_value const *self,
    struct utillib_string *string)
{
  struct utillib_json_value *value;
  value=utillib_json_object_at(self, "value");
  utillib_string_append(string, value->as_ptr);
}

static void ast_pretty_call(struct utillib_json_value const *self,
    struct utillib_string *string)
{
  struct utillib_json_value *lhs, *rhs;
  lhs=utillib_json_object_at(self, "lhs");
  rhs=utillib_json_object_at(self, "rhs");
  ast_pretty_factor(lhs, string);
  utillib_string_append(string, "(");
  if (utillib_json_array_size(rhs) == 0) {
    utillib_string_append(string, ")");
    return;
  }
  UTILLIB_JSON_ARRAY_FOREACH(arg, rhs) {
    ast_pretty_expr(arg, string);
    utillib_string_append(string, ", ");
  }
  utillib_string_erase_last(string);
  utillib_string_replace_last(string, ')');
}

static void ast_pretty_subscript(
    struct utillib_json_value const *self,
    struct utillib_string *string)
{
  struct utillib_json_value *lhs, *rhs;
  lhs=utillib_json_object_at(self, "lhs");
  rhs=utillib_json_object_at(self, "rhs");
  ast_pretty_factor(lhs, string);
  utillib_string_append(string, "[");
  ast_pretty_expr(rhs, string);
  utillib_string_append(string, "]");
}

static void ast_pretty_binary(
    struct utillib_json_value const *self,
    struct utillib_string *string)
{
  struct utillib_json_value *lhs, *rhs, *op;
  lhs=utillib_json_object_at(self, "lhs");
  rhs=utillib_json_object_at(self, "rhs");
  op=utillib_json_object_at(self, "op");

  ast_pretty_expr(lhs, string);
  utillib_string_append(string, " ");
  utillib_string_append(string, 
      cling_symbol_kind_tostring(op->as_size_t));
  utillib_string_append(string, " ");
  ast_pretty_expr(rhs, string);
}

static void ast_pretty_expr(struct utillib_json_value const *self,
    struct utillib_string *string)
{
  struct utillib_json_value *op;
  op=utillib_json_object_at(self, "op");
  if (!op) {
    ast_pretty_factor(self, string);
    return;
  }
  switch(op->as_size_t) {
  case SYM_RK:
    ast_pretty_subscript(self, string);
    return;
  case SYM_RP:
    ast_pretty_call(self, string);
    return;
  default:
    ast_pretty_binary(self, string);
    return;
  }
}

void cling_ast_pretty_expr(struct utillib_json_value const *self,
    struct utillib_string *string)
{
  utillib_string_init(string);
  ast_pretty_expr(self, string);
}

