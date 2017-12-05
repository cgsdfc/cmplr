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

/*
 * Semantic checking for Expressions.
 */

/*
 * Subscription:
 * int or char [defined] = int or char
 * Otherwise undef
 */
static int ast_check_subscript(struct utillib_json_value *self,
                               struct cling_rd_parser *parser,
                               struct utillib_token_scanner *input,
                               size_t context);

static int ast_check_expression(struct utillib_json_value *self,
                               struct cling_rd_parser *parser,
                               struct utillib_token_scanner *input,
                               size_t context);

/*
 * Factors are SYM_IDEN, SYM_INTEGER, SYM_UINT, SYM_CHAR.
 * If self is `SYM_IDEN' and exists
 * in symbol_table, the symbol_entry of it
 * will be returned together with its kind.
 * If self is other things, the corresponding CL_XXX is returned.
 */
static int ast_check_factor(struct utillib_json_value *self,
    struct cling_rd_parser *parser,
    struct utillib_token_scanner *input,
    size_t context,
    struct cling_symbol_entry **entry);

/*
 * Assignment.lhs:
 * Makes sure lhs is a left value.
 * If it is an array subscription, return the type of element.
 * If it is a single variable, return its type.
 * If it is not a lvalue, returns CL_UNDEF.
 */
static int ast_check_assign_lhs(struct utillib_json_value *self, 
    struct cling_rd_parser *parser,
    struct utillib_token_scanner *input,
    size_t context);


static void cling_ast_unreachable(size_t code) {
  puts(cling_symbol_kind_tostring(code));
  assert(false);
}

int cling_ast_check_iden_assignable(
    struct utillib_json_value *self,
    struct cling_rd_parser *parser,
    struct utillib_token_scanner *input,
    size_t context)
{ 
  return ast_check_assign_lhs(self, parser, input, context);
}

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
      goto unassignable;
    return lhs_type & (CL_INT | CL_CHAR);
  }
  if (op->as_size_t != SYM_RK) {
    /*
     * Although it is not assignable, we still need to check it.
     */
    ast_check_expression(self, parser, input, context);
    goto unassignable;
  }
  lhs_type=ast_check_subscript(self, parser, input, context);
  if (lhs_type == CL_UNDEF)
    return CL_UNDEF;

  return lhs_type;
unassignable:
  rd_parser_error_push_back(parser,
      cling_not_lvalue_error(input, self, context));
  return CL_UNDEF;
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

  rhs_type=ast_check_expression(rhs, parser, input, context);
  if (rhs_type != CL_UNDEF && !ast_integral_compatible(rhs_type)) {
    rd_parser_error_push_back(parser,
        cling_incompatible_type_error(input, rhs_type, CL_INT, context));
  }
  return entry->kind & (~CL_ARRAY);

check_index_only:
  ast_check_expression(rhs, parser, input, context);
  return CL_UNDEF;

}

/*
 * Assumes "op" == SYM_RP
 * Call:
 * defined(defined) = defined
 * Otherwise undef
 */
static int ast_check_call(struct utillib_json_value *self,
                          struct cling_rd_parser *parser,
                          struct utillib_token_scanner *input, size_t context) {
  struct utillib_json_value *callee, *array, *func_name;
  struct utillib_json_value *arglist, *formal_arg_type;
  struct utillib_json_value *formal_arg, *actual_arg;
  int lhs_type, arg_type;
  struct cling_symbol_entry *entry;
  size_t formal_argc, actual_argc;
  int i, j, actual_arg_type;

  callee = utillib_json_object_at(self, "lhs");
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

  for (i = 0, j = 0; j < actual_argc; ++i, ++j) {
    /*
     * 1. computes the type of each argument.
     * 2. compares it with the corresponding formal argument.
     */
    actual_arg = utillib_json_array_at(array, j);
    if (i >= formal_argc) {
      ast_check_expression(actual_arg, parser, input, context);
      continue;
    }
    actual_arg_type =
        ast_check_expression(actual_arg, parser, input, context);
    formal_arg = utillib_json_array_at(arglist, i);
    formal_arg_type = utillib_json_object_at(formal_arg, "type");
    if (actual_arg_type == CL_UNDEF)
      continue;
    if (ast_integral_compatible(actual_arg_type))
      continue;
      rd_parser_error_push_back(parser,
          cling_incompatible_arg_error(input, i, formal_arg_type->as_size_t,
                                       actual_arg_type, context));
  }
  if (i != formal_argc) {
    func_name=utillib_json_object_at(entry->value, "name");
    rd_parser_error_push_back(
        parser, cling_argc_unmatched_error(input, func_name->as_ptr, actual_argc,
                                           formal_argc, context));
    return CL_UNDEF;
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
      ast_check_expression(arg, parser, input, context);
    }
  } while (0);
  return CL_UNDEF;
}

/*
 * lhs_type=ast_check_operand(self, "lhs", ...);
 * Filter out the undefined iden and not integral
 * guys all to be CL_UNDEF and pushes error properly.
 * Notes since all our expression effectly operate
 * on integral type, there is actually no need to
 * check compatibility of operands based on each
 * operator. They all need to be integral.
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
  type=ast_check_expression(object, parser, input, context);
  if (type == CL_UNDEF)
    return CL_UNDEF;
  if (!ast_integral_compatible(type)) {
    rd_parser_error_push_back(parser,
        cling_incompatible_type_error(input, type, CL_INT, context));
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
  if (lhs_type == CL_VOID || rhs_type == CL_VOID) {
    rd_parser_error_push_back(parser, 
        cling_incompatible_type_error(input, CL_INT, CL_VOID, context));
    return CL_UNDEF;
  }
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
  if (lhs_type == CL_VOID || rhs_type == CL_VOID) {
    rd_parser_error_push_back(parser, 
        cling_incompatible_type_error(input, CL_INT, CL_VOID, context));
    return CL_UNDEF;
  }
  if (lhs_type != CL_UNDEF || rhs_type != CL_UNDEF)
    return CL_INT;
  return CL_UNDEF;
}

/*
 * Assignment:
 * When array subscription appears on the left-hand-side
 * it become assignable.
 * defined '=' defined = defined
 * Otherwise undef
 */
int cling_ast_check_assign(struct utillib_json_value *self,
                               struct cling_rd_parser *parser,
                               struct utillib_token_scanner *input,
                               size_t context) {
  struct utillib_json_value *lhs=utillib_json_object_at(self, "lhs");
  int lhs_type=ast_check_assign_lhs(lhs, parser, input, context);
  int rhs_type=ast_check_operand(self, "rhs", parser, input, context);
  if (lhs_type != CL_UNDEF  &&  rhs_type != CL_UNDEF)
    return lhs_type;
  return CL_UNDEF;
}

/*
 * Walks the expression tree
 * and checks for bad guys.
 * This version checks for every kind of expressions
 * that can be recognized by opg_parser.
 */
static int ast_check_expression(struct utillib_json_value *self,
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

/*
 * Checks a return_expr when a return_stmt
 * is seen.
 * If the type is compatible, returns return_type,
 * or else returns CL_UNDEF.
 */
int cling_ast_check_returnness(
    struct utillib_json_value *self,
    struct cling_rd_parser *parser,
    struct utillib_token_scanner *input,
    size_t context,
    bool void_flag) {
  /*
   * 1. checks expr first.
   */
  int expr_type, return_type;
  char const* func_name;
  struct cling_symbol_entry *func_entry;

  if (void_flag)
    expr_type=CL_VOID;
  else
    expr_type=ast_check_expression(self, parser, input, context);
  if (expr_type == CL_UNDEF)
    return CL_UNDEF;
  func_name=parser->curfunc;
  if (!func_name)
    return CL_UNDEF;
  func_entry=cling_symbol_table_find(parser->symbol_table,
      func_name, CL_GLOBAL);
  /*
   * Valid func_name means valid entry.
   * No verbose assertion here.
   */

  return_type=func_entry->kind & (~CL_FUNC);
  expr_type &= (~CL_CONST);
  if (return_type == expr_type)
    return return_type;
  rd_parser_error_push_back(parser,
      cling_incompatible_type_error(input, expr_type, return_type, context));

  return CL_UNDEF;
}

/*
 * Only assign_expr and call_expr are allowed!
 */
int cling_ast_check_expr_stmt(struct utillib_json_value *self,
                               struct cling_rd_parser *parser,
                               struct utillib_token_scanner *input,
                               size_t context)
{
  struct utillib_json_value *op;

  op=utillib_json_object_at(self, "op");
  if (!op) {
    /*
     * We see a factor.
     */
    goto error;
  }
  switch(op->as_size_t) {
  case SYM_RP:
    /*
     * call_expr
     */
    return ast_check_call(self, parser, input, context);
  case SYM_EQ:
    /*
     * assign_expr
     */
    return cling_ast_check_assign(self, parser, input, context);
  default:
    goto error;
  }

error:
  rd_parser_error_push_back(parser,
      cling_invalid_expr_error(input, self, context));
  return CL_UNDEF;
}

/*
 * We need to exclude certain variants of expressions
 * since they are not allowed by the grammar.
 * They are assign_expr and boolean_expr.
 */
int cling_ast_check_expression(struct utillib_json_value *self,
                               struct cling_rd_parser *parser,
                               struct utillib_token_scanner *input,
                               size_t context)
{
  /*
   * Picks out assign_expr and boolean expression.
   */
  struct utillib_json_value *op;
  op=utillib_json_object_at(self, "op");
  if (op && (op->as_size_t == SYM_EQ
        || opg_parser_is_relop(op->as_size_t))) {
    rd_parser_error_push_back(parser,
        cling_invalid_expr_error(input, self, context));
    return CL_UNDEF;
  }
  return ast_check_expression(self, parser, input, context);
}

/*
 * iden = expr
 * That all we can do in `for' the initial part.
 */
int cling_ast_check_for_init(struct utillib_json_value *self,
                               struct cling_rd_parser *parser,
                               struct utillib_token_scanner *input,
                               size_t context)
{
  struct utillib_json_value * lhs, *op, *type;
  op=utillib_json_object_at(self, "op");
  if (!op) 
    goto error;
  if (op->as_size_t != SYM_EQ)
    goto error;
  lhs=utillib_json_object_at(self, "lhs");
  type=utillib_json_object_at(lhs, "type");
  if (type->as_size_t != SYM_IDEN)
    goto error;
  return cling_ast_check_assign(self, parser, input, context);
error:
  rd_parser_error_push_back(parser,
      cling_invalid_expr_error(input, self, context));
  return CL_UNDEF;
}

/*
 * condition ::= expr relop expr
 * | expr
 * In brief, condition **is** boolean_expr
 * excluding assign_expr and call_expr that
 * returns void.
 */
int cling_ast_check_condition(struct utillib_json_value *self,
                               struct cling_rd_parser *parser,
                               struct utillib_token_scanner *input,
                               size_t context)
{

  struct utillib_json_value * lhs, *op, *type;
  int bool_type;
  struct cling_error *error;

  op=utillib_json_object_at(self, "op");
  if (op && op->as_size_t == SYM_EQ) {
    error= cling_invalid_expr_error(input, self, context);
    goto error;
  }

  bool_type= ast_check_boolean(self, parser, input, context);
  if (!ast_integral_compatible(bool_type)) {
    error=cling_incompatible_type_error(input, bool_type, CL_INT, context);
    goto error;
  }
  return bool_type;

error:
  rd_parser_error_push_back(parser, error);
  return CL_UNDEF;
}

/*
 * This boring function is because the **grammar**
 * put such an unreasonable|over-simplified restriction
 * on the step part of `for'. It should not have done so...
 *
 * Now that opg_parser has accepted self, which is very
 * uneasy for him (see opg_parser.c for why), we have to
 * manually walk this ast not too deeply.
 */
int cling_ast_check_for_step(struct utillib_json_value *self,
                               struct cling_rd_parser *parser,
                               struct utillib_token_scanner *input,
                               size_t context)
{
  struct utillib_json_value * lhs, *rhs, *op, *type;
  op=utillib_json_object_at(self, "op");
  if (!op || op->as_size_t != SYM_EQ) 
    /*
     * It is not assign_expr
     */
    goto error;

  lhs=utillib_json_object_at(self, "lhs");
  type=utillib_json_object_at(lhs, "type");
  if (!type || type->as_size_t != SYM_IDEN)
    /*
     * The lhs is not iden.
     */
    goto error;
  rhs=utillib_json_object_at(self, "rhs");
  op=utillib_json_object_at(rhs, "op");
  if (!op || 
      op->as_size_t != SYM_ADD && op->as_size_t != SYM_MINUS)
    /*
     * The rhs is not additive expr
     */
    goto error;
  lhs=utillib_json_object_at(rhs, "lhs");
  type=utillib_json_object_at(lhs, "type");
  if (!type || type->as_size_t != SYM_IDEN)
    /*
     * The lhs of the rhs of self is not iden
     */
    goto error;
  rhs=utillib_json_object_at(rhs, "rhs");
  type=utillib_json_object_at(rhs, "type");
  if (!type || type->as_size_t != SYM_UINT)
    /*
     * The rhs of the rhs of self is not SYM_UINT
     * Notes SYM_UINT should be non-zero.
     */
    goto error;
  /*
   * All the names need a check, so...
   */
  return cling_ast_check_assign(self, parser, input, context);
error:
  rd_parser_error_push_back(parser,
      cling_invalid_expr_error(input, self, context));
  return CL_UNDEF;
}

