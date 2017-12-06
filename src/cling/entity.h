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

#ifndef CLING_ENTITY_H
#define CLING_ENTITY_H
#include "ast.h"

#include <utillib/json.h>
#include <utillib/vector.h>

struct cling_entity_list {
  struct utillib_vector func_list;
  struct utillib_vector var_list;
};

struct cling_function {
  struct utillib_json_value *signature;
  struct utillib_vector code;
};

struct cling_variable {
  struct utillib_json_value *info;
  size_t scope;
};

struct cling_ir {
  int opcode;
  union cling_primary operands[CLING_AST_IR_MAX];
  size_t opcode;
};

void cling_entity_list_init(struct cling_entity_list *self);
void cling_entity_list_destroy(struct cling_entity_list *self);
struct cling_function *
cling_function_create(struct utillib_json_value *signature);
struct cling_variable *cling_variable_create(struct utillib_json_value *info,
                                             size_t scope);
struct cling_ir *cling_ir_create(size_t opcode);

#endif /* CLING_ENTITY_H */
