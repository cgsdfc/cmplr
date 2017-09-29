#include "recursive/error.h"
#include "recursive/stmt.h"
#include "recursive/expr.h"
#include "recursive/terminal.h"
#include "recursive/node_base.h"
#include "recursive/util.h"
#include "recursive/decl.h"

STMT_IS_FUNC_DECLARE (optional_expr)
{
  // [expr]
  return util_is_optional (context, expr_is_expression);
}

static void
reduce_jump_stmt (pcontext * context, TokenType tt)
{
  // goto identifier
  // return expr
  unary_node *jump_stmt = (unary_node *) make_unary_node (tt);
  node_base *operand = pcontext_pop_node (context);
  jump_stmt->operand = operand;
  pcontext_push_node (context, TO_NODE_BASE (jump_stmt));
}

STMT_IS_FUNC_DECLARE (jump)
{
  Token *t = pcontext_read_token (context, 0);
  switch (TOKEN_TYPE (t))
    {
    case TKT_KW_GOTO:		// goto identifier ;
      util_shift_one_token (context);	// shift off the goto
      if (util_is_identifier (context))
	{
	  reduce_jump_stmt (context, TOKEN_TYPE (t));
	  break;
	}
      die ("jump: expected identifier after goto token");
    case TKT_KW_CONTINUE:
    case TKT_KW_BREAK:
      pcontext_shift_token (context, 1);
      pcontext_push_node (context, make_terminal_node (t));
      break;
    case TKT_KW_RETURN:	// return [expr] ;
      pcontext_shift_token (context, 1);
      if (expr_is_expression (context))
	{
	  reduce_jump_stmt (context, TOKEN_TYPE (t));
	}
      else
	{
	  pcontext_push_node (context, make_terminal_node (t));
	}
      break;
    default:
      return false;
    }
  if (util_is_semicolon (context))
    {
      return true;
    }
  die ("jump: expected ';' at the end of a statement");
}

STMT_IS_FUNC_DECLARE (expr_for_seq)
{
  // for ([expr];[expr];[expr]) statement 
  return util_is_sequence (context,
			   stmt_is_optional_expr, util_is_semicolon,
			   stmt_is_optional_expr, util_is_semicolon,
			   stmt_is_optional_expr, NULL);
}

STMT_IS_FUNC_DECLARE (expr_for)
{
  return util_is_in_parentheses (context, stmt_is_expr_for_seq);
}

static void
reduce_for_expr (pcontext * context)
{
  ternary_node *tri = (ternary_node *) make_ternary_node ();
  node_base *step = pcontext_pop_node (context);
  node_base *cond = pcontext_pop_node (context);
  node_base *init = pcontext_pop_node (context);
  tri->first = init;
  tri->second = cond;
  tri->third = step;
  pcontext_push_node (context, TO_NODE_BASE (tri));
}


static void
reduce_for_stmt (pcontext * context)
{
  binary_node *for_node = (binary_node *) make_binary_node (TKT_KW_FOR);
  node_base *for_body = pcontext_pop_node (context);
  node_base *for_expr = pcontext_pop_node (context);
  for_node->lhs = for_expr;
  for_node->rhs = for_body;
  pcontext_push_node (context, TO_NODE_BASE (for_node));
}

static void
reduce_do_while_stmt (pcontext * context)
{
  binary_node *do_while = (binary_node *) make_binary_node (TKT_KW_DO);
  node_base *do_stmt = pcontext_pop_node (context);
  node_base *while_expr = pcontext_pop_node (context);
  do_while->lhs = do_stmt;
  do_while->rhs = while_expr;
  pcontext_push_node (context, TO_NODE_BASE (do_while));
}

static void
reduce_switch_or_while (pcontext * context, TokenType tt)
{
  binary_node *switch_or_while = (binary_node *) make_binary_node (tt);
  node_base *stmt = pcontext_pop_node (context);
  node_base *expr = pcontext_pop_node (context);
  switch_or_while->lhs = expr;
  switch_or_while->rhs = stmt;
  pcontext_push_node (context, TO_NODE_BASE (switch_or_while));
}

static void
reduce_if_stmt (pcontext * context)
{
  unary_node *if_stmt = (unary_node *) make_unary_node (TKT_KW_IF);
  ternary_node *if_body = (ternary_node *) make_ternary_node ();
  node_base *else_stmt = pcontext_pop_node (context);
  node_base *then = pcontext_pop_node (context);
  node_base *expr = pcontext_pop_node (context);
  if_stmt->operand = TO_NODE_BASE (if_body);
  if_body->first = expr;
  if_body->second = then;
  if_body->third = else_stmt;
  pcontext_push_node (context, TO_NODE_BASE (if_stmt));
}

STMT_IS_FUNC_DECLARE (expr_stmt_seq)
{
  // '(' expr ')' statement
  return
    util_is_sequence (context,
		      expr_is_in_parenthesis, stmt_is_statement, NULL);
}

STMT_IS_FUNC_DECLARE (while)
  {
    return util_is_terminal (context, TKT_KW_WHILE, false);
  }

STMT_IS_FUNC_DECLARE (do_while_stmt)
{
  // statement 'while' '(' expr ')' ';'
  return
    util_is_sequence (context,
		      stmt_is_statement, stmt_is_while,
		      expr_is_in_parenthesis, util_is_semicolon, NULL);
}

STMT_IS_FUNC_DECLARE (iterate)
{
  Token *t = pcontext_read_token (context, 0);
  switch (TOKEN_TYPE (t))
    {
    case TKT_KW_WHILE:
      util_shift_one_token (context);
      if (stmt_is_expr_stmt_seq (context))
	{
	  reduce_switch_or_while (context, TOKEN_TYPE (t));
	  return true;
	}
      die ("while: expected '(expr) statement' after 'while' token");
    case TKT_KW_FOR:
      util_shift_one_token (context);
      if (stmt_is_expr_for (context))
	{
	  reduce_for_expr (context);
	  if (stmt_is_statement (context))
	    {
	      reduce_for_stmt (context);
	      return true;
	    }
	  die ("for: expected statement after 'for...'");
	}
      die ("for: expected '(' [expr];[expr];[expr] ')' after 'for' token");
    case TKT_KW_DO:
      util_shift_one_token (context);
      if (stmt_is_do_while_stmt (context))
	{
	  reduce_do_while_stmt (context);
	  return true;
	}
      die ("do-while: expected 'while(expr);' after do '{' statement '}'");
    default:
      return false;
    }
}

STMT_IS_FUNC_DECLARE (
		       else
)
{
  return util_is_terminal (context, TKT_KW_ELSE, false /* pushing */ );
}

STMT_IS_FUNC_DECLARE (else_stmt)
{
  return util_is_sequence (context, stmt_is_else, stmt_is_statement);
}

STMT_IS_FUNC_DECLARE (select)
{
  Token *t = pcontext_read_token (context, 0);
  switch (TOKEN_TYPE (t))
    {
    case TKT_KW_IF:
      util_shift_one_token (context);
      if (stmt_is_expr_stmt_seq (context))
	{
	  if (!stmt_is_else_stmt (context))
	    {
	      util_push_node_null (context);
	    }
	  reduce_if_stmt (context);
	  return true;
	}
      die ("select: if: expected '(expr) statement' after 'if' token");
    case TKT_KW_SWITCH:
      util_shift_one_token (context);
      if (stmt_is_expr_stmt_seq (context))
	{
	  reduce_switch_or_while (context, TOKEN_TYPE (t));
	  return true;
	}
      die
	("select: switch: expected '(' expr ')' statemnet after 'switch' token");
    default:
      return false;
    }
}

static void
reduce_label_stmt (pcontext * context, TokenType tt)
{
  binary_node *label_stmt = (binary_node *) make_binary_node (tt);
  node_base *stmt = pcontext_pop_node (context);
  node_base *label = pcontext_pop_node (context);
  label_stmt->lhs = label;
  label_stmt->rhs = stmt;
  pcontext_push_node (context, TO_NODE_BASE (label_stmt));
}

STMT_IS_FUNC_DECLARE (colon_stmt_seq)
{
  // ':' statement
  return util_is_sequence (context, util_is_colon, stmt_is_statement, NULL);
}

STMT_IS_FUNC_DECLARE (case_stmt)
{
  return util_is_sequence (context, expr_is_constant, util_is_colon,
			   stmt_is_statement, NULL);
}

STMT_IS_FUNC_DECLARE (label)
{
  Token *t = pcontext_read_token (context, 0);
  Token *lookahead;
  switch (TOKEN_TYPE (t))
    {
    case TKT_KW_CASE:
      pcontext_shift_token (context, 1);
      if (stmt_is_case_stmt (context))
	{
	  reduce_label_stmt (context, TOKEN_TYPE (t));
	  return true;
	}
      die
	("case: expected constant expression ':' statement after 'case' token");
    case TKT_KW_DEFAULT:	// 'default' ':'
      util_shift_one_token (context);
      if (stmt_is_colon_stmt_seq (context))
	{
	  unary_node *default_stmt =
	    (unary_node *) make_unary_node (TOKEN_TYPE (t));
	  default_stmt->operand = pcontext_pop_node (context);
	  pcontext_push_node (context, TO_NODE_BASE (default_stmt));
	  return true;
	}
      die ("lable: expected ':' statement after 'default' token");
    case TKT_IDENTIFIER:	// identifier ':'
      // need lookahead to resolve
      lookahead = pcontext_read_token (context, 1);
      if (terminal_is_colon (lookahead))
	{
	  pcontext_shift_token (context, 2);
	  pcontext_push_node (context, make_terminal_node (t));	// push identifier
	  if (stmt_is_statement (context))
	    {
	      reduce_label_stmt (context, TOKEN_TYPE (t));
	      return true;
	    }
	  die ("label: expected statement after 'identifier:'");
	}			// fall through
    default:
      return false;
    }
}

STMT_IS_FUNC_DECLARE (exprstmt)
{
  // cannot push null until the semicolon is seen
  bool has_expr = expr_is_expression (context);
  bool has_semicolon = util_is_semicolon (context);
  if (has_expr)
    {
      if (has_semicolon)
	{
	  return true;
	}
      die ("exprstmt: expected ';' after expression");
    }
  else if (has_semicolon)
    {
      util_push_node_null (context);
      return true;
    }
  return false;
}

static void
reduce_compound (pcontext * context)
{
  binary_node *compound =
    (binary_node *) make_binary_node (TKT_STMT_COMPOUND);
  node_base *stmt_list = pcontext_pop_node (context);
  node_base *decl_list = pcontext_pop_node (context);
  compound->lhs = decl_list;
  compound->rhs = stmt_list;
  pcontext_push_node (context, TO_NODE_BASE (compound));
}

STMT_IS_FUNC_DECLARE (braceL)
{
  return util_is_terminal (context, TKT_LEFT_BRACE, false);
}

STMT_IS_FUNC_DECLARE (braceR)
{
  return util_is_terminal (context, TKT_RIGHT_BRACE, false);
}

STMT_IS_FUNC_DECLARE (statement_list)
{
  return util_is_list (context, stmt_is_statement, true /* allow_empty */ );
}

STMT_IS_FUNC_DECLARE (declare_list)
{
  return util_is_list (context, decl_is_declare, true /* allow_empty */ );
}

STMT_IS_FUNC_DECLARE (compound_seq)
{
  return util_is_sequence (context,
			   stmt_is_statement_list,
			   stmt_is_declare_list, NULL);
}

static
STMT_IS_FUNC_DECLARE (compound_impl1)
{
  if (util_is_in_braces (context, stmt_is_compound_seq))
    {
      reduce_compound (context);
      return true;
    }
  return false;
}

static
STMT_IS_FUNC_DECLARE (compound_impl2)
{
  Token *t1 = pcontext_read_token (context, 0);
  Token *t2 = pcontext_read_token (context, 1);
  // not a compound
  if (!terminal_is_braceL (t1))
    {
      return false;
    }
  // '{' '}' special case
  if (terminal_is_braceR (t2))
    {
      pcontext_shift_token (context, 2);
      util_push_node_null (context);
      return true;
    }
  util_shift_one_token (context);
  stmt_is_declare_list (context);
  stmt_is_statement_list (context);
  t2 = pcontext_read_token (context, 0);
  if (terminal_is_braceR (t2))
    {
      util_shift_one_token (context);
      reduce_compound (context);
      return true;
    }
  die ("compound: expected '}' at the end of statement list");
}

STMT_IS_FUNC_DECLARE (compound)
{
  return stmt_is_compound_impl2 (context);
}

STMT_IS_FUNC_DECLARE (statement)
{
  return
    util_is_one_of (context,
		    stmt_is_iterate,
		    stmt_is_select,
		    stmt_is_jump, stmt_is_label, stmt_is_compound,
		    // no prefix comes last
		    stmt_is_exprstmt, NULL);
}
