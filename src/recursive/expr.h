#ifndef RECUSIVE_EXPR_H
#define RECUSIVE_EXPR_H
#include <stdbool.h>
#include "recursive/context.h"
#include "recursive/node_base.h"

typedef enum associativity
{
  LEFT_ASSOC, RIGHT_ASSOC
} associativity;

#define EXPR_IS_FUNC_DECLARE(FUNC) \
bool expr_is_ ## FUNC (pcontext *context)

#define EXPR_IS_FUNC_DEFINE(FUNC, OP_FUNC, RHS_FUNC, FUNC_STR, RHS_STR)\
bool expr_is_ ## FUNC (pcontext *context) {\
  if (expr_is_ ## RHS_FUNC (context)) {\
    while (true) {\
      Token *t=pcontext_read_token (context,0);\
      if (terminal_is_ ## OP_FUNC (t)) {\
        pcontext_shift_token (context,1);\
        pcontext_push_node(context, make_binary_node(TOKEN_TYPE(t)));\
        if (expr_is_ ## RHS_FUNC (context)) {\
          reduce_binary(context);\
          continue;\
        } die(FUNC_STR ": expected " RHS_STR);\
      } else { break; }\
    } return true;\
  } return false;\
}

// =============================================== //
// unary_expr := postfix_expr
// | unary_op cast_expr
// | -- unary_expr
// | ++ unary_expr
// | sizeof '(' typename ')
// | sizeof unary_expr
// ;
EXPR_IS_FUNC_DECLARE (unary);
// =============================================== //
// cast_expr := unary_expr
// | '(' typename ')' cast_expr
// ;
EXPR_IS_FUNC_DECLARE (cast);
// =============================================== //
EXPR_IS_FUNC_DECLARE (postfix);
// =============================================== //
// primary_expr := string
// | identifier
// | float
// | int
// | '(' expr ')'
// ;
EXPR_IS_FUNC_DECLARE (primary);
EXPR_IS_FUNC_DECLARE (expression);
EXPR_IS_FUNC_DECLARE (additive);
EXPR_IS_FUNC_DECLARE (timing);
EXPR_IS_FUNC_DECLARE (shift);
EXPR_IS_FUNC_DECLARE (relatonal);
EXPR_IS_FUNC_DECLARE (equality);
EXPR_IS_FUNC_DECLARE (bit_and);
EXPR_IS_FUNC_DECLARE (bit_or);
EXPR_IS_FUNC_DECLARE (bit_xor);
EXPR_IS_FUNC_DECLARE (log_or);
EXPR_IS_FUNC_DECLARE (log_and);
// =============================================== //
// assign_expr := condition_expr
// | unary_expr assign_op assign_expr
// ;
EXPR_IS_FUNC_DECLARE (assign);
EXPR_IS_FUNC_DECLARE (arglist);
// =============================================== //
// condition_expr := logical_or_expr [ '?' expr ':' condition ]
// ;
EXPR_IS_FUNC_DECLARE (condition);
// =============================================== //
// constant_expr = condition_expr
// ;
EXPR_IS_FUNC_DECLARE (constant);
EXPR_IS_FUNC_DECLARE (in_parenthesis);
EXPR_IS_FUNC_DECLARE (colon);
EXPR_IS_FUNC_DECLARE(optional_constant);
#endif // RECUSIVE_EXPR_H
