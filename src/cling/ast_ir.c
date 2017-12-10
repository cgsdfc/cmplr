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
#include "ast.h"

#include <utillib/json_foreach.h>

#include <stdlib.h>
#include <assert.h>

static void emit_statement(
    struct utillib_json_value const *self,
    struct cling_ast_ir_global *global,
    struct utillib_vector *instrs);

static void emit_composite(
    struct utillib_json_value const *self,
    struct cling_ast_ir_global *global,
    struct utillib_vector *instrs);

static void emit_scanf_stmt(
    struct utillib_json_value const *self,
    struct cling_ast_ir_global *global,
    struct utillib_vector *instrs)
{
  struct utillib_json_value *value, *arglist;
  struct utillib_json_value const *object;
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
  struct utillib_json_value const*value, *str, *arglist;
  struct utillib_json_value const*type,*object;
  struct cling_ast_ir *ir;
  struct cling_polish_ir polish_ir;

  arglist=utillib_json_object_at(self, "arglist");

  UTILLIB_JSON_ARRAY_FOREACH(object, arglist) {
    type=utillib_json_object_at(object, "type");
    if (type && type->as_size_t == SYM_STRING) {
      value=utillib_json_object_at(object, "value");
      ir=emit_ir(OP_WRITE);
      ir->operands[0].text=value->as_ptr;
      ir->info[0]=CL_STRG;
    } else {
      cling_polish_ir_init(&polish_ir, object, global);
      cling_polish_ir_emit(&polish_ir, instrs);
      ir=emit_ir(OP_WRITE);
      cling_polish_ir_result(&polish_ir, ir, 0);
      cling_polish_ir_destroy(&polish_ir);
    }
    utillib_vector_push_back(instrs, ir);
  }
}

/*
 * An ordinary for-stmt is:
 * <init>
 * tricky_jump <body>
 * <cond>
 * cond_test: bez cond_expr <endfor>
 * <body>
 * <step>
 * loop_jump <cond>
 * <endfor>
 * 
 * Notes, tricky_jump is inserted
 * for special purpose, see below.
 */
static void emit_for_stmt(
    struct utillib_json_value const *self,
    struct cling_ast_ir_global *global,
    struct utillib_vector *instrs)
{
  struct utillib_json_value const*init, *cond, *step, *stmt;
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
  int loop_jump_jta, tricky_jump_jta;

  init = utillib_json_object_at(self, "init");
  cond = utillib_json_object_at(self, "cond");
  step = utillib_json_object_at(self, "step");
  stmt = utillib_json_object_at(self, "stmt");

  cling_polish_ir_init(&polish_ir, init,global );
  cling_polish_ir_emit(&polish_ir, instrs);
  tricky_jump=emit_ir(OP_JMP);
  utillib_vector_push_back(instrs, tricky_jump);
  /*
   * The JTA of loop_jump
   * is the next instr of the tricky_jump,
   * which is also the beginning of cond_test.
   */
  loop_jump_jta=utillib_vector_size(instrs);
  cling_polish_ir_destroy(&polish_ir);
  
  /*
   * cond.
   */
  cling_polish_ir_init(&polish_ir, cond,global );
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
  tricky_jump->operands[0].scalar=utillib_vector_size(instrs);
  cling_polish_ir_destroy(&polish_ir);

  /*
   * body
   */
  emit_statement(stmt, global, instrs);

  /*
   * step
   */
  cling_polish_ir_init(&polish_ir, step, global);
  cling_polish_ir_emit(&polish_ir, instrs);
  /*
   * The JTA of cond_test is the next instr 
   * of loop_jump. 
   */
  loop_jump=emit_ir(OP_JMP);
  loop_jump->operands[0].scalar=loop_jump_jta;
  utillib_vector_push_back(instrs, loop_jump);
  cond_test->operands[1].scalar=utillib_vector_size(instrs);

  /*
   * Clean up
   */
  cling_polish_ir_destroy(&polish_ir);
}

/*
 * A case_gaurd is the first instr of a
 * case clause. 
 * It is a OP_BNE taking the switch-expr and
 * case_label as operands to compare and its
 * BTA is the next case_gaurd.
 * The function creates a case_gaurd without
 * BTA which should be filled in later.
 */
/*
 * Ordinary switch-case statement
 * can be interpreted as:
 * <eval-switch-expr>
 * bne <case-label-1> <switch-expr> <next-bne>
 * <case-1-stmt>
 * break_jump <endswitch>
 * bne <case-label-2> <switch-expr> <default>
 * <case-2-stmt>
 * break_jump <endswitch>
 * <default>:
 * <default-stmt>
 * <endswitch>
 */
static struct cling_ast_ir *
emit_case_gaurd(size_t type, char const *value, 
    struct cling_polish_ir const* polish_ir) {
  struct cling_ast_ir *case_gaurd=emit_ir(OP_BNE);
  case_gaurd->operands[0].text=value;
  case_gaurd->info[0]=emit_wide(type) | CL_IMME;
  cling_polish_ir_result(polish_ir, case_gaurd, 1);
  return case_gaurd;
}

static void emit_switch_stmt(
    struct utillib_json_value const *self,
    struct cling_ast_ir_global *global,
    struct utillib_vector *instrs) 
{
  struct cling_polish_ir polish_ir;
  struct utillib_json_value const*object, *case_clause, *expr;
  struct utillib_json_value const*value, *case_, *stmt, *type;
  struct cling_ast_ir *case_gaurd, * break_jump; 
  /*
   * break_jumps keeps all the break_jump of
   * the case_clauses to fill their JTA to 
   * the end of switch_stmt later.
   */
  struct utillib_vector break_jumps;
  int break_jump_jta;

  utillib_vector_init(&break_jumps);
  expr=utillib_json_object_at(self, "expr");
  case_clause=utillib_json_object_at(self, "cases");
  cling_polish_ir_init(&polish_ir, expr, global);
  cling_polish_ir_emit(&polish_ir, instrs);

  UTILLIB_JSON_ARRAY_FOREACH(object, case_clause) {
    case_=utillib_json_object_at(object, "case");
    stmt=utillib_json_object_at(object, "stmt");
    if (case_)  {
      value=utillib_json_object_at(case_, "value");
      type=utillib_json_object_at(case_, "type");
      case_gaurd=emit_case_gaurd(type->as_size_t, value->as_ptr, &polish_ir);
      utillib_vector_push_back(instrs, case_gaurd);
      emit_statement(stmt, global, instrs);
      break_jump=emit_ir(OP_JMP);
      utillib_vector_push_back(instrs, break_jump);
      utillib_vector_push_back(&break_jumps, break_jump);
      /*
       * Fills in the BTA of case_gaurd, which
       * is the next instr of break_jump.
       */
      case_gaurd->operands[2].scalar=utillib_vector_size(instrs);
    } else {
      /*
       * default clause does not need a break_jump.
       */
      emit_statement(stmt, global, instrs);
      break_jump_jta=utillib_vector_size(instrs);
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
  struct utillib_json_value const*expr;
  struct cling_polish_ir polish_ir;

  expr=utillib_json_object_at(self, "expr");
  cling_polish_ir_init(&polish_ir, expr, global);
  cling_polish_ir_emit(&polish_ir, instrs);
  cling_polish_ir_destroy(&polish_ir);
}

static void emit_return_stmt(
    struct utillib_json_value const *self,
    struct cling_ast_ir_global *global,
    struct utillib_vector *instrs) 
{
  struct utillib_json_value const*expr;
  struct cling_polish_ir polish_ir;
  struct cling_ast_ir *ir;

  expr=utillib_json_object_at(self, "expr");
  ir=emit_ir(OP_RET);
  if (expr) {
    cling_polish_ir_init(&polish_ir, expr, global);
    cling_polish_ir_emit(&polish_ir, instrs);
    cling_polish_ir_result(&polish_ir, ir, 0);
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
  struct utillib_json_value const*expr, *then_clause, *else_clause;
  struct cling_polish_ir polish_ir;
  struct cling_ast_ir *skip_branch, *jump;

  expr=utillib_json_object_at(self, "expr");
  cling_polish_ir_init(&polish_ir, expr, global);
  cling_polish_ir_emit(&polish_ir, instrs);

  then_clause=utillib_json_object_at(self, "then");
  skip_branch=emit_ir(OP_BEZ);
  cling_polish_ir_result(&polish_ir, skip_branch, 0);
  utillib_vector_push_back(instrs, skip_branch);

  emit_statement(then_clause, global, instrs);
  else_clause=utillib_json_object_at(self, "else");
  if (else_clause) {
    jump=emit_ir(OP_JMP);
    utillib_vector_push_back(instrs, jump);
    skip_branch->operands[1].scalar=utillib_vector_size(instrs);
    emit_statement(else_clause, global, instrs);
    jump->operands[0].scalar=utillib_vector_size(instrs);
  } else {
    skip_branch->operands[1].scalar=utillib_vector_size(instrs);
  }
  cling_polish_ir_destroy(&polish_ir);
}

static void emit_statement(
    struct utillib_json_value const *self,
    struct cling_ast_ir_global *global,
    struct utillib_vector *instrs) {
  if (self == &utillib_json_null)
    return;
  struct utillib_json_value const*type=utillib_json_object_at(self, "type");
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
    case SYM_COMP_STMT:
      emit_composite(self, global, instrs);
      return;
    default:
      puts(cling_symbol_kind_tostring(type->as_size_t));
      assert(false);
  }
}

/*
 * Emits defvar ir.
 * defvar name(wide|scope) extend(is_array)
 */
static void emit_var_defs(
    struct utillib_json_value const *self,
    struct cling_ast_ir_global *global,
    struct utillib_vector *instrs) {
  struct utillib_json_value const *defs, *decl;
  struct utillib_json_value  *extend, *name, *type;
  struct cling_ast_ir *ir;
  int scope_bit;
  char const* extend_val;

  type=utillib_json_object_at(self, "type");
  defs=utillib_json_object_at(self, "var_defs");
  UTILLIB_JSON_ARRAY_FOREACH(decl, defs) {
    name=utillib_json_object_at(decl, "name");
    extend=utillib_json_object_at(decl, "extend");
    if (cling_symbol_table_exist_name(global->symbol_table,
          name->as_ptr, CL_GLOBAL))
      scope_bit=CL_GLBL;
    else
      scope_bit=CL_LOCL;
    if (extend)
      extend_val=extend->as_ptr;
    else
      extend_val=NULL;
    ir=emit_defvar(type->as_size_t, name->as_ptr,
        scope_bit, extend_val);
    utillib_vector_push_back(instrs, ir);
  }
}

/*
 * Emits defcon ir.
 * defcon name(wide|scope) value
 */
static void emit_const_defs(
    struct utillib_json_value const *self, 
    struct cling_ast_ir_global *global,
    struct utillib_vector *instrs) {
  struct utillib_json_value const *defs, *decl;
  struct utillib_json_value  const*value, *name, *type;
  int scope_bit;

  type=utillib_json_object_at(self, "type");
  defs=utillib_json_object_at(self, "const_defs");
  UTILLIB_JSON_ARRAY_FOREACH(decl, defs) {
    name=utillib_json_object_at(decl, "name");
    value=utillib_json_object_at(decl, "value");
    if (cling_symbol_table_exist_name(global->symbol_table,
          name->as_ptr, CL_GLOBAL))
      scope_bit=CL_GLBL;
    else
      scope_bit=CL_LOCL;
    utillib_vector_push_back(instrs, 
        emit_defcon(type->as_size_t, name->as_ptr, 
          scope_bit, value->as_ptr));
  }
}

/*
 * Wrapper around emit_const_defs
 * and emit_var_defs.
 * Notes it will insert const_decls and
 * var_decls into symbol_table.
 */
static void maybe_emit_decls(
    struct utillib_json_value const *self,
    struct cling_ast_ir_global *global,
    int scope_kind,
    struct utillib_vector *instrs) {
  struct utillib_json_value const*object, *const_decls, *var_decls;

  const_decls=utillib_json_object_at(self, "const_decls");
  var_decls=utillib_json_object_at(self, "var_decls");
  if (const_decls) {
    UTILLIB_JSON_ARRAY_FOREACH(object, const_decls) 
      cling_symbol_table_insert_const(global->symbol_table,object); 
      emit_const_defs(object , global, instrs);
  }

  if (var_decls) {
    UTILLIB_JSON_ARRAY_FOREACH(object,  var_decls) {
      cling_symbol_table_insert_variable(global->symbol_table,object);
      emit_var_defs(object,global,instrs );
    }
  }
}


static void emit_composite(
    struct utillib_json_value const *self,
    struct cling_ast_ir_global *global,
    struct utillib_vector *instrs)
{
  struct utillib_json_value const *object, *stmts;
  stmts=utillib_json_object_at(self, "stmts");
  maybe_emit_decls(self, global, CL_LOCAL, instrs);
  if (stmts) 
    /*
     * Watch out the empty case!
     */
    UTILLIB_JSON_ARRAY_FOREACH(object, stmts) {
      emit_statement(object, global, instrs);
    }
}

static void cling_ast_function_init(struct cling_ast_function *self, char const *name)
{
  self->name=strdup(name);
  self->temps=0;
  utillib_vector_init(&self->instrs);
}

static void cling_ast_function_destroy(struct cling_ast_function *self)
{
  free(self->name);
  utillib_vector_destroy_owning(&self->instrs, cling_ast_ir_destroy);
  free(self);
}

void cling_ast_program_init(struct cling_ast_program *self)
{
  utillib_vector_init(&self->init_code);
  utillib_vector_init(&self->funcs);
}

void cling_ast_program_destroy(struct cling_ast_program *self)
{
  utillib_vector_destroy_owning(&self->funcs, 
      cling_ast_function_destroy);
  utillib_vector_destroy_owning(&self->init_code,
      cling_ast_ir_destroy);
}
/*
 * Create an cling_ast_function from the func_node.
 */
static struct cling_ast_function *
cling_ast_ir_emit_function(struct utillib_json_value const* func_node, 
    struct cling_ast_ir_global *global) {
  struct cling_ast_function *self=malloc(sizeof *self);
  struct utillib_json_value const*type=utillib_json_object_at(func_node, "type");
  struct utillib_json_value const*name=utillib_json_object_at(func_node, "name");
  struct utillib_json_value const*arglist=utillib_json_object_at(func_node, "arglist");
  struct utillib_json_value const*comp=utillib_json_object_at(func_node, "comp");
  struct utillib_json_value const *arg;

  cling_ast_function_init(self, name->as_ptr);
  cling_symbol_table_insert_arglist(global->symbol_table,arglist); 
  utillib_vector_push_back(&self->instrs,
      emit_defunc(type->as_size_t, name->as_ptr));

  UTILLIB_JSON_ARRAY_FOREACH(arg, arglist) {
    struct utillib_json_value const* type, *name;
    type=utillib_json_object_at(arg, "type");
    name=utillib_json_object_at(arg, "name");
    utillib_vector_push_back(&self->instrs,
        emit_para(type->as_size_t, name->as_ptr));
  }
  emit_composite(comp, global, &self->instrs);
  utillib_vector_push_back(&self->instrs, emit_nop());
  return self;
}

static void ast_ir_global_init(struct cling_ast_ir_global *self,
    struct cling_symbol_table * symbol_table) {
  self->temps=0;
  self->symbol_table=symbol_table;
}

void cling_ast_ir_emit_program(
    struct utillib_json_value const *self,
    struct cling_symbol_table *symbol_table,
    struct cling_ast_program *program)
{
  struct utillib_json_value const*func_decls, *object;
  struct cling_ast_function *func;
  struct cling_ast_ir_global global;
  int temps;

  ast_ir_global_init(&global, symbol_table);
  /*
   * Enters these names to global-scope.
   */
  maybe_emit_decls(self, &global, CL_GLOBAL, 
      &program->init_code );
  func_decls=utillib_json_object_at(self, "func_decls");

  /*
   * All functions.
   */
  UTILLIB_JSON_ARRAY_FOREACH(object, func_decls) {
    temps=global.temps;
    cling_symbol_table_enter_scope(symbol_table);
    func=cling_ast_ir_emit_function(object, &global);
    cling_symbol_table_leave_scope(symbol_table);
    func->temps=global.temps-temps;
    utillib_vector_push_back(&program->funcs, func);
  }
}

void cling_ast_program_print(struct cling_ast_program const* self)
{
  struct cling_ast_function const* func;

  cling_ast_ir_print(&self->init_code);
  UTILLIB_VECTOR_FOREACH(func, &self->funcs) {
    cling_ast_ir_print(&func->instrs);
  }
}


