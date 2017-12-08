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
#include "symbols.h"

#include <utillib/json.h>
#include <utillib/json_foreach.h>
#include <stdlib.h>
#include <assert.h>

/*
 * First walk the ast to generate
 * Reversed-Polish form of ir and
 * then generate ast-ir from that.
 */
static struct utillib_json_value *
polish_ir_node_json_object_create(struct utillib_json_value const*self) {
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
  struct utillib_json_value const* arg;

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
  polish_ir_post_order(self, lhs);
  if (op->as_size_t == SYM_RP) {
    /*
     * call_expr case
     */
    UTILLIB_JSON_ARRAY_FOREACH(arg, rhs) {
      polish_ir_post_order(self, arg);
    }
  } else {
    polish_ir_post_order(self, rhs);
  }
  utillib_vector_push_back(&self->stack, node);
}

/*
 * Translates the `factor' to one of CL_NAME,
 * CL_TEMP or CL_IMME and put it onto to
 * the `index' operand of `ir'.
 */
static void polish_ir_emit_factor(
    struct utillib_json_value const* var,
    struct cling_ast_ir *ir, int index)
{
  struct utillib_json_value *value, *type;

  if (var->kind == UT_JSON_INT) {
    ir->operands[index].scalar=var->as_int;
    /*
     * Temps are all words.
     */
    ir->info[index]=CL_TEMP | CL_WORD;
  } else {
    /*
     * TODO: lookup symbol_table to
     * figure out its name and scope.
     */
    type=utillib_json_object_at(var, "type");
    value=utillib_json_object_at(var, "value");
    ir->operands[index].text=value->as_ptr;
    switch(type->as_size_t) {
      case SYM_IDEN:
        ir->info[index]=CL_NAME;
        break;
      default:
        ir->info[index]=CL_IMME;
        break;
    }
  }
}

static struct utillib_json_value *
polish_ir_make_temp(struct cling_polish_ir *self) {
  struct utillib_json_value *temp;
  temp=utillib_json_int_create(&self->temps);
  ++self->temps;
  return temp;
}

/*
 * TODO: use symbol_table 
 * to figure out void and non-void
 * functions.
 */
static void polish_ir_emit_call(struct cling_polish_ir *self,
    struct utillib_vector *instrs)
{
  struct utillib_json_value *lhs, *arg, *value, *temp;
  struct cling_ast_ir *ir;

  lhs=utillib_vector_back(&self->opstack);
  utillib_vector_pop_back(&self->opstack);
  while (!utillib_vector_empty(&self->opstack)) {
    arg=utillib_vector_back(&self->opstack);
    utillib_vector_pop_back(&self->opstack);
    ir=emit_ir(OP_PUSH);
    polish_ir_emit_factor(arg, ir, 0);
    utillib_vector_push_back(instrs, ir);
    if (arg->kind == UT_JSON_INT)
      utillib_json_value_destroy(arg);
  }
  value=utillib_json_object_at(lhs, "value");
  temp=polish_ir_make_temp(self);
  ir=emit_call(temp->as_int, value->as_ptr);
  utillib_vector_push_back(instrs, ir);
  utillib_vector_push_back(&self->opstack, temp);
}

static void polish_ir_emit_binary(struct cling_polish_ir *self, size_t op,
    struct utillib_vector *instrs)
{
  struct utillib_json_value *lhs, *rhs, *temp;
  struct cling_ast_ir *ir;

  lhs=utillib_vector_back(&self->opstack);
  utillib_vector_pop_back(&self->opstack);
  rhs=utillib_vector_back(&self->opstack);
  utillib_vector_pop_back(&self->opstack);
  switch(op) {
    case SYM_ADD:
      ir=emit_ir(OP_ADD);
      break;
    case SYM_MINUS:
      ir=emit_ir(OP_SUB);
      break;
    case SYM_MUL:
      ir=emit_ir(OP_MUL);
      break;
    case SYM_DIV:
      ir=emit_ir(OP_DIV);
      break;
    case SYM_EQ:
      ir=emit_ir(OP_EQ);
      break;
    case SYM_NE:
      ir=emit_ir(OP_NE);
      break;
    case SYM_LT:
      ir=emit_ir(OP_LT);
      break;
    case SYM_LE:
      ir=emit_ir(OP_LE);
      break;
    case SYM_GT:
      ir=emit_ir(OP_GT);
      break;
    case SYM_GE:
      ir=emit_ir(OP_GE);
      break;
    case SYM_RK:
      ir=emit_ir(OP_IDX);
      break;
    default:
      assert(false);
  }
  temp=polish_ir_make_temp(self);
  polish_ir_emit_factor(temp, ir, 0);
  polish_ir_emit_factor(lhs, ir, 1);
  polish_ir_emit_factor(rhs, ir, 2);
  utillib_vector_push_back(&self->opstack, temp);
  utillib_vector_push_back(instrs, ir);
  if (lhs->kind == UT_JSON_INT)
    utillib_json_value_destroy(lhs);
  if (rhs->kind == UT_JSON_INT)
    utillib_json_value_destroy(rhs);
}

void cling_polish_ir_emit(struct cling_polish_ir *self,
    struct utillib_vector *instrs)
{
  struct utillib_json_value const* object, *op, *lhs, *rhs;
  struct cling_ast_ir *ir;
  struct utillib_json_value * temp;
  struct utillib_vector *stack=&self->stack;
  struct utillib_vector *opstack=&self->opstack;

  UTILLIB_VECTOR_FOREACH(object, &self->stack) {
    op=utillib_json_object_at(object, "op");
    if (!op) {
      /*
       * operands
       */
      utillib_vector_push_back(opstack, object);
      continue;
    }
    switch(op->as_size_t) {
      case SYM_RP:
        polish_ir_emit_call(self, instrs);
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
    struct cling_ast_ir *ir, int index)
{
  struct utillib_json_value *result=utillib_vector_back(&self->opstack);
  polish_ir_emit_factor(result, ir, index);
}

struct utillib_json_value *
cling_polish_ir_json_array_create(struct cling_polish_ir const *self) {
  struct utillib_json_value *array;
  struct utillib_json_value const *object;

  array = utillib_json_array_create_empty();
  UTILLIB_VECTOR_FOREACH(object, &self->stack) {
    utillib_json_array_push_back(array, polish_ir_node_json_object_create(object));
  }
  return array;
}

void cling_polish_ir_init(struct cling_polish_ir *self,  int temps,
                         struct utillib_json_value const *root) {
  self->temps=temps;
  utillib_vector_init(&self->stack);
  utillib_vector_init(&self->opstack);
  polish_ir_post_order(self, root);
}

void cling_polish_ir_destroy(struct cling_polish_ir *self) {
  utillib_vector_destroy(&self->stack);
  utillib_vector_destroy(&self->opstack);
}


