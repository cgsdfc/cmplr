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

#include <assert.h>
#include <utillib/json_foreach.h>

static void pretty_expr(struct utillib_json_value const *self,
                            struct utillib_string *string);

static void pretty_statement(struct utillib_json_value const *self,
                                 struct utillib_string *string,
                                 int indent_level);

static void pretty_composite_stmt(struct utillib_json_value const *self,
                                      struct utillib_string *string,
                                      int indent_level);

static void pretty_expr_stmt(struct utillib_json_value const *self,
                                 struct utillib_string *string,
                                 int indent_level);

static void pretty_name(struct utillib_json_value const *self,
                            struct utillib_string *string) {
  utillib_string_append(string, self->as_ptr);
}

static void pretty_type(struct utillib_json_value const *self,
                            struct utillib_string *string) {
  utillib_string_append(string, symbol_kind_tostring(self->as_size_t));
}

static void pretty_indent(struct utillib_string *string, int indent_level) {
  for (int i = 0; i < indent_level; ++i)
    utillib_string_append(string, "  ");
}
static void pretty_factor(struct utillib_json_value const *self,
                              struct utillib_string *string) {
  struct utillib_json_value *value;
  value = utillib_json_object_at(self, "value");
  utillib_string_append(string, value->as_ptr);
}

static void pretty_call(struct utillib_json_value const *self,
                            struct utillib_string *string) {
  struct utillib_json_value const *lhs, *rhs, *arg;
  lhs = utillib_json_object_at(self, "lhs");
  rhs = utillib_json_object_at(self, "rhs");
  pretty_factor(lhs, string);
  utillib_string_append(string, "(");
  if (utillib_json_array_size(rhs) == 0) {
    utillib_string_append(string, ")");
    return;
  }
  UTILLIB_JSON_ARRAY_FOREACH(arg, rhs) {
    pretty_expr(arg, string);
    utillib_string_append(string, ", ");
  }
  utillib_string_erase_last(string);
  utillib_string_replace_last(string, ')');
}

static void pretty_subscript(struct utillib_json_value const *self,
                                 struct utillib_string *string) {
  struct utillib_json_value const *lhs, *rhs;
  lhs = utillib_json_object_at(self, "lhs");
  rhs = utillib_json_object_at(self, "rhs");
  pretty_factor(lhs, string);
  utillib_string_append(string, "[");
  pretty_expr(rhs, string);
  utillib_string_append(string, "]");
}

static void pretty_binary(struct utillib_json_value const *self,
                              struct utillib_string *string) {
  struct utillib_json_value const *lhs, *rhs, *op;
  lhs = utillib_json_object_at(self, "lhs");
  rhs = utillib_json_object_at(self, "rhs");
  op = utillib_json_object_at(self, "op");

  pretty_expr(lhs, string);
  utillib_string_append(string, " ");
  pretty_type(op, string);
  utillib_string_append(string, " ");
  pretty_expr(rhs, string);
}

static void pretty_expr(struct utillib_json_value const *self,
                            struct utillib_string *string) {
  struct utillib_json_value const *op;
  op = utillib_json_object_at(self, "op");
  if (!op) {
    pretty_factor(self, string);
    return;
  }
  switch (op->as_size_t) {
  case SYM_RK:
    pretty_subscript(self, string);
    return;
  case SYM_RP:
    pretty_call(self, string);
    return;
  default:
    pretty_binary(self, string);
    return;
  }
}

static void pretty_array(
    struct utillib_json_value const *self, struct utillib_string *string,
    int indent_level,
    void (*array_tostring)(struct utillib_json_value const *array,
                           struct utillib_string *string, int indent_level)) {
  struct utillib_json_value const *object;

  UTILLIB_JSON_ARRAY_FOREACH(object, self) {
    array_tostring(object, string, indent_level);
    utillib_string_append(string, "\n");
  }
}
static void pretty_list_end(struct utillib_string *string, char end) {
  utillib_string_erase_last(string);
  utillib_string_replace_last(string, end);
}

static void pretty_string(struct utillib_json_value const *self,
                              struct utillib_string *string) {
  utillib_string_append(string, "\"");
  pretty_name(self, string);
  utillib_string_append(string, "\"");
}

static void pretty_char(struct utillib_json_value const *self,
                            struct utillib_string *string) {
  utillib_string_append(string, "\'");
  pretty_name(self, string);
  utillib_string_append(string, "\'");
}

static void pretty_printf_stmt(struct utillib_json_value const *self,
                                   struct utillib_string *string,
                                   int indent_level) {
  struct utillib_json_value const *value, *type, *arglist, *object;

  arglist = utillib_json_object_at(self, "arglist");
  pretty_indent(string, indent_level);
  utillib_string_append(string, "printf(");
  UTILLIB_JSON_ARRAY_FOREACH(object, arglist) {
    type = utillib_json_object_at(object, "type");
    if (type && type->as_size_t == SYM_STRING) {
      value = utillib_json_object_at(object, "value");
      pretty_string(value, string);
    } else {
      pretty_expr(object, string);
    }
    utillib_string_append(string, ", ");
  }
  pretty_list_end(string, ')');
}

static void pretty_scanf_stmt(struct utillib_json_value const *self,
                                  struct utillib_string *string,
                                  int indent_level) {
  struct utillib_json_value const *arglist, *name, *object;

  arglist = utillib_json_object_at(self, "arglist");
  pretty_indent(string, indent_level);
  utillib_string_append(string, "scanf(");
  UTILLIB_JSON_ARRAY_FOREACH(object, arglist) {
    name = utillib_json_object_at(object, "value");
    pretty_name(name, string);
    utillib_string_append(string, ", ");
  }
  pretty_list_end(string, ')');
}

static void pretty_clause(struct utillib_json_value const *self,
                              struct utillib_string *string, int indent_level) {
  struct utillib_json_value const *type;
  type = utillib_json_object_at(self, "type");
  if (type->as_size_t == SYM_COMP_STMT) {
    utillib_string_append(string, " ");
    pretty_statement(self, string, indent_level);
    utillib_string_append(string, " ");
  } else {
    utillib_string_append(string, "\n");
    pretty_statement(self, string, indent_level);
    utillib_string_append(string, "\n");
  }
}

static void pretty_for_stmt(struct utillib_json_value const *self,
                                struct utillib_string *string,
                                int indent_level) {
  struct utillib_json_value const *init, *cond, *step, *stmt;
  init = utillib_json_object_at(self, "init");
  cond = utillib_json_object_at(self, "cond");
  step = utillib_json_object_at(self, "step");
  stmt = utillib_json_object_at(self, "stmt");

  pretty_indent(string, indent_level);
  utillib_string_append(string, "for (");
  pretty_expr(init, string);
  utillib_string_append(string, "; ");
  pretty_expr(cond, string);
  utillib_string_append(string, "; ");
  pretty_expr(step, string);
  utillib_string_append(string, ")");
  pretty_clause(stmt, string, indent_level + 1);
}

static void pretty_case_label(struct utillib_json_value const *self,
                                  struct utillib_string *string) {
  struct utillib_json_value const *case_, *value;
  case_ = utillib_json_object_at(self, "case");
  if (case_) {
    value = utillib_json_object_at(case_, "value");
    utillib_string_append(string, "case ");
    pretty_name(value, string);
  } else {
    utillib_string_append(string, "default");
  }
  utillib_string_append(string, ":\n");
}

static void pretty_case_clause(struct utillib_json_value const *self,
                                   struct utillib_string *string,
                                   int indent_level) {
  struct utillib_json_value const *stmt;
  stmt = utillib_json_object_at(self, "stmt");
  pretty_indent(string, indent_level);
  pretty_case_label(self, string);
  pretty_statement(stmt, string, indent_level + 1);
}

static void pretty_switch_stmt(struct utillib_json_value const *self,
                                   struct utillib_string *string,
                                   int indent_level) {
  struct utillib_json_value const *expr, *cases;
  expr = utillib_json_object_at(self, "expr");
  cases = utillib_json_object_at(self, "cases");

  pretty_indent(string, indent_level);
  utillib_string_append(string, "switch (");
  pretty_expr(expr, string);
  utillib_string_append(string, ") {\n");
  pretty_array(cases, string, indent_level, pretty_case_clause);
  pretty_indent(string, indent_level);
  utillib_string_append(string, "}");
}

static void pretty_if_stmt(struct utillib_json_value const *self,
                               struct utillib_string *string,
                               int indent_level) {
  struct utillib_json_value const *expr, *then_clause, *else_clause;
  expr = utillib_json_object_at(self, "expr");
  then_clause = utillib_json_object_at(self, "then");

  pretty_indent(string, indent_level);
  utillib_string_append(string, "if (");
  pretty_expr(expr, string);
  utillib_string_append(string, ")");

  pretty_clause(then_clause, string, indent_level + 1);

  pretty_indent(string, indent_level);
  else_clause = utillib_json_object_at(self, "else");
  if (else_clause) {
    utillib_string_append(string, "else");
    pretty_clause(else_clause, string, indent_level + 1);
  }
}

static void pretty_expr_stmt(struct utillib_json_value const *self,
                                 struct utillib_string *string,
                                 int indent_level) {
  struct utillib_json_value *expr;
  expr = utillib_json_object_at(self, "expr");
  pretty_indent(string, indent_level);
  pretty_expr(expr, string);
}

static void pretty_return_stmt(struct utillib_json_value const *self,
                                   struct utillib_string *string,
                                   int indent_level) {
  struct utillib_json_value const *expr;

  pretty_indent(string, indent_level);
  utillib_string_append(string, "return");
  expr = utillib_json_object_at(self, "expr");
  if (expr) {
    utillib_string_append(string, " (");
    pretty_expr(expr, string);
    utillib_string_append(string, ")");
  }
}

static void pretty_statement(struct utillib_json_value const *self,
                                 struct utillib_string *string,
                                 int indent_level) {
  struct utillib_json_value const *type;
  if (self == &utillib_json_null) {
    pretty_indent(string, indent_level);
    goto append_semi;
  }
  type = utillib_json_object_at(self, "type");
  switch (type->as_size_t) {
  case SYM_SWITCH_STMT:
    pretty_switch_stmt(self, string, indent_level);
    return;
  case SYM_IF_STMT:
    pretty_if_stmt(self, string, indent_level);
    return;
  case SYM_FOR_STMT:
    pretty_for_stmt(self, string, indent_level);
    return;
  case SYM_EXPR_STMT:
    pretty_expr_stmt(self, string, indent_level);
    goto append_semi;
  case SYM_PRINTF_STMT:
    pretty_printf_stmt(self, string, indent_level);
    goto append_semi;
  case SYM_SCANF_STMT:
    pretty_scanf_stmt(self, string, indent_level);
    goto append_semi;
  case SYM_RETURN_STMT:
    pretty_return_stmt(self, string, indent_level);
    goto append_semi;
  case SYM_COMP_STMT:
    pretty_composite_stmt(self, string, indent_level);
    return;
  }
append_semi:
  utillib_string_append(string, ";");
}

static void pretty_single_var_decl(struct utillib_json_value const *self,
                                       struct utillib_string *string,
                                       int indent_level) {
  struct utillib_json_value const *type, *extend, *name, *var_defs, *object;

  type = utillib_json_object_at(self, "type");
  var_defs = utillib_json_object_at(self, "var_defs");

  pretty_indent(string, indent_level);
  pretty_type(type, string);
  utillib_string_append(string, " ");
  UTILLIB_JSON_ARRAY_FOREACH(object, var_defs) {
    name = utillib_json_object_at(object, "name");
    utillib_string_append(string, name->as_ptr);
    extend = utillib_json_object_at(object, "extend");
    if (extend) {
      utillib_string_append(string, "[");
      utillib_string_append(string, extend->as_ptr);
      utillib_string_append(string, "]");
    }
    utillib_string_append(string, ", ");
  }
  pretty_list_end(string, ';');
}

static void pretty_single_const_decl(struct utillib_json_value const *self,
                                         struct utillib_string *string,
                                         int indent_level) {
  struct utillib_json_value const *type, *const_defs, *name, *value, *object;

  type = utillib_json_object_at(self, "type");
  const_defs = utillib_json_object_at(self, "const_defs");

  pretty_indent(string, indent_level);
  utillib_string_append(string, "const ");
  pretty_type(type, string);
  utillib_string_append(string, " ");
  UTILLIB_JSON_ARRAY_FOREACH(object, const_defs) {
    name = utillib_json_object_at(object, "name");
    value = utillib_json_object_at(object, "value");
    pretty_name(name, string);
    utillib_string_append(string, " = ");
    if (type->as_size_t == SYM_KW_CHAR) {
      pretty_char(value, string);
    } else {
      pretty_name(value, string);
    }
    utillib_string_append(string, ", ");
  }
  pretty_list_end(string, ';');
}

static void pretty_maybe_decls(struct utillib_json_value const *self,
                                   struct utillib_string *string,
                                   int indent_level) {
  struct utillib_json_value const *const_decls, *var_decls;
  const_decls = utillib_json_object_at(self, "const_decls");
  var_decls = utillib_json_object_at(self, "var_decls");
  if (const_decls) {
    pretty_array(const_decls, string, indent_level,
                     pretty_single_const_decl);
    utillib_string_append(string, "\n");
  }
  if (var_decls) {
    pretty_array(var_decls, string, indent_level,
                     pretty_single_var_decl);
    utillib_string_append(string, "\n");
  }
}

static void pretty_composite_stmt(struct utillib_json_value const *self,
                                      struct utillib_string *string,
                                      int indent_level) {
  struct utillib_json_value const *stmts;

  stmts = utillib_json_object_at(self, "stmts");
  utillib_string_append(string, "{\n");
  pretty_maybe_decls(self, string, indent_level);
  if (stmts) {
    pretty_array(stmts, string, indent_level, pretty_statement);
  } else {
    utillib_string_append(string, "\n");
  }
  pretty_indent(string, indent_level - 1);
  utillib_string_append(string, "}");
}

static void pretty_arglist(struct utillib_json_value const *self,
                               struct utillib_string *string) {
  struct utillib_json_value const *type, *name, *object;

  utillib_string_append(string, "(");
  if (utillib_json_array_size(self) == 0) {
    utillib_string_append(string, ")");
    return;
  }
  UTILLIB_JSON_ARRAY_FOREACH(object, self) {
    type = utillib_json_object_at(object, "type");
    name = utillib_json_object_at(object, "name");
    pretty_type(type, string);
    utillib_string_append(string, " ");
    pretty_name(name, string);
    utillib_string_append(string, ", ");
  }
  pretty_list_end(string, ')');
}

static void pretty_function(struct utillib_json_value const *self,
                                struct utillib_string *string,
                                int indent_level) {
  struct utillib_json_value const *arglist, *type, *name, *comp;
  type = utillib_json_object_at(self, "type");
  name = utillib_json_object_at(self, "name");
  arglist = utillib_json_object_at(self, "arglist");
  comp = utillib_json_object_at(self, "comp");
  pretty_type(type, string);
  utillib_string_append(string, " ");
  pretty_name(name, string);
  pretty_arglist(arglist, string);
  utillib_string_append(string, "\n");
  pretty_composite_stmt(comp, string, indent_level + 1);
  utillib_string_append(string, "\n");
}

static void pretty_program(struct utillib_json_value const *self,
                               struct utillib_string *string) {
  struct utillib_json_value const *func_decls;
  func_decls = utillib_json_object_at(self, "func_decls");
  pretty_maybe_decls(self, string, 0);
  pretty_array(func_decls, string, 0, pretty_function);
}

char *ast_pretty_expr(struct utillib_json_value const *self) {
  struct utillib_string string;
  utillib_string_init(&string);
  pretty_expr(self, &string);
  return utillib_string_c_str(&string);
}

char *ast_pretty_program(struct utillib_json_value const *self) {
  struct utillib_string string;
  utillib_string_init(&string);
  pretty_program(self, &string);
  return utillib_string_c_str(&string);
}
