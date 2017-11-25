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
#include "json_ast.h"

struct utillib_json_value *utillib_json_ast_create(struct utillib_symbol const *symbol)
{
  struct utillib_json_value *ast = utillib_json_object_create_empty();
  utillib_json_object_push_back(ast, "__kind__", utillib_json_int_create(&symbol->value));
  utillib_json_object_push_back(ast, "__name__", utillib_json_string_create(&symbol->name));
  return ast;
}

int utillib_json_ast_getkind(struct utillib_json_value const *self) {
  struct utillib_json_value *kind = utillib_json_object_at(self, "__kind__");
  return kind->as_int;
}

char const * utillib_json_ast_getname(struct utillib_json_value const *self) {
  struct utillib_json_value *name = utillib_json_object_at(self, "__name__");
  return name->as_ptr;
}



