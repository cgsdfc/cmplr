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

#include "ast_check.h"
#include "error.h"
#include "symbol_table.h"
#include "symbols.h"

#include <assert.h>
#include <utillib/json_foreach.h>

int cling_ast_check_assignable(char const *name,
                               struct cling_symbol_table const *symbol_table) {
  struct cling_symbol_entry *entry;
  int kind;

  entry = cling_symbol_table_find(symbol_table, name, CL_LEXICAL);
  if (!entry)
    return CL_EUNDEFINED;
  kind = entry->kind;
  if (kind & CL_FUNC || kind & CL_ARRAY || kind & CL_CONST)
    return CL_ENOTLVALUE;
  return 0;
}

static int ast_check_factor(struct utillib_json_value *self,
                            struct cling_symbol_table *symbol_table,
                            /*
                             * If self is `SYM_IDEN' and exists
                             * in symbol_table, the symbol_entry of it
                             * will be returned.
                             */
                            struct cling_symbol_entry **entry,
                            char const **name) {
  struct utillib_json_value *value, *type;
  value = utillib_json_object_at(self, "value");
  type = utillib_json_object_at(self, "type");
  switch (type->as_size_t) {
  case SYM_IDEN:
    *name = value->as_ptr,
    *entry = cling_symbol_table_find(symbol_table, *name, CL_LEXICAL);
    if (!*entry) {
      return CL_UNDEF;
    }
    return (*entry)->kind;
  case SYM_CHAR:
    return CL_CHAR;
  case SYM_UINT:
  case SYM_INTEGER:
    return CL_INT;
  default:
    puts(cling_symbol_kind_tostring(type->as_size_t));
    assert(false);
  }
}

static int ast_check_subscript(struct utillib_json_value *self,
                               struct cling_rd_parser *parser,
                               struct utillib_token_scanner *input,
                               size_t context) {}

/*
 * Assumes "op" == SYM_RP
 */
static int ast_check_call(struct utillib_json_value *self,
                          struct cling_rd_parser *parser,
                          struct utillib_token_scanner *input, size_t context) {
  struct utillib_json_value *callee, *array, *return_type;
  struct utillib_json_value *arglist, *formal_arg_type;
  struct utillib_json_value *formal_arg, *actual_arg;
  char const *func_name, *arg_name;
  int lhs_type, arg_type;
  struct cling_symbol_entry *entry;
  size_t formal_argc, actual_argc;
  int i, j, actual_arg_type;

  callee = utillib_json_object_at(self, "lhs");
  array = utillib_json_object_at(self, "rhs");
  lhs_type = ast_check_factor(callee, parser->symbol_table, &entry, &func_name);
  if (lhs_type == CL_UNDEF) {
    /*
     * The callee is undefined.
     */
    rd_parser_error_push_back(
        parser, cling_undefined_name_error(input, func_name, context));
    goto check_args_only;
  }
  if (!(lhs_type & CL_FUNC)) {
    /*
     * The callee is not callable.
     */
    rd_parser_error_push_back(parser, cling_incompatible_type_error(
                                          input, lhs_type, CL_FUNC, context));
    goto check_args_only;
  }
  arglist = utillib_json_object_at(entry->value, "arglist");
  if (!arglist)
    /*
     * Due to syntax error, arglist is missing.
     */
    goto check_args_only;
  formal_argc = utillib_json_array_size(arglist);
  actual_argc = utillib_json_array_size(array);

  for (i = 0, j = 0; i < formal_argc && j < actual_argc; ++i, ++j) {
    /*
     * 1. computes the type of each argument.
     * 2. compares it with the corresponding formal argument.
     */
    formal_arg = utillib_json_array_at(arglist, i);
    actual_arg = utillib_json_array_at(array, j);
    formal_arg_type = utillib_json_object_at(formal_arg, "type");
    actual_arg_type =
        cling_ast_check_expression(actual_arg, parser, input, context);
    switch (actual_arg_type) {
    case CL_UNDEF:
      continue;
    case CL_VOID:
      /*
       * Passing void to a function.
       * void foo() {}
       * int bar(int i) {}
       * void main() { bar(foo()); }
       */
      rd_parser_error_push_back(
          parser,
          cling_incompatible_arg_error(input, i, formal_arg_type->as_size_t,
                                       actual_arg_type, context));
      break;
    default:
      /*
       * Since char=>int and int=>char is ok,
       * we ignore it here.
       */
      break;
    }
  }
  if (i != j) {
    rd_parser_error_push_back(
        parser, cling_argc_unmatched_error(input, func_name, actual_argc,
                                           formal_argc, context));
  }
  return_type = utillib_json_object_at(self, "type");
  return return_type->as_size_t;

check_args_only:
  /*
   * The foreach macro declares a variable,
   * so to get rid of "a label can only be
   * part of a statement"...
   */
  do {
    UTILLIB_JSON_ARRAY_FOREACH(arg, array) {
      /*
       * Since The name or the arglist of the callee
       * is missing, we can only check those subexpr.
       */
      cling_ast_check_expression(arg, parser, input, context);
    }
  } while (0);
  return CL_UNDEF;
}

int cling_ast_check_expression(struct utillib_json_value *self,
                               struct cling_rd_parser *parser,
                               struct utillib_token_scanner *input,
                               size_t context) {
  struct utillib_json_value *lhs, *rhs, *op;
  char const *name;
  int lhs_type, rhs_type, type;
  struct cling_symbol_entry *entry;

  op = utillib_json_object_at(self, "op");
  if (!op) {
    /*
     * A factor does not have "op"
     */
    type = ast_check_factor(self, parser->symbol_table, &entry, &name);
    if (type == CL_UNDEF) {
      rd_parser_error_push_back(
          parser, cling_undefined_name_error(input, name, context));
    }
    return type;
  }
  /*
   * Now check for binary operator.
   *
   */
  switch (op->as_size_t) {
  case SYM_RP:
    /*
     * call_expr.
     */
    return ast_check_call(self, parser, input, context);
  case SYM_RK:
    /*
     * subscript_expr
     */
    return ast_check_subscript(self, parser, input, context);
  }

  lhs = utillib_json_object_at(self, "lhs");
  lhs_type = cling_ast_check_expression(lhs, parser, input, context);
  if (lhs_type == CL_UNDEF)
    return CL_UNDEF;
  rhs = utillib_json_object_at(self, "rhs");
  rhs_type = cling_ast_check_expression(rhs, parser, input, context);
  if (rhs_type == CL_UNDEF)
    return CL_UNDEF;

  return 0;
}

int cling_ast_check_returnness(struct cling_rd_parser const *parser,
                               int expr_type, int *return_type) {
  assert(parser->curfunc);
  struct cling_symbol_entry *entry;
  int kind;

  entry =
      cling_symbol_table_find(parser->symbol_table, parser->curfunc, CL_GLOBAL);
  assert(entry);
  kind = entry->kind;
  assert(kind & CL_FUNC);
  *return_type = kind & (~CL_FUNC);
  if (*return_type == expr_type)
    return 0;

  switch (expr_type) {
  case CL_VOID:
    return CL_RET_GARBAGE;
  case CL_INT:
  case CL_CHAR:
    if (*return_type == CL_VOID)
      return CL_RET_DISCARD;
    return 0;
  default:
    assert(false);
  }
}
