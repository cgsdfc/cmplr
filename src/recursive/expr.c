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
  ternary_node *tri = (ternary_node *) pcontext_pop_node (context);
  node_base *cond = pcontext_pop_node (context);
  node_base *expr = pcontext_pop_node (context);
  node_base *log_or = pcontext_pop_node (context);
  tri->first = log_or;
  tri->second = expr;
  tri->third = cond;
  pcontext_push_node (context, TO_NODE_BASE (tri));
}

static void
reduce_unary (pcontext * context)
{
  // op=0, operand=1
  unary_node *op = (unary_node *) pcontext_pop_node (context);
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
  binary_node *op = (binary_node *) pcontext_pop_node (context);
  TokenType *op_type = &(op->op);
  node_base *lhs = pcontext_pop_node (context);
  op->lhs = lhs;
  op->rhs = rhs;
  reduce_operator (&(op->op), op->base.tag);
  pcontext_push_node (context, TO_NODE_BASE (op));
}

EXPR_IS_FUNC_DECLARE (in_parenthesis)	// context
{
  return util_is_in_parentheses (context, expr_is_expression);
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
      pcontext_push_node (context, make_terminal_node (t));
      return true;
    case TKT_LEFT_PARENTHESIS:
      if (expr_is_in_parenthesis (context))
	{
	  return true;
	}
      die
	("primary: expected string, int, float, identifier or expression in parenthesis");
    default:
      return false;
    }
}

EXPR_IS_FUNC_DECLARE (in_bracket)	// context
{
  return util_is_in_brackets (context, expr_is_expression);
}

EXPR_IS_FUNC_DECLARE (optional_arglist)	// context
{
  return util_is_optional (context, expr_is_arglist);
}

EXPR_IS_FUNC_DECLARE (optional_arglist_in_parenthesis)	// context
{
  return util_is_in_parentheses (context, expr_is_optional_arglist);
}

EXPR_IS_FUNC_DECLARE (arglist)
{
  // this is only called by postfix_list when
  // a '(' has been seen, but not shift.
  assert (terminal_is_parenthesisL (util_read_first_token (context)));
  util_shift_one_token (context);	// shift off the '('
  vector_node *v = NULL;	// telling whether it is an empty list
  if (expr_is_assign (context))
    {
      v = (vector_node *) make_vector_node ();	// non-empty
      vector_node_push_back (v, pcontext_pop_node (context));
      while (util_is_comma (context))
	{
	  if (expr_is_assign (context))
	    {
	      vector_node_push_back (v, pcontext_pop_node (context));
	      continue;
	    }
	  die ("arglist: expected assign expression after ',' token");
	}
    }
  if (util_is_parenthesisR (context))
    {
      if (v)
	{
	  pcontext_push_node (context, TO_NODE_BASE (v));
	}
      else
	{
	  util_push_node_null (context);
	}
      return true;
    }
  die ("arglist: expected ')' at the end of argument list");
}

EXPR_IS_FUNC_DECLARE (postfix_list)
{
  assert (pcontext_top_node (context));
  // this is only called when a primary
  // has been seen.
  Token *t = pcontext_read_token (context, 0);
  switch (TOKEN_TYPE (t))
    {
    case TKT_LEFT_BRACKET:
      pcontext_push_node (context,
			  make_binary_node (TKT_BINARY_OP_SUBSCRIPT));
      if (expr_is_in_bracket (context))
	{			// expr is bracket is not optional
	  reduce_binary (context);
	  return true;
	}
      die ("postfix: expected expression after '[' token");
    case TKT_LEFT_PARENTHESIS:
      pcontext_push_node (context, make_binary_node (TKT_BINARY_OP_INVOKE));
      if (expr_is_optional_arglist (context))
	{
	  reduce_binary (context);
	  return true;
	}
      die ("postfix: expected argument list after '(' token");
    case TKT_DOT:
    case TKT_BINARY_OP_MEMBER_ARROW:
      pcontext_push_node (context, make_binary_node (TOKEN_TYPE (t)));
      t = pcontext_read_token (context, 1);
      if (terminal_is_identifier (t))
	{
	  pcontext_shift_token (context, 2);
	  pcontext_push_node (context, make_terminal_node (t));
	  reduce_binary (context);
	  return true;
	}
      die ("postfix: expected identifier after '->' or '.' token");
    case TKT_UNARY_OP_PLUS_PLUS:	// '++'
    case TKT_UNARY_OP_MINUS_MINUS:	// '--'
      pcontext_shift_token (context, 1);
      pcontext_push_node (context, make_unary_node (TOKEN_TYPE (t)));
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
      while (expr_is_postfix_list (context))
	;
      return true;
    }
  return false;
}

EXPR_IS_FUNC_DECLARE (sizeof)
{
  // this is called by unary only when
  // the sizeof token has been seen but
  // not shift;
  assert (TKT_KW_SIZEOF == TOKEN_TYPE (util_read_first_token (context)));
  util_shift_one_token (context);	// shift off sizeof
  if (util_is_parenthesisL (context))
    {
      if (decl_is_typename_in_parenthesis (context))
	{
	  if (util_is_parenthesisR (context))
	    {
	      pcontext_push_node (context,
				  make_unary_node
				  (TKT_UNARY_OP_SIZOF_TYPENAME));
	      reduce_unary (context);	// typename, sizeof
	      return true;
	    }
	  die ("unary: expected ')' at the end of sizeof ");
	}
    }
  else if (expr_is_unary (context))
    {
      pcontext_push_node (context, make_unary_node (TKT_UNARY_OP_SIZOF_EXPR));
      reduce_unary (context);	// typename, sizeof
      return true;
    }
  return false;
}

EXPR_IS_FUNC_DECLARE (unary_impl)
{
  Token *t;
  unary_node *op;
  if (pcontext_get_unary_ontop (context))
    {
      pcontext_set_unary_ontop (context, false);
      return true;
    }
  t = util_read_first_token (context);
  if (terminal_is_unary_op (t))
    {
      util_shift_one_token (context);
      if (expr_is_cast (context))
	{
	  pcontext_push_node (context, make_unary_node (TOKEN_TYPE (t)));
	  reduce_unary (context);
	  return true;
	}
      die ("unary: expected cast expression after unary operator");
    }
  if (expr_is_postfix (context))
    {
      return true;
    }
  return false;
}

// this is the same as assign
// with a varying length of prefix.
EXPR_IS_FUNC_DECLARE (unary)
{
  Token *t;
  if (expr_is_unary_impl (context))
    {
      return true;
    }
  t = util_read_first_token (context);
  switch (TOKEN_TYPE (t))
    {
    case TKT_UNARY_OP_PLUS_PLUS:
    case TKT_UNARY_OP_MINUS_MINUS:
      util_shift_one_token (context);
      if (expr_is_unary (context))
	{
	  pcontext_push_node (context, make_unary_node (TOKEN_TYPE (t)));
	  reduce_unary (context);
	  return true;
	}
      die ("unary: expected unary after '++' or '--' token");
    case TKT_KW_SIZEOF:
      if (expr_is_sizeof (context))
	{
	  return true;
	}
      die ("unary: expected unary or ( typename ) after sizeof token");
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
      pcontext_push_node (context, make_binary_node (TKT_UNARY_OP_CAST));
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

static
EXPR_IS_FUNC_DECLARE (question)
{
  return util_is_terminal (context, TKT_QUESTION, false /* pushing */ );
}

static
EXPR_IS_FUNC_DECLARE (ternary_seq)
{
  return util_is_sequence (context, expr_is_question,	// '?'
			   expr_is_expression,	// expr
			   util_is_colon,	// ':'
			   expr_is_condition,	// condition
			   NULL);
}

EXPR_IS_FUNC_DECLARE (condition)
{
  if (expr_is_log_or (context))
    {
      if (expr_is_ternary_seq (context))
	{
	  pcontext_push_node (context, make_ternary_node ());
	  reduce_ternary (context);
	}
      return true;
    }
  return false;
}

enum
{
  ASSIGN_IMPL_ONE_NODE,
  ASSIGN_IMPL_THREE_NODE,
  ASSIGN_IMPL_ERROR,
};

int
expr_is_assign_impl (pcontext * context)
{
  // return ASSIGN_IMPL_ONE_NODE if
  // only a unary was seen.
  // return ASSIGN_IMPL_THREE_NODE if
  // the stack has at least 3 node and
  // the top node is unary to be reduce
  // to a condition.
  // return ASSIGN_IMPL_ERROR if after
  // seeing an assign_op we failed to
  // see a unary, thus, this tailing 
  // cannot be reduce to condition.
  // assuming there is a already a unary
  // on the top of the node_stack
  Token *t = util_read_first_token (context);
  if (!terminal_is_assign_op (t))
    {
      return ASSIGN_IMPL_ONE_NODE;
    }
  util_shift_one_token (context);
  pcontext_push_node (context, make_binary_node (TOKEN_TYPE (t)));
  while (expr_is_unary (context))
    {
      Token *lookahead = util_read_first_token (context);
      if (terminal_is_assign_op (lookahead))
	{
	  util_shift_one_token (context);
	  reduce_binary (context);
	  pcontext_push_node (context,
			      make_binary_node (TOKEN_TYPE (lookahead)));
	  continue;
	}
      return ASSIGN_IMPL_THREE_NODE;
    }
  return ASSIGN_IMPL_ERROR;
}



EXPR_IS_FUNC_DECLARE (assign)
{
  if (expr_is_unary (context))
    {
      int r = (expr_is_assign_impl (context));
      if (r == ASSIGN_IMPL_ERROR)
	{
	  die ("assign: expected unary after assign operator");
	}
      pcontext_set_unary_ontop (context, true);
      if (expr_is_condition (context))
	{
	  switch (r)
	    {
	    case ASSIGN_IMPL_ONE_NODE:
	      // the unary on the top was reduce to condition
	      // directly.
	      break;
	    case ASSIGN_IMPL_THREE_NODE:
	      // there is at least 3 nodes on top of stack;
	      // and the top one is reduced to condition;
	      reduce_binary (context);
	      break;
	    }
	  return true;
	}
      die ("assign: expected condition expression after assign operator");
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

EXPR_IS_FUNC_DECLARE (comma_assign_seq)
{
  return util_is_sequence (context, util_is_comma, expr_is_assign, NULL);
}

EXPR_IS_FUNC_DECLARE (optional_constant)
{
  return util_is_optional (context, expr_is_constant);
}
