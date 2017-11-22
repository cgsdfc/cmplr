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

void pascal_ast_const_decl_init(struct pascal_ast_const_decl *self) {
  utillib_vector_init(&self->items);
}

void pascal_ast_var_decl_init(struct pascal_ast_var_decl *self) {
  utillib_vector_init(&self->items);
}

void pascal_ast_proc_decl_init(struct pascal_ast_proc_decl *self) {
  self->name = NULL;
  self->subprogram = NULL;
}

void pascal_ast_assign_stmt_init(struct pascal_ast_assign_stmt *self) {
  self->LHS = NULL;
  self->RHS = NULL;
}

void pascal_ast_read_stmt_init(struct pascal_ast_read_stmt *self) {
  utillib_vector_init(&self->items);
}

void pascal_ast_write_stmt_init(struct pascal_ast_write_stmt *self) {
  utillib_vector_init(&self->items);
}

void pascal_ast_term_init(struct pascal_ast_term *self) {
  self->op = 0;
  self->LHS = NULL;
  self->RHS = NULL;
}

void pascal_ast_expr_init(struct pascal_ast_expr *self) {
  self->sign = 0;
  self->op = 0;
  self->LHS = NULL;
  self->RHS = NULL;
}

void pascal_ast_cond_stmt_init(struct pascal_ast_cond_stmt *self) {
  self->cond = NULL;
  self->then = NULL;
}

void pascal_ast_comp_stmt_init(struct pascal_ast_comp_stmt *self) {
  utillib_vector_init(&self->items);
}

void pascal_ast_loop_stmt_init(struct pascal_ast_loop_stmt *self) {
  self->cond = NULL;
  self->do_ = NULL;
}

void pascal_ast_call_stmt_init(struct pascal_ast_call_stmt *self) {
  self->proc_name = NULL;
}

void pascal_ast_condition_init(struct pascal_ast_condition *self) {
  self->odd = false;
  self->op = 0;
  self->LHS = NULL;
  self->RHS = NULL;
}

void pascal_ast_subprogram_init(struct pascal_ast_subprogram *self) {
  self->const_decl = NULL;
  self->var_decl = NULL;
  utillib_vector_init(&self->proc_list);
  self->comp_stmt = NULL;
}

struct pascal_ast_node *pascal_ast_node_create(int kind) {
  struct pascal_ast_node *self = malloc(sizeof *self);
  self->kind = kind;
  void const *aux;
  union pascal_ast_union node;

  switch (kind) {
  case PAS_AST_CONST:
    self->as_ptr = malloc(sizeof *node.const_decl);
    pascal_ast_const_decl_init(self->as_ptr);
    return self;
  case PAS_AST_VAR:
    self->as_ptr = malloc(sizeof *node.var_decl);
    pascal_ast_var_decl_init(self->as_ptr);
    return self;
  case PAS_AST_ASSIGN:
    self->as_ptr = malloc(sizeof *node.assign_stmt);
    pascal_ast_assign_stmt_init(self->as_ptr);
    return self;
  case PAS_AST_READ:
    self->as_ptr = malloc(
    utillib_vector_init(self->as_ptr);
    return self;
  case PAS_AST_WRITE:
    self->as_ptr = malloc(
    utillib_vector_init(self->as_ptr);
    return self;
  case PAS_AST_TERM:
    self->as_ptr = malloc(
    memset(self->as_ptr, 0, sizeof(struct pascal_ast_term));
    return self;
  case PAS_AST_EXPR:
    self->as_ptr = malloc(
    memset(self->as_ptr, 0, sizeof(struct pascal_ast_expr));
    return self;
  case PAS_AST_CALL:
    self->as_ptr = malloc(
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
