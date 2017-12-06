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
#include "ast_pretty.h"
#include "symbols.h"

#include <utillib/json_foreach.h>
#include <assert.h>

static void ast_pretty_expr(struct utillib_json_value const *self,
    struct utillib_string *string);

static void ast_pretty_statement(
    struct utillib_json_value const* self,
    struct utillib_string *string,
    int indent_level);

static void ast_pretty_composite_stmt(
    struct utillib_json_value const *self,
    struct utillib_string *string,
    int indent_level);

static void ast_pretty_expr_stmt(
    struct utillib_json_value const* self,
    struct utillib_string *string);

static void ast_pretty_factor(struct utillib_json_value const *self,
    struct utillib_string *string)
{
  struct utillib_json_value *value;
  value=utillib_json_object_at(self, "value");
  utillib_string_append(string, value->as_ptr);
}

static void ast_pretty_call(struct utillib_json_value const *self,
    struct utillib_string *string)
{
  struct utillib_json_value *lhs, *rhs;
  lhs=utillib_json_object_at(self, "lhs");
  rhs=utillib_json_object_at(self, "rhs");
  ast_pretty_factor(lhs, string);
  utillib_string_append(string, "(");
  if (utillib_json_array_size(rhs) == 0) {
    utillib_string_append(string, ")");
    return;
  }
  UTILLIB_JSON_ARRAY_FOREACH(arg, rhs) {
    ast_pretty_expr(arg, string);
    utillib_string_append(string, ", ");
  }
  utillib_string_erase_last(string);
  utillib_string_replace_last(string, ')');
}

static void ast_pretty_subscript(
    struct utillib_json_value const *self,
    struct utillib_string *string)
{
  struct utillib_json_value *lhs, *rhs;
  lhs=utillib_json_object_at(self, "lhs");
  rhs=utillib_json_object_at(self, "rhs");
  ast_pretty_factor(lhs, string);
  utillib_string_append(string, "[");
  ast_pretty_expr(rhs, string);
  utillib_string_append(string, "]");
}

static void ast_pretty_binary(
    struct utillib_json_value const *self,
    struct utillib_string *string)
{
  struct utillib_json_value *lhs, *rhs, *op;
  lhs=utillib_json_object_at(self, "lhs");
  rhs=utillib_json_object_at(self, "rhs");
  op=utillib_json_object_at(self, "op");

  ast_pretty_expr(lhs, string);
  utillib_string_append(string, " ");
  utillib_string_append(string, 
      cling_symbol_kind_tostring(op->as_size_t));
  utillib_string_append(string, " ");
  ast_pretty_expr(rhs, string);
}

static void ast_pretty_expr(struct utillib_json_value const *self,
    struct utillib_string *string)
{
  struct utillib_json_value *op;
  op=utillib_json_object_at(self, "op");
  if (!op) {
    ast_pretty_factor(self, string);
    return;
  }
  switch(op->as_size_t) {
  case SYM_RK:
    ast_pretty_subscript(self, string);
    return;
  case SYM_RP:
    ast_pretty_call(self, string);
    return;
  default:
    ast_pretty_binary(self, string);
    return;
  }
}

void cling_ast_pretty_expr(struct utillib_json_value const *self,
    struct utillib_string *string)
{
  utillib_string_init(string);
  ast_pretty_expr(self, string);
}

static void ast_pretty_array(
    struct utillib_json_value const *self,
    struct utillib_string *string,
    int indent_level,
    void (*array_tostring)(
      struct utillib_json_value const* array,
      struct utillib_string *string,
      int indent_level))
{
  UTILLIB_JSON_ARRAY_FOREACH(object, self) {
    array_tostring(object, string, indent_level);
    utillib_string_append(string, "\n");
  }
}
static void ast_pretty_list_end(
    struct utillib_string *string, char end)
{
  utillib_string_erase_last(string);
  utillib_string_replace_last(string, end);
}

static void ast_pretty_name(
    struct utillib_json_value const* self,
    struct utillib_string *string)
{
  utillib_string_append(string, self->as_ptr);
}

static void ast_pretty_type(
    struct utillib_json_value const* self,
    struct utillib_string *string)
{
  utillib_string_append(string, cling_symbol_kind_tostring(self->as_size_t));
}

static void ast_pretty_indent(
    struct utillib_string *string,
    int indent_level)
{
  for (int i=0; i<indent_level; ++i)
    utillib_string_append(string, "\t");
}

static void ast_pretty_string(
    struct utillib_json_value const *self,
    struct utillib_string *string)
{
  utillib_string_append(string, "\"");
  ast_pretty_name(self, string);
  utillib_string_append(string, "\"");
}

static void ast_pretty_printf_stmt(
    struct utillib_json_value const *self,
    struct utillib_string *string,
    int indent_level)
{
  struct utillib_json_value *value,*type, *arglist;
  arglist=utillib_json_object_at(self, "arglist");
  utillib_string_append(string, "printf(");
  UTILLIB_JSON_ARRAY_FOREACH(object, arglist) {
    type=utillib_json_object_at(object, "type");
    if (type->as_size_t == SYM_STRING) {
      value=utillib_json_object_at(object, "value");
      ast_pretty_string(value, string);
    } else {
      ast_pretty_expr(object, string);
    }
    utillib_string_append(string, ", ");
  }
  ast_pretty_list_end(string, ')');
}

static void ast_pretty_scanf_stmt(
    struct utillib_json_value const *self,
    struct utillib_string *string,
    int indent_level)
{
  struct utillib_json_value *arglist, *name;
  arglist=utillib_json_object_at(self, "arglist");
  utillib_string_append(string, "scanf(");
  UTILLIB_JSON_ARRAY_FOREACH(object, arglist) {
    name=utillib_json_object_at(object, "name");
    ast_pretty_name(name, string);
    utillib_string_append(string, ", ");
  }
  ast_pretty_list_end(string, ')');
}

static void ast_pretty_for_stmt(
    struct utillib_json_value const *self,
    struct utillib_string *string,
    int indent_level)
{
  struct utillib_json_value *init, *cond, *step, *stmt;
  init=utillib_json_object_at(self, "init");
  cond=utillib_json_object_at(self, "cond");
  step=utillib_json_object_at(self, "step");
  stmt=utillib_json_object_at(self, "stmt");
  utillib_string_append(string, "for (");
  ast_pretty_expr(init, string);
  utillib_string_append(string, "; ");
  ast_pretty_expr(cond, string);
  utillib_string_append(string, "; ");
  ast_pretty_expr(step, string);
  utillib_string_append(string, ")");
  ast_pretty_statement(stmt, string, indent_level+1);
}

static void ast_pretty_case_label(
    struct utillib_json_value const* self,
    struct utillib_string *string)
{
  struct utillib_json_value *case_, *value;
  case_=utillib_json_object_at(self, "case");
  if (case_) {
    value=utillib_json_object_at(case_, "value");
    utillib_string_append(string, "case ");
    ast_pretty_name(value, string);
  } else {
    utillib_string_append(string, "default");
  }
  utillib_string_append(string, ":\n");
}


static void ast_pretty_case_clause(
    struct utillib_json_value const *self,
    struct utillib_string *string,
    int indent_level)
{
  struct utillib_json_value *stmt;
  stmt=utillib_json_object_at(self, "stmt");
  ast_pretty_indent(string, indent_level);
  ast_pretty_case_label(self, string);
  ast_pretty_statement(stmt, string, indent_level+1);
  utillib_string_append(string, "\n");
}

static void ast_pretty_switch_stmt(
    struct utillib_json_value const *self,
    struct utillib_string *string,
    int indent_level)
{
  struct utillib_json_value *expr, *cases;
  expr=utillib_json_object_at(self, "expr");
  cases=utillib_json_object_at(self, "cases");

  utillib_string_append(string, "switch (");
  ast_pretty_expr(expr, string);
  utillib_string_append(string, ") {\n");
  ast_pretty_array(cases, string,
      indent_level, ast_pretty_case_clause);
  utillib_string_append(string, "}\n");
}

static void ast_pretty_if_stmt(
    struct utillib_json_value const* self,
    struct utillib_string *string,
    int indent_level)
{
  struct utillib_json_value *expr, *then_clause, *else_clause;
  expr=utillib_json_object_at(self, "expr");
  then_clause=utillib_json_object_at(self, "then");
  utillib_string_append(string, "if (");
  ast_pretty_expr(expr, string);
  utillib_string_append(string, ")");
  ast_pretty_statement(then_clause, string, indent_level+1);
  else_clause=utillib_json_object_at(self, "else");
  if (else_clause) {
    utillib_string_append(string, "else");
    ast_pretty_statement(else_clause, string, indent_level+1);
  }
}


static void ast_pretty_expr_stmt(
    struct utillib_json_value const* self,
    struct utillib_string *string)
{
  struct utillib_json_value *expr;
  expr=utillib_json_object_at(self, "expr");
  if (expr)
    ast_pretty_expr(expr, string);

}

static void ast_pretty_statement(
    struct utillib_json_value const* self,
    struct utillib_string *string,
    int indent_level)
{
  struct utillib_json_value *type;
  ast_pretty_indent(string, indent_level);
  type=utillib_json_object_at(self, "type");
  switch (type->as_size_t) {
  case SYM_EXPR_STMT:
    ast_pretty_expr_stmt(self, string);
    goto append_semi;
  case SYM_SWITCH_STMT:
    ast_pretty_switch_stmt(self, string, indent_level);
    break;
  case SYM_IF_STMT:
    ast_pretty_if_stmt(self, string, indent_level);
    break;
  case SYM_FOR_STMT:
    ast_pretty_for_stmt(self, string, indent_level);
    break;
  case SYM_PRINTF_STMT:
    ast_pretty_printf_stmt(self, string, indent_level);
    goto append_semi;
  case SYM_SCANF_STMT:
    ast_pretty_scanf_stmt(self, string, indent_level);
    goto append_semi;
  case SYM_COMP_STMT:
    ast_pretty_composite_stmt(self, string, indent_level);
    break;
  default:
    assert(false);
  }
append_semi:
  utillib_string_append(string, ";\n");
}

static void ast_pretty_single_var_decl(
    struct utillib_json_value const *self,
    struct utillib_string *string,
    int indent_level)
{
  struct utillib_json_value *type, *extend, *name, *var_defs;
  type=utillib_json_object_at(self, "type");
  var_defs=utillib_json_object_at(self, "var_defs");

  ast_pretty_indent(string,indent_level );
  ast_pretty_type(type, string);
  utillib_string_append(string, " ");
  UTILLIB_JSON_ARRAY_FOREACH(object, var_defs) {
    name=utillib_json_object_at(object, "name");
    utillib_string_append(string, name->as_ptr);
    extend=utillib_json_object_at(object, "extend");
    if (extend) {
      utillib_string_append(string, "[");
      utillib_string_append(string, extend->as_ptr);
      utillib_string_append(string, "]");
    }
    utillib_string_append(string, ", ");
  }
  ast_pretty_list_end(string, ';');
}

static void ast_pretty_single_const_decl(
    struct utillib_json_value const *self,
    struct utillib_string *string,
    int indent_level)
{
  struct utillib_json_value *type, *const_defs, *name, *value;
  type=utillib_json_object_at(self, "type");
  const_defs=utillib_json_object_at(self, "const_defs");

  ast_pretty_indent(string,indent_level );
  utillib_string_append(string, "const ");
  utillib_string_append(string, cling_symbol_kind_tostring(type->as_size_t));
  utillib_string_append(string, " ");
  UTILLIB_JSON_ARRAY_FOREACH(object, const_defs) {
    name=utillib_json_object_at(object, "name");
    value=utillib_json_object_at(object, "value");
    ast_pretty_name(name, string);
    utillib_string_append(string, " = ");
    utillib_string_append(string, value->as_ptr);
    utillib_string_append(string, ", ");
  }
  ast_pretty_list_end(string, ';');

}


static void ast_pretty_maybe_decls(
    struct utillib_json_value const *self,
    struct utillib_string *string,
    int indent_level)
{
  struct utillib_json_value *const_decls, *var_decls;
  const_decls=utillib_json_object_at(self, "const_decls");
  var_decls=utillib_json_object_at(self, "var_decls");
  if (const_decls) 
    ast_pretty_array(const_decls, string,
        indent_level, ast_pretty_single_const_decl);
  if (var_decls)
    ast_pretty_array(const_decls, string,
        indent_level, ast_pretty_single_var_decl);
}

static void ast_pretty_composite_stmt(
    struct utillib_json_value const *self,
    struct utillib_string *string,
    int indent_level)
{
  struct utillib_json_value *stmts;

  ast_pretty_maybe_decls(self, string, indent_level);
  stmts=utillib_json_object_at(self, "stmts");
  ast_pretty_array(stmts, string,
      indent_level, ast_pretty_statement);
}
  
static void ast_pretty_arglist(
    struct utillib_json_value const *self,
    struct utillib_string *string)
{
  struct utillib_json_value *type, *name;
  utillib_string_append(string, "(");
  if (utillib_json_array_size(self) == 0) {
    utillib_string_append(string, ")");
    return;
  }
  UTILLIB_JSON_ARRAY_FOREACH(object, self) {
    type=utillib_json_object_at(object, "type");
    name=utillib_json_object_at(object, "name");
    ast_pretty_type(type, string);
    utillib_string_append(string, " ");
    ast_pretty_name(name, string);
    utillib_string_append(string, ", ");
  }
  ast_pretty_list_end(string, ')');
}

static void ast_pretty_function(
    struct utillib_json_value const *self,
    struct utillib_string *string,
    int indent_level)
{
  struct utillib_json_value *arglist, *type, *name, *comp;
  type=utillib_json_object_at(self, "type");
  name=utillib_json_object_at(self, "name");
  arglist=utillib_json_object_at(self, "arglist");
  comp=utillib_json_object_at(self, "comp");
  ast_pretty_type(type, string);
  utillib_string_append(string, " ");
  ast_pretty_name(name, string);
  ast_pretty_arglist(arglist,string);
  utillib_string_append(string, "\n");
  ast_pretty_composite_stmt(comp, string, indent_level+1);
  utillib_string_append(string, "\n");
}

static void ast_pretty_program(
    struct utillib_json_value const *self,
    struct utillib_string *string)
{
  struct utillib_json_value *func_decls;
  func_decls=utillib_json_object_at(self, "func_decls");
  ast_pretty_maybe_decls(self, string, 0);
  ast_pretty_array(func_decls, string, 0, ast_pretty_function);
}

void cling_ast_pretty_program(struct utillib_json_value const *self,
  struct utillib_string* string)
{
  utillib_string_init(string);
  ast_pretty_program(self, string);
}
