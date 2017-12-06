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
#include "ast_ir.h"
#include "symbols.h"
#include <assert.h>


UTILLIB_ETAB_BEGIN(cling_ast_opcode_kind)
UTILLIB_ETAB_ELEM(OP_ADD)
UTILLIB_ETAB_ELEM(OP_SUB)
UTILLIB_ETAB_ELEM(OP_IDX)
UTILLIB_ETAB_ELEM(OP_CAL)
UTILLIB_ETAB_ELEM(OP_DIV)
UTILLIB_ETAB_ELEM(OP_MUL)
UTILLIB_ETAB_ELEM(OP_STORE)
UTILLIB_ETAB_END(cling_ast_opcode_kind);

static void ast_ir_emit_factor(struct utillib_json_value *self,
    struct cling_ir_gen *gen)
{


}

static int ast_ir_emit_expression(struct utillib_json_value *self,
    struct cling_ir_gen *gen)
{
  struct utillib_json_value *op, *lhs, *rhs;
  op=utillib_json_object_at(self, "op");
  if (!op) {


}


static void ast_ir_emit_for_stmt(struct utillib_json_value *self,
    struct cling_program *program)
{}


static void ast_ir_emit_if_stmt(struct utillib_json_value *self,
    struct cling_program *program)
{
  struct utillib_json_value *expr, *then_clause, *else_clause;
  expr=utillib_json_object_at(self, "expr");
  assert(expr);




}

static void ast_ir_emit_statement(struct utillib_json_value *self,
    struct cling_program *program)
{
  struct utillib_json_value *type;
  type=utillib_json_object_at(self, "type");
  assert(type);
  switch (type->as_size_t) {
    case SYM_IF_STMT:
      ast_ir_emit_if_stmt(self, program);
      return;
    case SYM_FOR_STMT:
      ast_ir_emit_for_stmt(self, program);
      return;
    default:
      assert(false);
  }
}


