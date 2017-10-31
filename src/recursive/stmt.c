#include "recursive/stmt.h"
#include "construct.h"
#include "recursive/decl.h"
#include "recursive/error.h"
#include "recursive/expr.h"
#include "recursive/node_base.h"
#include "recursive/terminal.h"
#include "recursive/util.h"

STMT_IS_FUNC_DECLARE(jump)
{
	Token *t = pcontext_read_token(context, 0);
	switch (TOKEN_TYPE(t)) {
	case TKT_KW_GOTO:	// goto identifier ;
		util_shift_one_token(context);	// shift off the goto
		if (util_is_identifier(context)) {
			pcontext_reduce_unary(context, STMT_GOTO);
			break;
		}
		die("jump: expected identifier after goto token");
	case TKT_KW_CONTINUE:
	case TKT_KW_BREAK:
		pcontext_shift_token(context, 1);
		pcontext_push_node(context, make_terminal_node(t));
		break;
	case TKT_KW_RETURN:	// return [expr] ;
		pcontext_shift_token(context, 1);
		expr_is_optional_expr(context);
		pcontext_reduce_unary(context, STMT_RETURN);
		break;
	default:
		return false;
	}
	if (util_is_semicolon(context)) {
		return true;
	}
	die("jump: expected ';' at the end of a statement");
}

STMT_IS_FUNC_DECLARE(expr_for_seq)
{
	// for ([expr];[expr];[expr]) stmt
	return util_is_sequence(context, expr_is_optional_expr,
				util_is_semicolon, expr_is_optional_expr,
				util_is_semicolon, expr_is_optional_expr,
				NULL);
}

STMT_IS_FUNC_DECLARE(expr_for)
{
	return util_is_in_parentheses(context, stmt_is_expr_for_seq);
}

STMT_IS_FUNC_DECLARE(expr_stmt_seq)
{
	// '(' expr ')' stmt
	return util_is_sequence(context, expr_is_in_parenthesis,
				stmt_is_statement, NULL);
}

STMT_IS_FUNC_DECLARE(while_)
{
	return util_is_terminal(context, TKT_KW_WHILE, false);
}

STMT_IS_FUNC_DECLARE(do_while_stmt)
{
	// stmt 'while' '(' expr ')' ';'
	return util_is_sequence(context, stmt_is_statement, stmt_is_while_,
				expr_is_in_parenthesis, util_is_semicolon,
				NULL);
}

STMT_IS_FUNC_DECLARE(iterate)
{
	Token *t = pcontext_read_token(context, 0);
	switch (TOKEN_TYPE(t)) {
	case TKT_KW_WHILE:
		util_shift_one_token(context);
		if (stmt_is_expr_stmt_seq(context)) {
			pcontext_reduce_binary(context, STMT_WHILE);
			return true;
		}
		die("while: expected '(expr) statement' after 'while' token");
	case TKT_KW_FOR:
		util_shift_one_token(context);
		if (stmt_is_expr_for(context)) {
			pcontext_reduce_ternary(context);
			if (stmt_is_statement(context)) {
				pcontext_reduce_binary(context, STMT_FOR);
				return true;
			}
			die("for: expected statement after 'for...'");
		}
		die("for: expected '(' [expr];[expr];[expr] ')' after 'for' token");
	case TKT_KW_DO:
		util_shift_one_token(context);
		if (stmt_is_do_while_stmt(context)) {
			pcontext_reduce_binaryR(context, STMT_DO_WHILE);
			return true;
		}
		die("do-while: expected 'while(expr);' after do '{' statement '}'");
	default:
		return false;
	}
}

STMT_IS_FUNC_DECLARE(else_)
{
	return util_is_terminal(context, TKT_KW_ELSE,
				false /* pushing */ );
}

STMT_IS_FUNC_DECLARE(else_stmt)
{
	return util_is_sequence(context, stmt_is_else_, stmt_is_statement);
}

STMT_IS_FUNC_DECLARE(optional_else_stmt)
{
	return util_is_optional(context, stmt_is_else_stmt);
}

STMT_IS_FUNC_DECLARE(select)
{
	Token *t = pcontext_read_token(context, 0);
	switch (TOKEN_TYPE(t)) {
	case TKT_KW_IF:
		util_shift_one_token(context);
		if (stmt_is_expr_stmt_seq(context)) {
			stmt_is_optional_else_stmt(context);
			// binary_node(else, then_stmt, else_stmt);
			pcontext_reduce_binary(context, STMT_ELSE);
			// binary_node(if, expr, else);
			pcontext_reduce_binary(context, STMT_IF);
			return true;
		}
		die("select: if: expected '(expr) statement' after 'if' token");
	case TKT_KW_SWITCH:
		util_shift_one_token(context);
		if (stmt_is_expr_stmt_seq(context)) {
			pcontext_reduce_binary(context, STMT_SWITCH);
			return true;
		}
		die("select: switch: expected '(' expr ')' statemnet after 'switch' token");
	default:
		return false;
	}
}

STMT_IS_FUNC_DECLARE(colon_stmt_seq)
{
	// ':' stmt
	return util_is_sequence(context, util_is_colon, stmt_is_statement,
				NULL);
}

STMT_IS_FUNC_DECLARE(case_stmt)
{
	return util_is_sequence(context, expr_is_constant, util_is_colon,
				stmt_is_statement, NULL);
}

STMT_IS_FUNC_DECLARE(label)
{
	Token *t = pcontext_read_token(context, 0);
	Token *lookahead;
	switch (TOKEN_TYPE(t)) {
	case TKT_KW_CASE:
		pcontext_shift_token(context, 1);
		if (stmt_is_case_stmt(context)) {
			pcontext_reduce_binary(context, STMT_CASE);
			return true;
		}
		die("case: expected constant expression ':' statement after 'case' token");
	case TKT_KW_DEFAULT:	// 'default' ':'
		util_shift_one_token(context);
		if (stmt_is_colon_stmt_seq(context)) {
			pcontext_reduce_unary(context, STMT_DEFAULT);
			return true;
		}
		die("lable: expected ':' statement after 'default' token");
	case TKT_IDENTIFIER:	// identifier ':'
		// need lookahead to resolve
		lookahead = pcontext_read_token(context, 1);
		if (terminal_is_colon(lookahead)) {
			pcontext_shift_token(context, 2);
			pcontext_push_node(context, make_terminal_node(t));	// push identifier
			if (stmt_is_statement(context)) {
				pcontext_reduce_binary(context,
						       STMT_LABEL);
				return true;
			}
			die("label: expected statement after 'identifier:'");
		}		// fall through
	default:
		return false;
	}
}

STMT_IS_FUNC_DECLARE(exprstmt)
{
	// cannot push null until the semicolon is seen
	bool has_expr = expr_is_expression(context);
	bool has_semicolon = util_is_semicolon(context);
	if (has_expr) {
		if (has_semicolon) {
			return true;
		}
		die("exprstmt: expected ';' after expression");
	} else if (has_semicolon) {
		util_push_node_null(context);
		return true;
	}
	return false;
}

STMT_IS_FUNC_DECLARE(statement_list)
{
	return util_is_list(context, stmt_is_statement,
			    true /* allow_empty */ );
}

STMT_IS_FUNC_DECLARE(compound_seq)
{
	return util_is_sequence(context, stmt_is_statement_list,
				decl_is_declare_list, NULL);
}

static STMT_IS_FUNC_DECLARE(compound_impl1)
{
	if (util_is_in_braces(context, stmt_is_compound_seq)) {
		pcontext_reduce_binary(context, STMT_COMPOUND);
		return true;
	}
	return false;
}

static STMT_IS_FUNC_DECLARE(compound_impl2)
{
	Token *t1 = pcontext_read_token(context, 0);
	Token *t2 = pcontext_read_token(context, 1);
	// not a compound
	if (!terminal_is_braceL(t1)) {
		return false;
	}
	// '{' '}' special case
	if (terminal_is_braceR(t2)) {
		pcontext_shift_token(context, 2);
		util_push_node_null(context);
		return true;
	}
	util_shift_one_token(context);
	decl_is_declare_list(context);
	stmt_is_statement_list(context);
	t2 = pcontext_read_token(context, 0);
	if (terminal_is_braceR(t2)) {
		util_shift_one_token(context);
		pcontext_reduce_binary(context, STMT_COMPOUND);
		return true;
	}
	die("compound: expected '}' at the end of statement list");
}

STMT_IS_FUNC_DECLARE(compound)
{
	return stmt_is_compound_impl2(context);
}

STMT_IS_FUNC_DECLARE(statement)
{
	return util_is_one_of(context, stmt_is_iterate, stmt_is_select,
			      stmt_is_jump, stmt_is_label,
			      stmt_is_compound,
			      // no prefix comes last
			      stmt_is_exprstmt, NULL);
}
