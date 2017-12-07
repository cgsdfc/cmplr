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

static struct utillib_json_value *
polan_ir_node_json_object_create(struct utillib_json_value *self) {
  /*
   * Make a node look more like an operator or operand.
   */
  struct utillib_json_value *op, *value;
  char const *opstr;
  op = utillib_json_object_at(self, "op");
  if (!op) {
    /*
     * Get the string of this node
     */
    value = utillib_json_object_at(self, "value");
    return utillib_json_value_copy(value);
  }
  /*
   * Get the opstr from op
   */
  opstr = cling_symbol_kind_tostring(op->as_size_t);
  return utillib_json_string_create(&opstr);
}

static void polan_ir_post_order(struct cling_polan_ir *self,
                                struct utillib_json_value const *node) {
  assert(node);
  struct utillib_json_value *op, *lhs, *rhs;
  op = utillib_json_object_at(node, "op");
  if (!op) {
    /*
     * Trivial case.
     */
    utillib_vector_push_back(&self->stack, node);
    return;
  }
  lhs = utillib_json_object_at(node, "lhs");
  rhs = utillib_json_object_at(node, "rhs");
  polan_ir_post_order(self, lhs);
  polan_ir_post_order(self, rhs);
  utillib_vector_push_back(&self->stack, node);
}

void cling_polan_ir_init(struct cling_polan_ir *self) {
  utillib_vector_init(&self->stack);
}

void cling_polan_ir_destroy(struct cling_polan_ir *self) {
  utillib_vector_destroy(&self->stack);
}

void cling_polan_ir_walk(struct cling_polan_ir *self,
                         struct utillib_json_value const *root) {
  polan_ir_post_order(self, root);
}

struct utillib_json_value *
cling_polan_ir_json_array_create(struct cling_polan_ir const *self) {
  struct utillib_json_value *array;
  struct utillib_json_value const *node;

  array = utillib_json_array_create_empty();
  UTILLIB_VECTOR_FOREACH(node, &self->stack) {
    utillib_json_array_push_back(array, polan_ir_node_json_object_create(node));
  }
  return array;
}
