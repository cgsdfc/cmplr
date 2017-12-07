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

#include <utillib/json_foreach.h>

#include <stdlib.h>
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
polan_ir_node_json_object_create(struct utillib_json_value const*self) {
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

void cling_polan_ir_init(struct cling_polan_ir *self,
                         struct utillib_json_value const *root) {
  utillib_vector_init(&self->stack);
  polan_ir_post_order(self, root);
}

void cling_polan_ir_destroy(struct cling_polan_ir *self) {
  utillib_vector_destroy(&self->stack);
}


void cling_polan_ir_emit(struct cling_polan_ir *self,
    struct cling_ast_function *function)
{


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

static void ast_function_instr_push_back(struct cling_ast_function *self,
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

static struct cling_ast_ir *
emit_ir(int opcode) {
  struct cling_ast_ir *self=malloc(sizeof *self);
  self->opcode=opcode;
  return self;
}

static struct cling_ast_ir *
emit_defcon(int type, char const *name, char const *value) {
  struct cling_ast_ir *self=emit_ir(OP_DEFCON);
  self->operands[0].scalar=type;
  self->operands[1].text=name;
  self->operands[2].text=value;
  return self;
}

static struct cling_ast_ir *
emit_defvar(int type, char const* name, char const *extend) {
  struct cling_ast_ir *self=emit_ir(OP_DEFVAR);
  self->operands[0].scalar=type;
  self->operands[1].text=name;
  self->operands[2].text=extend;
  return self;
}

static struct cling_ast_ir *
emit_defunc(int type, char const *name) {
  struct cling_ast_ir *self=emit_ir(OP_DEFUNC);
  self->operands[0].scalar=type;
  self->operands[1].text=name;
  return self;
}

static struct cling_ast_ir *
emit_para(int type, char const *name) {
  struct cling_ast_ir *self=emit_ir(OP_PARA);
  self->operands[0].scalar=type;
  self->operands[1].text=name;
  return self;
}

static void emit_if_stmt(
    struct utillib_json_value const *self,
    struct cling_ast_function *func) {
  struct utillib_json_value *expr;
  struct cling_polan_ir expr_ir;
  expr=utillib_json_object_at(self, "expr");
  cling_polan_ir_init(&expr_ir, expr);
  cling_polan_ir_emit(&expr_ir, func);

}

static void emit_statement(
    struct utillib_json_value const *self,
    struct cling_ast_function *func) {

}

static void emit_var_decls(
    struct utillib_json_value const *self,
    struct cling_ast_function *func) {
  struct utillib_json_value const *defs, *decl;
  struct utillib_json_value  *extend, *name, *type;
  struct cling_ast_ir *ir;

  type=utillib_json_object_at(self, "type");
  defs=utillib_json_object_at(self, "var_defs");
  UTILLIB_JSON_ARRAY_FOREACH(decl, defs) {
    name=utillib_json_object_at(decl, "name");
    extend=utillib_json_object_at(decl, "value");
    if (extend)
      ir=emit_defvar(type->as_size_t, name->as_ptr, extend->as_ptr);
    else
      ir=emit_defvar(type->as_size_t, name->as_ptr, NULL);
      ast_function_instr_push_back(func, ir);
  }
}


static void emit_const_decls(
    struct utillib_json_value const *self, 
    struct cling_ast_function *func) {
  struct utillib_json_value const *defs, *decl;
  struct utillib_json_value  *value, *name, *type;

  type=utillib_json_object_at(self, "type");
  defs=utillib_json_object_at(self, "const_defs");
  UTILLIB_JSON_ARRAY_FOREACH(decl, defs) {
    name=utillib_json_object_at(decl, "name");
    value=utillib_json_object_at(decl, "value");
    ast_function_instr_push_back(func, 
        emit_defcon(type->as_size_t, name->as_ptr, value->as_ptr));
  }
}

static void maybe_emit_decls(
    struct utillib_json_value const *self,
    struct cling_ast_function *func) {
  struct utillib_json_value *const_decls, *var_decls;

  const_decls=utillib_json_object_at(self, "const_decls");
  var_decls=utillib_json_object_at(self, "var_decls");
  if (const_decls)
    emit_const_decls(const_decls,func );
  if (var_decls) 
    emit_var_decls(var_decls,func );
}

static void emit_composite(
    struct utillib_json_value const *self,
    struct cling_ast_function *func) {
  struct utillib_json_value const *object, *stmts;
  stmts=utillib_json_object_at(self, "stmts");
  maybe_emit_decls(self,func );
  UTILLIB_JSON_ARRAY_FOREACH(object, stmts) {
    emit_statement(object, func);
  }
}

/*
 * Create an cling_ast_function from the func_node.
 */
struct cling_ast_function *
cling_ast_function_create(struct utillib_json_value const* func_node) {
  struct cling_ast_function *self=malloc(sizeof *self);
  struct utillib_json_value *type=utillib_json_object_at(func_node, "type");
  struct utillib_json_value *name=utillib_json_object_at(func_node, "name");
  struct utillib_json_value *arglist=utillib_json_object_at(func_node, "arglist");
  struct utillib_json_value *comp=utillib_json_object_at(func_node, "comp");
  struct utillib_json_value const *arg;

  cling_ast_function_init(self, name->as_ptr);
  ast_function_instr_push_back(self,
      emit_defunc(type->as_size_t, name->as_ptr));

  UTILLIB_JSON_ARRAY_FOREACH(arg, arglist) {
    struct utillib_json_value * type, *name;
    type=utillib_json_object_at(arg, "type");
    name=utillib_json_object_at(arg, "name");
    ast_function_instr_push_back(self,
        emit_para(type->as_size_t, name->as_ptr));
  }
  emit_composite(comp, self);
  return self;
}

