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
#include <utillib/json.h>

struct utillib_json_value *
pascal_json_ast_create(int kind) {
  struct utillib_json_value *ast=utillib_json_object_create_empty();
  utillib_json_object_push_back(ast, "kind", 
      utillib_json_int_create(&kind));
  return ast;
}

int pascal_json_ast_kind(struct utillib_json_value const *self)
{
  struct utillib_json_value *kind=utillib_json_object_at(self, "kind");
  return kind->as_int;
}

/**
 * \function pascal_json_ast_push_back
 * Assumes that `self' is an object that
 * has an "item" field which is an json array
 * and add the item to the array.
 */
void pascal_json_ast_add_item(struct utillib_json_value *self,
    struct utillib_json_value const *item)
{
  struct utillib_json_value *array;
  array=utillib_json_object_at(self, "item");
  utillib_json_array_push_back(array, item);
}
