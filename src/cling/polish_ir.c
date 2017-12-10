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
#include "polish_ir.h"
#include "ir.h"
#include "symbol_table.h"
#include "symbols.h"

#include <assert.h>
#include <stdlib.h>
#include <utillib/json.h>
#include <utillib/json_foreach.h>

static void polish_ir_emit_factor(struct cling_polish_ir const *self,
                                  struct utillib_json_value const *var,
                                  struct cling_ast_ir *ir, int index) {
  if (var->kind == UT_JSON_INT) {
    init_temp(&ir->operands[index], var->as_int);
    return;
  }
  emit_factor(&ir->operands[index], var, self->global->symbol_table);
}

/*
 * First walk the ast to generate
 * Reversed-Polish form of ir and
 * then generate ast-ir from that.
 */
static struct utillib_json_value *
polish_ir_node_json_object_create(struct utillib_json_value const *self) {
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

static void polish_ir_post_order(struct cling_polish_ir *self,
                                 struct utillib_json_value const *node) {
  assert(node);
  struct utillib_json_value *op, *lhs, *rhs;
  struct utillib_json_value const *arg;

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
  if (op->as_size_t == SYM_RP) {
    /*
     * call_expr case
     * first arglist, then name.
     */
    UTILLIB_JSON_ARRAY_FOREACH(arg, rhs) { polish_ir_post_order(self, arg); }
    polish_ir_post_order(self, lhs);
  } else if (op->as_size_t == SYM_EQ) {
    /*
     * Assignment is right associative.
     */
    polish_ir_post_order(self, rhs);
    polish_ir_post_order(self, lhs);
  } else {
    /*
     * Other left associative binary operators.
     */
    polish_ir_post_order(self, lhs);
    polish_ir_post_order(self, rhs);
  }
  utillib_vector_push_back(&self->stack, node);
}

static struct utillib_json_value *
polish_ir_make_temp(struct cling_polish_ir *self) {
  struct utillib_json_value *temp;
  temp = utillib_json_int_create(&self->global->temps);
  ++self->global->temps;
  return temp;
}

static void
polish_ir_maybe_release_temp(struct utillib_json_value *maybe_temp) {
  if (maybe_temp->kind == UT_JSON_INT)
    utillib_json_value_destroy(maybe_temp);
}

static size_t polish_ir_fetch_argc(struct cling_polish_ir *self,
                                   char const *name) {
  struct cling_symbol_entry *entry;
  entry = cling_symbol_table_find(self->global->symbol_table, name, CL_GLOBAL);
  return entry->function.argc;
}

static int polish_ir_fetch_return_type(struct cling_polish_ir *self,
                                       char const *name) {
  struct cling_symbol_entry *entry;
  entry = cling_symbol_table_find(self->global->symbol_table, name, CL_GLOBAL);
  return entry->function.return_type;
}

/*
 * Notes, __cdecl's Reversed order is
 * determined in this function.
 */
static void polish_ir_emit_call(struct cling_polish_ir *self,
                                struct utillib_vector *instrs) {
  struct utillib_json_value *lhs, *arg;
  struct utillib_json_value *value, *temp;
  struct cling_ast_ir *ir;
  size_t argc, return_type;

  lhs = utillib_vector_back(&self->opstack);
  value = utillib_json_object_at(lhs, "value");
  argc = polish_ir_fetch_argc(self, value->as_ptr);
  return_type = polish_ir_fetch_return_type(self, value->as_ptr);
  utillib_vector_pop_back(&self->opstack);
  for (int i = 0; i < argc; ++i) {
    arg = utillib_vector_back(&self->opstack);
    utillib_vector_pop_back(&self->opstack);
    ir = emit_ir(OP_PUSH);
    polish_ir_emit_factor(self, arg, ir, 0);
    utillib_vector_push_back(instrs, ir);
    polish_ir_maybe_release_temp(arg);
  }
  if (return_type != CL_VOID) {
    /*
     * If function has a return value,
     * the call will have a temp to hold that.
     * Otherwise, no temp is made or the temp
     * is simply CL_NULL.
     */
    temp = polish_ir_make_temp(self);
    ir = emit_call( value->as_ptr,temp->as_int);
    utillib_vector_push_back(&self->opstack, temp);
  } else {
    ir = emit_call(value->as_ptr, -1);
  }
  utillib_vector_push_back(instrs, ir);
}

static void polish_ir_emit_binary(struct cling_polish_ir *self, size_t op,
                                  struct utillib_vector *instrs) {
  struct utillib_json_value *lhs, *rhs, *temp;
  struct cling_ast_ir *ir;

  rhs = utillib_vector_back(&self->opstack);
  utillib_vector_pop_back(&self->opstack);
  lhs = utillib_vector_back(&self->opstack);
  utillib_vector_pop_back(&self->opstack);
  switch (op) {
  case SYM_RK:
    ir = emit_ir(OP_IDX);
    break;
  case SYM_ADD:
    ir = emit_ir(OP_ADD);
    break;
  case SYM_MINUS:
    ir = emit_ir(OP_SUB);
    break;
  case SYM_MUL:
    ir = emit_ir(OP_MUL);
    break;
  case SYM_DIV:
    ir = emit_ir(OP_DIV);
    break;
  case SYM_DEQ:
    ir = emit_ir(OP_EQ);
    break;
  case SYM_NE:
    ir = emit_ir(OP_NE);
    break;
  case SYM_LT:
    ir = emit_ir(OP_LT);
    break;
  case SYM_LE:
    ir = emit_ir(OP_LE);
    break;
  case SYM_GT:
    ir = emit_ir(OP_GT);
    break;
  case SYM_GE:
    ir = emit_ir(OP_GE);
    break;
  default:
    assert(false);
  }
  temp = polish_ir_make_temp(self);
  polish_ir_emit_factor(self, temp, ir, 0);
  polish_ir_emit_factor(self, lhs, ir, 1);
  polish_ir_emit_factor(self, rhs, ir, 2);
  utillib_vector_push_back(&self->opstack, temp);
  utillib_vector_push_back(instrs, ir);
  polish_ir_maybe_release_temp(lhs);
  polish_ir_maybe_release_temp(rhs);
}

static void polish_ir_emit_assign(struct cling_polish_ir *self,
                                  struct utillib_vector *instrs) {
  /*
   * OP_STORE name | temp1 temp2
   * stores temp2 to the address referred by `name'
   * or pointed to by `temp1'.
   * How to map a name to an address is out of concern
   * here.
   */

  struct utillib_json_value *lhs, *rhs;
  struct cling_ast_ir *ir;

  lhs = utillib_vector_back(&self->opstack);
  utillib_vector_pop_back(&self->opstack);
  rhs = utillib_vector_back(&self->opstack);
  utillib_vector_pop_back(&self->opstack);
  ir = emit_ir(OP_STORE);
  polish_ir_emit_factor(self, lhs, ir, 0);
  polish_ir_emit_factor(self, rhs, ir, 1);
  utillib_vector_push_back(instrs, ir);
  /*
   * According to the grammar, assign_expr
   * has no value so no need allocate temp.
   */
  polish_ir_maybe_release_temp(lhs);
  polish_ir_maybe_release_temp(rhs);
}

void cling_polish_ir_emit(struct cling_polish_ir *self,
                          struct utillib_vector *instrs) {
  struct utillib_json_value const *object, *op, *lhs, *rhs;
  struct cling_ast_ir *ir;
  struct utillib_json_value *temp;
  struct utillib_vector *stack = &self->stack;
  struct utillib_vector *opstack = &self->opstack;

  UTILLIB_VECTOR_FOREACH(object, &self->stack) {
    op = utillib_json_object_at(object, "op");
    if (!op) {
      /*
       * operands
       */
      utillib_vector_push_back(opstack, object);
      continue;
    }
    switch (op->as_size_t) {
    case SYM_RP:
      polish_ir_emit_call(self, instrs);
      break;
    case SYM_EQ:
      polish_ir_emit_assign(self, instrs);
      break;
    default:
      /*
       * polish_ir_emit_binary handles all the
       * rest of cases.
       */
      polish_ir_emit_binary(self, op->as_size_t, instrs);
      break;
    }
  }
}

void cling_polish_ir_result(struct cling_polish_ir const *self,
    struct cling_ast_ir *ir, int index) {
  struct utillib_json_value *result = utillib_vector_back(&self->opstack);
  polish_ir_emit_factor(self, result, ir, index);
}

struct utillib_json_value *
cling_polish_ir_json_array_create(struct cling_polish_ir const *self) {
  struct utillib_json_value *array;
  struct utillib_json_value const *object;

  array = utillib_json_array_create_empty();
  UTILLIB_VECTOR_FOREACH(object, &self->stack) {
    utillib_json_array_push_back(array,
                                 polish_ir_node_json_object_create(object));
  }
  return array;
}

void cling_polish_ir_init(struct cling_polish_ir *self,
                          struct utillib_json_value const *root,
                          struct cling_ast_ir_global *global) {
  self->global = global;
  utillib_vector_init(&self->stack);
  utillib_vector_init(&self->opstack);
  polish_ir_post_order(self, root);
}

void cling_polish_ir_destroy(struct cling_polish_ir *self) {
  utillib_vector_destroy(&self->stack);
  utillib_vector_destroy(&self->opstack);
}
