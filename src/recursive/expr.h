#ifndef RECUSIVE_EXPR_H
#define RECUSIVE_EXPR_H
#include <stdbool.h>
#include "recursive/context.h"
#include "recursive/node_base.h"

#define EXPR_IS_FUNC_DECLARE(FUNC) \
bool expr_is_ ## FUNC (pcontext *context)

EXPR_IS_FUNC_DECLARE (unary);
EXPR_IS_FUNC_DECLARE (cast);
EXPR_IS_FUNC_DECLARE (postfix);
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
EXPR_IS_FUNC_DECLARE (assign);
EXPR_IS_FUNC_DECLARE (arglist);
EXPR_IS_FUNC_DECLARE (condition);
EXPR_IS_FUNC_DECLARE (constant);
EXPR_IS_FUNC_DECLARE (in_parenthesis);
EXPR_IS_FUNC_DECLARE (optional_constant);
EXPR_IS_FUNC_DECLARE (optional_expr);
#endif // RECUSIVE_EXPR_H
