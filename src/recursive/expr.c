#include "recursive/error.h"
#include "recursive/expr.h"
#include "recursive/terminal.h"
#include "recursive/util.h"
#include "recursive/decl.h"
static void
reduce_operator (TokenType * op_type, node_tag tag)
{
  // fix operator
  switch (*op_type)
    {
    case TKT_AMPERSAND:
      switch (tag)
	{
	case NODE_TAG_UNARY:
	  *op_type = TKT_UNARY_OP_ADDRESS;
	  break;
	case NODE_TAG_BINARY:
	  *op_type = TKT_BINARY_OP_BIT_AND;
	  break;
	}
      break;
    case TKT_STAR:
      switch (tag)
	{
	case NODE_TAG_UNARY:
	  *op_type = TKT_UNARY_OP_DEREFERENCE;
	  break;
	case NODE_TAG_BINARY:
	  *op_type = TKT_BINARY_OP_MUL;
	  break;
	}
      break;
      break;
    case TKT_PLUS:
      switch (tag)
	{
	case NODE_TAG_UNARY:
	  *op_type = TKT_UNARY_OP_POSITIVE;
	  break;
	case NODE_TAG_BINARY:
	  *op_type = TKT_BINARY_OP_ADD;
	  break;
	}
      break;
    case TKT_MINUS:
      switch (tag)
	{
	case NODE_TAG_UNARY:
	  *op_type = TKT_UNARY_OP_NEGATIVE;
	  break;
	case NODE_TAG_BINARY:
	  *op_type = TKT_BINARY_OP_SUB;
	  break;
	}
      break;
    }
}

static void
reduce_ternary (pcontext * context)
{
  // ternary = log_or ? expr : cond;
  ternary_node *tri = pcontext_pop_node (context);
  node_base *cond = pcontext_pop_node (context);
  node_base *expr = pcontext_pop_node (context);
  node_base *log_or = pcontext_pop_node (context);
  tri->cond = log_or;
  tri->first = expr;
  tri->second = cond;
  pcontext_push_node (context, TO_NODE_BASE (tri));
}

static void
reduce_unary (pcontext * context)
{
  // op=0, operand=1
  unary_node *op = pcontext_pop_node (context);
  node_base *operand = pcontext_pop_node (context);
  op->operand = operand;
  reduce_operator (&(op->op), op->base.tag);
  pcontext_push_node (context, TO_NODE_BASE (op));
}

static void
reduce_binary (pcontext * context)
{
  // 0==rhs, 1==op, 2==lhs
  node_base *rhs = pcontext_pop_node (context);
  binary_node *op = pcontext_pop_node (context);
  TokenType *op_type = &(op->op);
  node_base *lhs = pcontext_pop_node (context);
  op->lhs = lhs;
  op->rhs = rhs;
  reduce_operator (&(op->op), op->base.tag);
  pcontext_push_node (context, TO_NODE_BASE (op));
}

EXPR_IS_FUNC_DECLARE (in_parenthesis)	// context
{
  return util_is_in_parentheses (expr_is_expression, context);
}

EXPR_IS_FUNC_DECLARE (primary)	// context
{
  Token *t = pcontext_read_token (context, 0);
  switch (TOKEN_TYPE (t))
    {
    case TKT_STRING_CONST:
    case TKT_INT_CONST:
    case TKT_FLOAT_CONST:
    case TKT_IDENTIFIER:
      pcontext_shift_token (context, 1);
      pcontext_push_node (context, TO_NODE_BASE (make_terminal_node (t)));
      return true;
    default:
      if (expr_is_in_parenthesis (context))
	{
	  return true;
	}
      die
	("primary: expected string, int, float, identifier or expression in parenthesis");
    }
}

EXPR_IS_FUNC_DECLARE (in_bracket)	// context
{
  return util_is_in_brackets (expr_is_expression, context);
}

EXPR_IS_FUNC_DECLARE (optional_argument)	// context
{
  return util_is_optional (expr_is_argument, context);
}

EXPR_IS_FUNC_DECLARE (optional_argument_in_parenthesis)	// context
{
  return util_is_in_parentheses (expr_is_optional_argument, context);
}

static bool
expr_postfix_list (pcontext * context)	// context
{
  Token *t = pcontext_read_token (context, 0);
  TokenType expected;
  switch (TOKEN_TYPE (t))
    {
    case TKT_LEFT_BRACKET:
      pcontext_push_node (context,
			  TO_NODE_BASE (make_binary_node
					(TKT_BINARY_OP_SUBSCRIPT,
					 "operator[]")));
      if (expr_is_in_bracket (context))
	{
	  reduce_binary (context);	// 0==primary, 1==op[], 2==expr
	  return true;
	}
      die ("postfix: expected expression after '['");
    case TKT_LEFT_PARENTHESIS:
      pcontext_push_node (context,
			  TO_NODE_BASE (make_binary_node
					(TKT_BINARY_OP_INVOKE,
					 "operator()")));
      if (expr_is_optional_argument_in_parenthesis (context))
	{
	  reduce_binary (context);	// 0==primary, 1==op(), 2==expr opl
	  return true;
	}
      die ("postfix: expected expression after '('");
    case TKT_DOT:
    case TKT_BINARY_OP_MEMBER_ARROW:
      pcontext_push_node (context,
			  TO_NODE_BASE (make_binary_node
					(TOKEN_TYPE (t),
					 "operator* | operator->")));
      t = pcontext_read_token (context, 1);
      if (terminal_is_identifier (t))
	{
	  pcontext_shift_token (context, 2);	// (->|.)id
	  pcontext_push_node (context, TO_NODE_BASE (make_terminal_node (t)));
	  reduce_binary (context);	// lazy reduce;
	  return true;
	}
      die ("postfix: expected identifier after '->' or '.' token");
    case TKT_UNARY_OP_PLUS_PLUS:	// '++'
    case TKT_UNARY_OP_MINUS_MINUS:	// '--'
      pcontext_shift_token (context, 1);
      pcontext_push_node (context,
			  TO_NODE_BASE (make_unary_node
					(TOKEN_TYPE (t),
					 "operator--(int) | operator++(int)")));
      reduce_unary (context);
      return true;
    default:
      return false;
    }
}

EXPR_IS_FUNC_DECLARE (postfix)
{
  if (expr_is_primary (context))
    {
      util_is_list (expr_postfix_list, context);
      return true;
    }
  return false;
}


EXPR_IS_FUNC_DECLARE (unary)
{
  if (expr_is_postfix (context))
    {
      return true;
    }
  Token *t = pcontext_read_token (context, 0);
  unary_node *op;
  if (terminal_is_unary_op (t))
    {
      pcontext_shift_token (context, 1);
      if (expr_is_cast (context))
	{
	  pcontext_push_node (context,
			      TO_NODE_BASE (make_unary_node
					    (TOKEN_TYPE (t),
					     "operator cast")));
	  reduce_unary (context);
	  return true;
	}
      die ("unary: expected cast expression after unary operator");
    }
  switch (TOKEN_TYPE (t))
    {
    case TKT_UNARY_OP_PLUS_PLUS:
    case TKT_UNARY_OP_MINUS_MINUS:
      pcontext_shift_token (context, 1);
      if (expr_is_unary (context))
	{
	  pcontext_push_node (context,
			      TO_NODE_BASE (make_unary_node
					    (TOKEN_TYPE (t),
					     "operator++ | operator--")));
	  reduce_unary (context);
	  return true;
	}
      die ("unary: expected unary after '++' or '--' token");
    case TKT_KW_SIZEOF:
      pcontext_shift_token (context, 1);
      t = pcontext_read_token (context, 0);
      if (terminal_is_parenthesisL (t))
	{
	  if (decl_is_typename_in_parenthesis (context))
	    {
	      pcontext_push_node (context,
				  TO_NODE_BASE (make_unary_node
						(TKT_UNARY_OP_SIZOF_TYPENAME,
						 "sizeof")));
	      reduce_unary (context);	// typename, sizeof
	      return true;
	    }
	  die ("unary: expected sizeof (typename)");
	}
      else if (expr_is_unary (context))
	{
	  pcontext_push_node (context,
			      TO_NODE_BASE (make_unary_node
					    (TKT_UNARY_OP_SIZOF_EXPR,
					     "sizeof")));
	  reduce_unary (context);	// typename, sizeof
	  return true;
	}			// fail through
    default:
      return false;
    }
}

EXPR_IS_FUNC_DECLARE (cast)
{
  if (expr_is_unary (context))
    {
      return true;
    }
  if (decl_is_typename_in_parenthesis (context))
    {
      // rhs in stack
      pcontext_push_node (context,
			  TO_NODE_BASE (make_binary_node
					(TKT_UNARY_OP_CAST,
					 "operator cast")));
      // note that the cast-node is a binary node since it has rhs the typename, lhs the expr
      // and op the cast, but its op is a unary op
      if (expr_is_cast (context))
	{
	  reduce_binary (context);
	  return true;
	}
      die ("cast: expected cast expression after '(typename)'");
    }
  return false;
}

EXPR_IS_FUNC_DECLARE (colon)	// adaptor
{
  return util_is_terminal (context, TKT_COLON);
}

EXPR_IS_FUNC_DECLARE (ternary_sequence)
{
  return util_is_sequence (context,
			   expr_is_expression,
			   expr_is_colon, expr_is_condition, NULL);
}

EXPR_IS_FUNC_DECLARE (condition)
{
  if (expr_is_log_or (context))
    {
      Token *t = pcontext_read_token (context, 0);
      // no ? expr : cond
      if (!terminal_is_question (t))
	{
	  return true;
	}
      pcontext_shift_token (context, 1);	// shift '?'
      if (expr_is_ternary_sequence (context))
	{
	  reduce_ternary (context);
	  return true;
	}
      die ("condition: expected 'expr : condition' after '?' token");
    }
  return false;
}


EXPR_IS_FUNC_DECLARE (assign)
{
  if (expr_is_condition (context))
    {
      return true;
    }
  if (expr_is_unary (context))
    {
      Token *t = pcontext_read_token (context, 0);
      if (terminal_is_assign_op (t))
	{
	  pcontext_shift_token (context, 1);
	  pcontext_push_node (context,
			      TO_NODE_BASE (make_binary_node
					    (TOKEN_TYPE (t),
					     "operator assign")));
	  if (expr_is_assign (context))
	    {
	      reduce_binary (context);
	      return true;
	    }
	  die ("assign: expected assign expression after assign operator");
	}
      die ("assign: expected assign operator after unary expression");
    }
  return false;
}

EXPR_IS_FUNC_DEFINE (timing, timing_op, cast, "timing", "cast");
EXPR_IS_FUNC_DEFINE (additive, additive_op, timing, "additive", "timing");
EXPR_IS_FUNC_DEFINE (shift, shift_op, additive, "shift", "additive");
EXPR_IS_FUNC_DEFINE (relation, relation_op, shift, "relation", "shift");
EXPR_IS_FUNC_DEFINE (equality, equality_op, relation, "equality", "relation");
EXPR_IS_FUNC_DEFINE (bit_and, bit_and_op, equality, "bit_and", "equality");
EXPR_IS_FUNC_DEFINE (bit_xor, bit_xor_op, bit_and, "bit_xor", "bit_and");
EXPR_IS_FUNC_DEFINE (bit_or, bit_or_op, bit_xor, "bit_or", "bit_xor");
EXPR_IS_FUNC_DEFINE (log_and, log_and_op, bit_or, "log_and", "bit_or");
EXPR_IS_FUNC_DEFINE (log_or, log_or_op, log_and, "log_or", "log_and");
EXPR_IS_FUNC_DEFINE (expression, comma, assign, "expression", "assign");
EXPR_IS_FUNC_DECLARE (constant)
{
  return expr_is_condition (context);
}

// note: argument is in fact a list of 
// assign expr separated by comma.
EXPR_IS_FUNC_DECLARE (argument)
{
  if (expr_is_assign (context))
    {
      vector_node *node = (vector_node *) make_vector_node ("argument");
      vector_node_push_back (node, pcontext_pop_node (context));
      while (true)
	{
	  Token *t = pcontext_read_token (context, 0);
	  if (!terminal_is_assign_op (t))
	    {
	      pcontext_push_node (context, TO_NODE_BASE (node));
	      reduce_binary (context);	// 0==argument, 1==oper(), 2==unary
	      return true;
	    }
	  pcontext_shift_token (context, 1);
	  if (expr_is_assign (context))
	    {
	      vector_node_push_back (node, pcontext_pop_node (context));
	      continue;
	    }
	  die ("argument: expected assign expr after assign operator");
	}
    }
  return false;
}
