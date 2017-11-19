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
#include <stdlib.h> // strtoul free
#include <string.h>
#include <utillib/pair.h>

struct pascal_ast_node *pascal_ast_node_create_empty(int kind) {
  struct pascal_ast_node *self = malloc(sizeof *self);
  self->kind = kind;
  void const *aux;
  union pascal_ast_union node;

  switch (kind) {
  case PAS_AST_CONST:
    self->as_ptr = malloc(sizeof(struct pascal_ast_const_decl));
    utillib_vector_init(self->as_ptr);
    return self;
  case PAS_AST_VAR:
    self->as_ptr = malloc(sizeof(struct pascal_ast_var_decl));
    utillib_vector_init(self->as_ptr);
    return self;
  case PAS_AST_ASSIGN:
    self->as_ptr = malloc(sizeof(struct pascal_ast_assign_stmt));
    memset(self->as_ptr, 0, sizeof(struct pascal_ast_assign_stmt));
    return self;
  case PAS_AST_READ:
    self->as_ptr = malloc(sizeof(struct pascal_ast_read_stmt));
    utillib_vector_init(self->as_ptr);
    return self;
  case PAS_AST_WRITE:
    self->as_ptr = malloc(sizeof(struct pascal_ast_write_stmt));
    utillib_vector_init(self->as_ptr);
    return self;
  case PAS_AST_TERM:
    self->as_ptr = malloc(sizeof(struct pascal_ast_term));
    memset(self->as_ptr, 0, sizeof(struct pascal_ast_term));
    return self;
  case PAS_AST_EXPR:
    self->as_ptr = malloc(sizeof(struct pascal_ast_expr));
    memset(self->as_ptr, 0, sizeof(struct pascal_ast_expr));
    return self;
  case PAS_AST_CALL:
    self->as_ptr = malloc(sizeof(struct pascal_ast_call_stmt));
    return self;
  case PAS_AST_UINT:
    return self;
  default:
    return self;
  }
}

void pascal_ast_node_destroy(struct pascal_ast_node *self) {
  struct utillib_vector *items;
  switch (self->kind) {
  case PAS_AST_IDEN:
    free(self->as_iden);
    break;
  case PAS_AST_UINT:
    break;
  case PAS_AST_CONST:
    items = self->as_ptr;
    UTILLIB_VECTOR_FOREACH(struct utillib_pair *, pair, items) {
      pascal_ast_node_destroy(pair->up_first);
      pascal_ast_node_destroy(pair->up_second);
      free(pair);
    }
    utillib_vector_destroy(items);
    break;
  }
}
