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
#include "entity.h"
#include <stdlib.h>

static void function_destroy(struct cling_function *self) {
  utillib_vector_destroy_owning(&self->code, free);
  free(self);
}

void cling_entity_list_init(struct cling_entity_list *self) {
  utillib_vector_init(&self->func_list);
  utillib_vector_init(&self->var_list);
}

void cling_entity_list_destroy(struct cling_entity_list *self) {
  utillib_vector_destroy_owning(&self->func_list, function_destroy);
  utillib_vector_destroy_owning(&self->var_list, free);
}

struct cling_function *
cling_function_create(struct utillib_json_value *signature) {
  struct cling_function *self = malloc(sizeof *self);
  self->signature = signature;
  utillib_vector_init(&self->code);
  return self;
}

struct cling_variable *cling_variable_create(struct utillib_json_value *info,
                                             size_t scope) {
  struct cling_variable *self = malloc(sizeof *self);
  self->info = info;
  self->scope = scope;
  return self;
}

struct cling_ir *cling_ir_create(size_t opcode) {
  struct cling_ir *self = malloc(sizeof *self);
  self->opcode = opcode;
  return self;
}
