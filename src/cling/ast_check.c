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

static void cling_ast_unreachable(size_t code) {
  puts(cling_symbol_kind_tostring(code));
  assert(false);
}
static int ast_check_subscript(struct utillib_json_value *self,
                               struct cling_rd_parser *parser,
                               struct utillib_token_scanner *input,
                               size_t context);

static int ast_check_factor(struct utillib_json_value *self,
    struct cling_rd_parser *parser,
    struct utillib_token_scanner *input,
    size_t context,
    struct cling_symbol_entry **entry);

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

/*
 * Assignment.lhs:
 * Makes sure lhs of an assignment is a left value.
 * If it is an array subscription, return the type of element
 * if the array is defined or undef if it is undefined.
 * If it is a single variable, return its type if it is defined
 * or undefined if it is not.
 */
static int ast_check_assign_lhs(struct utillib_json_value *self, 
    struct cling_rd_parser *parser,
    struct utillib_token_scanner *input,
    size_t context)
{
  struct utillib_json_value *op;
  struct cling_symbol_entry *entry;
  int lhs_type;

  op=utillib_json_object_at(self, "op");
  if (!op) {
    /*
     * Single variable.
     * A single array without subscription is **not** assignable.
     *
     */
    lhs_type=ast_check_factor(self, parser, input, context, &entry);
    if (lhs_type == CL_UNDEF) 
      return CL_UNDEF;
    if (lhs_type & CL_FUNC || lhs_type & CL_ARRAY || lhs_type & CL_CONST)
      return CL_UNDEF;
    return lhs_type & (CL_INT | CL_CHAR);
  }
  if (op->as_size_t != SYM_RK) {
    /*
     * Although it is not assignable, we still need to check it.
     */
    cling_ast_check_expression(self, parser, input, context);
    return CL_UNDEF;
  }
  lhs_type=ast_check_subscript(self, parser, input, context);
  if (lhs_type == CL_UNDEF)
    return CL_UNDEF;
  return lhs_type;
}

/*
 * Integral means char, int or unsigned.
 * Being compatible to ingegral means it can be used
 * in subscription and function arguments.
 */
static bool ast_integral_compatible(int kind) {
  assert (kind != CL_UNDEF);
  if (kind & CL_ARRAY || kind & CL_FUNC || kind & CL_VOID)
    return false;
  if (kind & CL_INT || kind & CL_CHAR)
    return true;
  return false;
}

/*
 * If self is `SYM_IDEN' and exists
 * in symbol_table, the symbol_entry of it
 * will be returned.
 */
static int ast_check_factor(struct utillib_json_value *self,
    struct cling_rd_parser *parser,
    struct utillib_token_scanner *input,
    size_t context,
    struct cling_symbol_entry **entry) 
{
  struct utillib_json_value *value, *type;
  char const *name;
  value = utillib_json_object_at(self, "value");
  type = utillib_json_object_at(self, "type");

  switch (type->as_size_t) {
    case SYM_IDEN:
      name = value->as_ptr;
      *entry = cling_symbol_table_find(parser->symbol_table,
          name, CL_LEXICAL);
      if (!*entry) {
        rd_parser_error_push_back(parser,
            cling_undefined_name_error(input, name, context));
        return CL_UNDEF;
      }
      return (*entry)->kind;
    case SYM_CHAR:
      return CL_CHAR;
    case SYM_UINT:
    case SYM_INTEGER:
      return CL_INT;
    default:
      cling_ast_unreachable(type->as_size_t);
  }
}


/*
 * Subscription:
 * int or char [whatever] = int or char
 * undef[whatever] = undef
 */
static int ast_check_subscript(struct utillib_json_value *self,
                               struct cling_rd_parser *parser,
                               struct utillib_token_scanner *input,
                               size_t context)
{
  struct utillib_json_value *lhs, *rhs;
  int lhs_type, rhs_type;
  struct cling_symbol_entry *entry;
  char const *name;

  lhs=utillib_json_object_at(self, "lhs");
  rhs=utillib_json_object_at(self, "rhs");
  lhs_type=ast_check_factor(lhs, parser, input, context, &entry);
  if (lhs_type == CL_UNDEF) {
    /*
     * This array is undefined.
     */
     goto check_index_only;
  }
  if (!(lhs_type & CL_ARRAY)) {
    /*
     * This is not an array.
     */
    rd_parser_error_push_back(parser, 
        cling_incompatible_type_error(input, lhs_type, CL_ARRAY, context));
    goto check_index_only;
  }

  rhs_type=cling_ast_check_expression(rhs, parser, input, context);
  if (rhs_type != CL_UNDEF && !ast_integral_compatible(rhs_type)) {
    rd_parser_error_push_back(parser,
        cling_incompatible_type_error(input, rhs_type, CL_INT, context));
  }
  return entry->kind & (~CL_ARRAY);

check_index_only:
  cling_ast_check_expression(rhs, parser, input, context);
  return CL_UNDEF;

}

/*
 * Assumes "op" == SYM_RP
 * Call:
 * undef(whatever) = undef
 * defined(whatever) = defined
 */
static int ast_check_call(struct utillib_json_value *self,
                          struct cling_rd_parser *parser,
                          struct utillib_token_scanner *input, size_t context) {
  struct utillib_json_value *callee, *array;
  struct utillib_json_value *arglist, *formal_arg_type;
  struct utillib_json_value *formal_arg, *actual_arg;
  char const *func_name;
  int lhs_type, arg_type;
  struct cling_symbol_entry *entry;
  size_t formal_argc, actual_argc;
  int i, j, actual_arg_type;

  callee = utillib_json_object_at(self, "lhs");
  func_name=callee->as_ptr;
  array = utillib_json_object_at(self, "rhs");
  lhs_type = ast_check_factor(callee, parser, input, context, &entry);
  if (lhs_type == CL_UNDEF) {
    /*
     * The callee is undefined.
     */
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
    if (actual_arg_type == CL_UNDEF)
      continue;
    if (ast_integral_compatible(actual_arg_type))
      continue;
      rd_parser_error_push_back(parser,
          cling_incompatible_arg_error(input, i, formal_arg_type->as_size_t,
                                       actual_arg_type, context));
  }
  if (i != j) {
    rd_parser_error_push_back(
        parser, cling_argc_unmatched_error(input, func_name, actual_argc,
                                           formal_argc, context));
  }
  return entry->kind & (~CL_FUNC);

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

/*
 * lhs_type=ast_check_operand(self, "lhs", ...);
 * Filter out the undefined iden and not integral
 * guys all to be CL_UNDEF and pushes error properly.
 */
static int ast_check_operand(struct utillib_json_value *self,
    char const *operand,
    struct cling_rd_parser *parser,
    struct utillib_token_scanner *input,
    size_t context) 
{
  struct cling_symbol_entry *entry;
  char const *name;
  int type;
  struct utillib_json_value *object;

  object=utillib_json_object_at(self, operand);
  type=cling_ast_check_expression(object, parser, input, context);
  if (type == CL_UNDEF)
    return CL_UNDEF;
  if (!ast_integral_compatible(type)) {
    rd_parser_error_push_back(parser,
        cling_incompatible_type_error(input, CL_INT, type, context));
    return CL_UNDEF;
  }
  return type;
}
/*
 * Arithmetic:
 * char + char = char
 * int + int or char = int
 * undef + int or char = int or char
 * undef + undef = undef
 */
static int ast_check_arithmetic(struct utillib_json_value *self,
                               struct cling_rd_parser *parser,
                               struct utillib_token_scanner *input,
                               size_t context) {
  int lhs_type, rhs_type;
  lhs_type=ast_check_operand(self, "lhs", parser, input, context);
  rhs_type=ast_check_operand(self, "rhs", parser, input, context);
  if (lhs_type == CL_CHAR && rhs_type == CL_CHAR)
    return CL_CHAR;
  if (lhs_type == CL_INT || rhs_type == CL_INT)
    return CL_INT;
  if (lhs_type == CL_CHAR || rhs_type == CL_CHAR)
    return CL_CHAR;
  return CL_UNDEF;
}

/*
 * Boolean
 * int or char != int or char = int
 * undef != int or char = int
 * undef != undef = undef
 */
static int ast_check_boolean(struct utillib_json_value *self,
                               struct cling_rd_parser *parser,
                               struct utillib_token_scanner *input,
                               size_t context) {
  int lhs_type, rhs_type;
  lhs_type=ast_check_operand(self, "lhs", parser, input, context);
  rhs_type=ast_check_operand(self, "rhs", parser, input, context);
  if (lhs_type != CL_UNDEF || rhs_type != CL_UNDEF)
    return CL_INT;
  return CL_UNDEF;
}

/*
 * Assignment:
 * When array subscription appears on the left-hand-side
 * it become assignable.
 * int = int = int
 * char = char = char
 * undef = char or int = rhs
 * undef = undef = undef
 */
static int cling_ast_check_assign(struct utillib_json_value *self,
                               struct cling_rd_parser *parser,
                               struct utillib_token_scanner *input,
                               size_t context) {
  int lhs_type=ast_check_assign_lhs(self, parser, input, context);
  int rhs_type=ast_check_operand(self, "rhs", parser, input, context);
  if (lhs_type != CL_UNDEF)
    return lhs_type;
  if (rhs_type != CL_UNDEF)
    return rhs_type;
  return CL_UNDEF;
}

/*
 * Walks the expression tree
 * and checks for bad guys.
 */
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
    return ast_check_factor(self, parser, input, context, &entry);
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
  case SYM_ADD:
  case SYM_MINUS:
  case SYM_MUL:
  case SYM_DIV:
    /*
     * arithmetic
     */
    return ast_check_arithmetic(self, parser, input, context);
  case SYM_LE:
  case SYM_DEQ:
  case SYM_NE:
  case SYM_LT:
  case SYM_GT:
  case SYM_GE:
    /*
     * boolean
     */
    return ast_check_boolean(self, parser, input, context);
  case SYM_EQ:
    /*
     * assignment
     */
    return cling_ast_check_assign(self, parser, input, context);
  default:
    cling_ast_unreachable(op->as_size_t);
  }
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
