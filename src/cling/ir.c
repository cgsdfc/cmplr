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
#include "ir.h"
#include <utillib/json.h>
#include <stdlib.h>

UTILLIB_ETAB_BEGIN(cling_ast_opcode_kind)
UTILLIB_ETAB_ELEM(OP_ADD)
UTILLIB_ETAB_ELEM(OP_SUB)
UTILLIB_ETAB_ELEM(OP_IDX)
UTILLIB_ETAB_ELEM(OP_CAL)
UTILLIB_ETAB_ELEM(OP_DIV)
UTILLIB_ETAB_ELEM(OP_MUL)
UTILLIB_ETAB_ELEM(OP_STORE)
UTILLIB_ETAB_END(cling_ast_opcode_kind);

struct cling_ast_ir *
emit_ir(int opcode) {
  struct cling_ast_ir *self=malloc(sizeof *self);
  self->opcode=opcode;
  return self;
}

struct cling_ast_ir *
emit_call(int value, char const *name)
{
  struct cling_ast_ir *self=emit_ir(OP_CAL);
  self->operands[0].scalar=value;
  self->operands[1].text=name;
}

void cling_ast_function_push_back(struct cling_ast_function *self,
    struct cling_ast_ir const* ast_ir)
{
  utillib_vector_push_back(&self->instrs, ast_ir);
}

void cling_ast_function_init(struct cling_ast_function *self, char const *name)
{
  self->name=strdup(name);
  self->temps=0;
  utillib_vector_init(&self->instrs);
}

void cling_ast_function_destroy(struct cling_ast_function *self)
{
  free(self->name);
  utillib_vector_destroy(&self->instrs);
}

struct cling_ast_ir *
emit_defcon(int type, char const *name, char const *value) {
  struct cling_ast_ir *self=emit_ir(OP_DEFCON);
  self->operands[0].scalar=type;
  self->operands[1].text=name;
  self->operands[2].text=value;
  return self;
}

struct cling_ast_ir *
emit_defvar(int type, char const* name, char const *extend) {
  struct cling_ast_ir *self=emit_ir(OP_DEFVAR);
  self->operands[0].scalar=type;
  self->operands[1].text=name;
  self->operands[2].text=extend;
  return self;
}

struct cling_ast_ir *
emit_defunc(int type, char const *name) {
  struct cling_ast_ir *self=emit_ir(OP_DEFUNC);
  self->operands[0].scalar=type;
  self->operands[1].text=name;
  return self;
}

struct cling_ast_ir *
emit_para(int type, char const *name) {
  struct cling_ast_ir *self=emit_ir(OP_PARA);
  self->operands[0].scalar=type;
  self->operands[1].text=name;
  return self;
}

void cling_ast_program_init(struct cling_ast_program *self)
{
  utillib_vector_init(&self->init_code);
  utillib_vector_init(&self->funcs);
}

void cling_ast_program_destroy(struct cling_ast_program *self)
{

}

