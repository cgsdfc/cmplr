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
#include "polish_ir.h"

#include <utillib/json_foreach.h>

#include <stdlib.h>
#include <assert.h>

static void emit_statement(
    struct utillib_json_value const *self,
    struct cling_ast_ir_global *global,
    struct utillib_vector *instrs);

static void emit_scanf_stmt(
    struct utillib_json_value const *self,
    struct cling_ast_ir_global *global,
    struct utillib_vector *instrs)
{
  struct utillib_json_value *value, *arglist;
  struct utillib_json_value *object;
  struct cling_ast_ir *ir;

  arglist=utillib_json_object_at(self, "arglist");
  UTILLIB_JSON_ARRAY_FOREACH(object, arglist) {
    value=utillib_json_object_at(object, "value");
    ir=emit_read(value->as_ptr);
    utillib_vector_push_back(instrs, ir);
  }
}

static void emit_printf_stmt(
    struct utillib_json_value const *self,
    struct cling_ast_ir_global *global,
    struct utillib_vector *instrs)
{
  struct utillib_json_value *value, *str, *arglist;
  struct utillib_json_value *type,*object;
  struct cling_ast_ir *ir;
  struct cling_polish_ir polish_ir;

  arglist=utillib_json_object_at(self, "arglist");
  UTILLIB_JSON_ARRAY_FOREACH(object, arglist) {
    type=utillib_json_object_at(object, "type");
    if (type && type->as_size_t == SYM_STRING) {
      value=utillib_json_object_at(self, "value");
      ir=emit_ir(OP_WRITE);
      ir->operands[0].text=value->as_ptr;
      ir->info[0]=CL_STRG;
    } else {
      cling_polish_ir_init(&polish_ir, global->temps, object);
      cling_polish_ir_emit(&polish_ir, instrs);
      global->temps=polish_ir.temps;
      ir=emit_ir(OP_WRITE);
      cling_polish_ir_result(&polish_ir, ir, 0);
      cling_polish_ir_destroy(&polish_ir);
    }
    utillib_vector_push_back(instrs, ir);
  }
}

static void emit_for_stmt(
    struct utillib_json_value const *self,
    struct cling_ast_ir_global *global,
    struct utillib_vector *instrs)
{
  struct utillib_json_value *init, *cond, *step, *stmt;
  /*
   * Since the grammar asks for an abnormal interpretation
   * of the ordinary for-statement, we add one `tricky_jmp'
   * to fulfill its odd need. 
   * Essentially it asks us to execute the for-body once before
   * the for-cond is ever considered, which is achieved by inserting
   * an unconditional jump between for-init and for-cond jumping
   * directly to for-body. 
   * The JTA is refilled when the cond was emitted, so we keep a ref
   * to tricky_jmp.
   */
  struct cling_ast_ir *tricky_jump, *cond_test, *loop_jump;
  struct cling_polish_ir polish_ir;
  int loop_jump_jta, tricky_jump_jta, initial;

  initial=utillib_vector_size(instrs);
  init = utillib_json_object_at(self, "init");
  cond = utillib_json_object_at(self, "cond");
  step = utillib_json_object_at(self, "step");
  stmt = utillib_json_object_at(self, "stmt");

  cling_polish_ir_init(&polish_ir, global->temps, init);
  cling_polish_ir_emit(&polish_ir, instrs);
  tricky_jump=emit_ir(OP_JMP);
  utillib_vector_push_back(instrs, tricky_jump);
  global->temps=polish_ir.temps;
  /*
   * The JTA of loop_jump
   * is the next instr of the tricky_jump,
   * which is also the beginning of cond_test.
   */
  loop_jump_jta=utillib_vector_size(instrs)+1-initial;
  cling_polish_ir_destroy(&polish_ir);
  
  /*
   * cond.
   */
  cling_polish_ir_init(&polish_ir, global->temps, cond);
  cling_polish_ir_emit(&polish_ir, instrs);
  cond_test=emit_ir(OP_BEZ);
  /*
   * The result of cond is the judgement of cond_test.
   */
  cling_polish_ir_result(&polish_ir, cond_test, 0);
  utillib_vector_push_back(instrs, cond_test);
  /*
   * The JTA of tricky_jump is the next instr of
   * cond_test, which is also the beginning of
   * body.
   */
  tricky_jump->operands[0].scalar=utillib_vector_size(instrs)+1-initial;
  global->temps=polish_ir.temps;
  cling_polish_ir_destroy(&polish_ir);

  /*
   * body
   */
  emit_statement(stmt, global, instrs);

  /*
   * step
   */
  cling_polish_ir_init(&polish_ir, global->temps, step);
  cling_polish_ir_emit(&polish_ir, instrs);
  global->temps=polish_ir.temps;
  /*
   * The JTA of cond_test is the next instr 
   * of step, which goes out of the for-stmt.
   */
  cond_test->operands[0].scalar=utillib_vector_size(instrs)+1-initial;
  loop_jump=emit_ir(OP_JMP);
  loop_jump->operands[0].scalar=loop_jump_jta;
  utillib_vector_push_back(instrs, loop_jump);

  /*
   * Clean up
   */
  cling_polish_ir_destroy(&polish_ir);
}

static void emit_switch_stmt(
    struct utillib_json_value const *self,
    struct cling_ast_ir_global *global,
    struct utillib_vector *instrs) 
{
  struct cling_polish_ir polish_ir;
  struct utillib_json_value *object, *case_clause, *expr;
  struct utillib_json_value *value, *case_, *stmt, *type;
  struct cling_ast_ir *case_gaurd, * break_jump; 
  /*
   * break_jumps keeps all the break_jump of
   * the case_clauses to fill their JTA to 
   * the end of switch_stmt later.
   */
  struct utillib_vector break_jumps;
  const int initial=utillib_vector_size(instrs);
  int break_jump_jta;

  utillib_vector_init(&break_jumps);
  expr=utillib_json_object_at(self, "expr");
  case_clause=utillib_json_object_at(self, "cases");
  cling_polish_ir_init(&polish_ir, global->temps, expr);
  cling_polish_ir_emit(&polish_ir, instrs);

  UTILLIB_JSON_ARRAY_FOREACH(object, case_clause) {
    case_=utillib_json_object_at(object, "case");
    stmt=utillib_json_object_at(object, "stmt");
    if (case_)  {
      value=utillib_json_object_at(case_, "value");
      type=utillib_json_object_at(case_, "type");
      case_gaurd=emit_ir(OP_BNE);
      case_gaurd->operands[0].text=value->as_ptr;
      case_gaurd->info[0]=type->as_size_t;
      cling_polish_ir_result(&polish_ir, case_gaurd, 1);
      utillib_vector_push_back(instrs, case_gaurd);
      emit_statement(stmt, global, instrs);
      case_gaurd->operands[2].scalar=utillib_vector_size(instrs)-initial+1;
      break_jump=emit_ir(OP_JMP);
      utillib_vector_push_back(instrs, break_jump);
      utillib_vector_push_back(&break_jumps, break_jump);
    } else {
      /*
       * default clause does not need a break_jump.
       */
      emit_statement(stmt, global, instrs);
      break_jump_jta=utillib_vector_size(instrs)+1-initial;
    }
  }

  UTILLIB_VECTOR_FOREACH(break_jump, &break_jumps) {
    break_jump->operands[0].scalar=break_jump_jta;
  }

  utillib_vector_destroy(&break_jumps);
  cling_polish_ir_destroy(&polish_ir);
}

static void emit_expr_stmt(
    struct utillib_json_value const *self,
    struct cling_ast_ir_global *global,
    struct utillib_vector *instrs) 
{
  struct utillib_json_value *expr;
  struct cling_polish_ir polish_ir;

  expr=utillib_json_object_at(self, "expr");
  cling_polish_ir_init(&polish_ir, global->temps, expr);
  global->temps=polish_ir.temps;
  cling_polish_ir_emit(&polish_ir, instrs);
  cling_polish_ir_destroy(&polish_ir);
}

static void emit_return_stmt(
    struct utillib_json_value const *self,
    struct cling_ast_ir_global *global,
    struct utillib_vector *instrs) 
{
  struct utillib_json_value *expr;
  struct cling_polish_ir polish_ir;
  struct cling_ast_ir *ir;

  expr=utillib_json_object_at(self, "expr");
  ir=emit_ir(OP_RET);
  if (expr) {
    cling_polish_ir_init(&polish_ir, global->temps, expr);
    cling_polish_ir_emit(&polish_ir, instrs);
    cling_polish_ir_result(&polish_ir, ir, 0);
    global->temps=polish_ir.temps;
    cling_polish_ir_destroy(&polish_ir);
  } else {
    ir->info[0]=CL_NULL;
  }
  utillib_vector_push_back(instrs, ir);
}



static void emit_if_stmt(
    struct utillib_json_value const *self,
    struct cling_ast_ir_global *global,
    struct utillib_vector *instrs) 
{
  struct utillib_json_value *expr, *then_clause, *else_clause;
  struct cling_polish_ir polish_ir;
  struct utillib_vector else_ir, then_ir;
  struct cling_ast_ir *ir;

  expr=utillib_json_object_at(self, "expr");
  cling_polish_ir_init(&polish_ir, global->temps, expr);
  cling_polish_ir_emit(&polish_ir, instrs);
  global->temps=polish_ir.temps;

  then_clause=utillib_json_object_at(self, "then");
  utillib_vector_init(&then_ir);
  emit_statement(then_clause, global, &then_ir);
  ir=emit_ir(OP_BNZ);
  cling_polish_ir_result(&polish_ir, ir, 0);
  ir->operands[1].scalar=utillib_vector_size(&then_ir)+1;
  utillib_vector_push_back(instrs, ir);

  else_clause=utillib_json_object_at(self, "else");
  if (else_clause) {
    utillib_vector_init(&else_ir);
    emit_statement(self, global, &else_ir);
    /*
     * Jump over else_clause directly after
     * then_clause
     */
    ir=emit_ir(OP_JMP);
    ir->operands[0].scalar=utillib_vector_size(&else_ir)+1;
    utillib_vector_push_back(&then_ir , ir);
  }
  utillib_vector_append(instrs, &then_ir);
  utillib_vector_destroy(&then_ir);
  if (else_clause) {
    utillib_vector_append(instrs, &else_ir);
    utillib_vector_destroy(&else_ir);
  }
  cling_polish_ir_destroy(&polish_ir);
}

static void emit_statement(
    struct utillib_json_value const *self,
    struct cling_ast_ir_global *global,
    struct utillib_vector *instrs) {
  if (self == &utillib_json_null)
    return;
  struct utillib_json_value *type=utillib_json_object_at(self, "type");
  switch(type->as_size_t) {
    case SYM_PRINTF_STMT:
      emit_printf_stmt(self, global, instrs);
      return;
    case SYM_SCANF_STMT:
      emit_scanf_stmt(self, global, instrs);
      return;
    case SYM_FOR_STMT:
      emit_for_stmt(self, global, instrs);
      return;
    case SYM_IF_STMT:
      emit_if_stmt(self, global, instrs);
      return;
    case SYM_SWITCH_STMT:
      emit_switch_stmt(self, global, instrs);
      return;
    case SYM_RETURN_STMT:
      emit_return_stmt(self, global, instrs);
      return;
    case SYM_EXPR_STMT:
      emit_expr_stmt(self, global, instrs);
      return;
    default:
      assert(false);
  }
}

static void emit_var_decls(
    struct utillib_json_value const *self,
    struct utillib_vector *instrs) {
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
      utillib_vector_push_back(instrs, ir);
  }
}


static void emit_const_decls(
    struct utillib_json_value const *self, 
    struct utillib_vector *instrs) {
  struct utillib_json_value const *defs, *decl;
  struct utillib_json_value  *value, *name, *type;

  type=utillib_json_object_at(self, "type");
  defs=utillib_json_object_at(self, "const_defs");
  UTILLIB_JSON_ARRAY_FOREACH(decl, defs) {
    name=utillib_json_object_at(decl, "name");
    value=utillib_json_object_at(decl, "value");
    utillib_vector_push_back(instrs, 
        emit_defcon(type->as_size_t, name->as_ptr, value->as_ptr));
  }
}

static void maybe_emit_decls(
    struct utillib_json_value const *self,
    struct utillib_vector *instrs) {
  struct utillib_json_value *const_decls, *var_decls;

  const_decls=utillib_json_object_at(self, "const_decls");
  var_decls=utillib_json_object_at(self, "var_decls");
  if (const_decls)
    emit_const_decls(const_decls,instrs );
  if (var_decls) 
    emit_var_decls(var_decls,instrs );
}


static void emit_composite(
    struct utillib_json_value const *self,
    struct cling_ast_ir_global *global,
    struct utillib_vector *instrs)
{
  struct utillib_json_value const *object, *stmts;
  stmts=utillib_json_object_at(self, "stmts");
  maybe_emit_decls(self, instrs);
  UTILLIB_JSON_ARRAY_FOREACH(object, stmts) {
    emit_statement(object, global, instrs);
  }
}

/*
 * Create an cling_ast_function from the func_node.
 */
struct cling_ast_function *
cling_ast_ir_emit_function(struct utillib_json_value const* func_node, 
    struct cling_ast_ir_global *global) {
  struct cling_ast_function *self=malloc(sizeof *self);
  struct utillib_json_value *type=utillib_json_object_at(func_node, "type");
  struct utillib_json_value *name=utillib_json_object_at(func_node, "name");
  struct utillib_json_value *arglist=utillib_json_object_at(func_node, "arglist");
  struct utillib_json_value *comp=utillib_json_object_at(func_node, "comp");
  struct utillib_json_value const *arg;

  cling_ast_function_init(self, name->as_ptr);
  cling_ast_function_push_back(self,
      emit_defunc(type->as_size_t, name->as_ptr));

  UTILLIB_JSON_ARRAY_FOREACH(arg, arglist) {
    struct utillib_json_value * type, *name;
    type=utillib_json_object_at(arg, "type");
    name=utillib_json_object_at(arg, "name");
    utillib_vector_push_back(&self->instrs,
        emit_para(type->as_size_t, name->as_ptr));
  }
  emit_composite(comp, global, &self->instrs);
  return self;
}

static void ast_ir_global_init(struct cling_ast_ir_global *self) {
  self->temps=0;
  self->instrs=0;
}

void cling_ast_ir_emit_program(
    struct utillib_json_value const *self,
    struct cling_ast_program *program)
{
  struct utillib_json_value *func_decls, *object;
  struct cling_ast_function *func;
  struct cling_ast_ir_global global;

  ast_ir_global_init(&global);
  cling_ast_program_init(program);
  func_decls=utillib_json_object_at(self, "func_decls");
  maybe_emit_decls(self, &program->init_code);
  UTILLIB_JSON_ARRAY_FOREACH(object, func_decls) {
    func=cling_ast_ir_emit_function(object, &global);
    utillib_vector_push_back(&program->funcs, func);
  }
}

