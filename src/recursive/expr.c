#include "recursive/expr.h"
#include "construct.h"
#include "recursive/decl.h"
#include "recursive/error.h"
#include "recursive/terminal.h"
#include "recursive/util.h"
#include "utillib/return.h"

#define EXPR_IS_FUNC_DEFINE(FUNC, OP_FUNC, RHS_FUNC, FUNC_STR, RHS_STR)        \
  bool expr_is_##FUNC(pcontext *context) {                                     \
    if (expr_is_##RHS_FUNC(context)) {                                         \
      while (true) {                                                           \
        Token *t = util_read_first_token(context);                             \
        if (terminal_is_##OP_FUNC(t)) {                                        \
          pcontext_shift_token(context, 1);                                    \
          if (expr_is_##RHS_FUNC(context)) {                                   \
            pcontext_reduce_binary(                                            \
                context, util_token_type_to_construct(TOKEN_TYPE(t)));         \
            continue;                                                          \
          }                                                                    \
          die(FUNC_STR ": expected " RHS_STR);                                 \
        } else {                                                               \
          break;                                                               \
        }                                                                      \
      }                                                                        \
      return true;                                                             \
    }                                                                          \
    return false;                                                              \
  }

EXPR_IS_FUNC_DECLARE(in_parenthesis)
{
	pcontext_set_pfunction_name(context, "expression");
	return util_is_in_parentheses(context, expr_is_expression);
}

EXPR_IS_FUNC_DECLARE(primary)
{
	Token *t = util_read_first_token(context);
	switch (TOKEN_TYPE(t)) {
	case TKT_STRING_CONST:
	case TKT_INT_CONST:
	case TKT_FLOAT_CONST:
	case TKT_IDENTIFIER:
		util_shift_one_token(context);
		pcontext_reduce_terminal(context, t);
		return RETURN_ACCEPT;
	case TKT_LEFT_PARENTHESIS:
		switch (expr_is_in_parenthesis(context)) {
		case RETURN_ACCEPT:
			return RETURN_ACCEPT;
		case RETURN_ERROR:
			return RETURN_ERROR;
		case RETURN_REJECT:
		default:
			assert(false);
		}
	default:
		return RETURN_REJECT;
	}
}

EXPR_IS_FUNC_DECLARE(in_bracket)
{
	pcontext_set_pfunction_name(context, "expression");
	return util_is_in_brackets(context, expr_is_expression);
}

EXPR_IS_FUNC_DECLARE(optional_arglist)
{
	pcontext_set_pfunction_name(context, "argument list");
	return util_is_optional(context, expr_is_arglist);
}

EXPR_IS_FUNC_DECLARE(optional_arglist_in_parenthesis)
{
	pcontext_set_pfunction_name(context, "optional argument list");
	return util_is_in_parentheses(context, expr_is_optional_arglist);
}

static EXPR_IS_FUNC_DECLARE(arglist_aux)
{
	return util_is_comma_sep_list(context, expr_is_assign,
				      true /* allow_empty */ );
}

EXPR_IS_FUNC_DECLARE(arglist)
{
	return util_is_in_parentheses(context, expr_is_arglist_aux);
}

EXPR_IS_FUNC_DECLARE(postfix_list)
{
	Token *t = util_read_first_token(context);
	int what = TOKEN_TYPE(t);
	int r;
	switch (what) {
	case TKT_LEFT_BRACKET:
	case TKT_LEFT_PARENTHESIS:
		switch (what == TKT_LEFT_PARENTHESIS ? r =
			expr_is_optional_arglist(context)
			: r = expr_is_in_bracket(context)) {
		case RETURN_REJECT:
		default:
			assert(false);
			//
		case RETURN_ACCEPT:
			pcontext_reduce_binary(context, OP_INVOKE);
			return r;
		case RETURN_ERROR:
			return r;
		}
	case TKT_DOT:
	case TKT_BINARY_OP_MEMBER_ARROW:
		util_shift_one_token(context);
		if (util_is_identifier(t)) {
			pcontext_reduce_binary(context,
					       util_token_type_to_construct
					       (what));
			return RETURN_ACCEPT;
		}
		return pcontext_push_expected_error(context, ERR_EXPECTED,
						    "identifier",
						    ERR_AFTER,
						    "'->' or '.' token",
						    NULL, NULL);
	case TKT_UNARY_OP_PLUS_PLUS:
	case TKT_UNARY_OP_MINUS_MINUS:
		util_shift_one_token(context);
		pcontext_reduce_unary(context,
				      util_token_type_to_construct(what));
		return RETURN_ACCEPT;
	default:
		return RETURN_REJECT;
	}
}

EXPR_IS_FUNC_DECLARE(postfix)
{
	int r;
	switch (r = expr_is_primary(context)) {
	case RETURN_ACCEPT:
		while (true) {
			switch (r = expr_is_postfix_list(context)) {
			case RETURN_ACCEPT:
				continue;
			case RETURN_ERROR:
				return r;
			case RETURN_REJECT:
				return r;
			}
		}
	default:
		return r;
	}
}

EXPR_IS_FUNC_DECLARE(sizeof_)
{
	// this is called by unary only when
	// the sizeof token has been seen but
	// not shift;
	int r;
	util_shift_one_token(context);
	if (util_is_parenthesisL(context)) {
		switch (r = decl_is_typename_in_parenthesis(context)) {
		case RETURN_ACCEPT:
			pcontext_reduce_unary(context, OP_SIZEOF_TYPE);
			return r;
		case RETURN_ERROR:
			return r;
		case RETURN_REJECT:
			break;
		}
	}
	switch (r = expr_is_unary(context)) {
	case RETURN_ACCEPT:
		pcontext_reduce_unary(context, OP_SIZEOF_UNARY);
		return r;
	case RETURN_REJECT:
		break;
	case RETURN_ERROR:
		return r;
	}
	return r;
}

EXPR_IS_FUNC_DECLARE(unary_impl)
{
	Token *t;
	int r;
	int what;
	if (pcontext_test_prefix(context, PCONTEXT_UNARY)) {
		pcontext_mark_prefix(context, PCONTEXT_UNARY, false);
		return RETURN_ACCEPT;
	}
	t = util_read_first_token(context);
	what = TOKEN_TYPE(t);
	if (terminal_is_unary_op(t)) {
		util_shift_one_token(context);
		switch (r = expr_is_cast(context)) {
		case RETURN_ACCEPT:
			pcontext_reduce_unary(context,
					      util_token_type_to_construct
					      (what));
			return r;
		case RETURN_REJECT:
			return expr_is_postfix(context);
		case RETURN_ERROR:
			return r;
		}
	}
}

EXPR_IS_FUNC_DECLARE(unary)
{
	Token *t;
	construct c;
	int r;
	switch (r = expr_is_unary_impl(context)) {
	case RETURN_ERROR:
	case RETURN_ACCEPT:
		return r;
	}
	t = util_read_first_token(context);
	switch (TOKEN_TYPE(t)) {
	case TKT_UNARY_OP_PLUS_PLUS:
	case TKT_UNARY_OP_MINUS_MINUS:
		util_shift_one_token(context);
		switch (r = expr_is_unary(context)) {
		case RETURN_ERROR:
			return r;
		case RETURN_ACCEPT:
			c = util_token_type_to_construct_op(TOKEN_TYPE(t),
							    NODE_TAG_UNARY);
			pcontext_reduce_unary(context, c);
			return r;
		case RETURN_REJECT:
			return pcontext_push_expected_error(context,
							    EXPECTED,
							    "unary expression",
							    AFTER,
							    "'++' or '--' token",
							    NULL, NULL);
		}
	case TKT_KW_SIZEOF:
		switch (r = expr_is_sizeof_(context)) {
		case RETURN_ERROR:
		case RETURN_ACCEPT:
			return r;
		case RETURN_REJECT:
			return pcontext_push_expected_error(context,
							    EXPECTED,
							    "typename or unary expression",
							    AFTER,
							    "'sizeof' token",
							    NULL, NULL);
		}
	default:
		return RETURN_REJECT;
	}
}

EXPR_IS_FUNC_DECLARE(cast)
{
	if (expr_is_unary(context)) {
		return true;
	}
	if (decl_is_typename_in_parenthesis(context)) {
		if (expr_is_cast(context)) {
			pcontext_reduce_binary(context, OP_C_CAST);
			return true;
		}
		die("cast: expected cast expression after '(typename)'");
	}
	return false;
}

static EXPR_IS_FUNC_DECLARE(ternary_seq)
{
	pcontext_set_pfunction_name(context, "ternary expression");
	return util_is_sequence(context, util_is_question,
				expr_is_expression, util_is_colon,
				expr_is_condition, NULL);
}

EXPR_IS_FUNC_DECLARE(condition)
{
	// ternary expression in C
	// if the '?' form is absent, it is just a
	// binary_node, otherwise, it will be a ternary_node
	// NOTICE;
	int r;
	switch (r = expr_is_log_or(context)) {
	case RETURN_ERROR:
	case RETURN_REJECT:
		return r;
	case RETURN_ACCEPT:
		switch (r = expr_is_ternary_seq(context)) {
		case RETURN_ACCEPT:
			pcontext_reduce_ternary(context);
			return r;
		case RETURN_ERROR:
			return r;
		case RETURN_REJECT:
			return RETURN_ACCEPT;
		}
	}
}

enum {
	ASSIGN_IMPL_ONE_NODE,
	ASSIGN_IMPL_THREE_NODE,
	ASSIGN_IMPL_ERROR,
};
int expr_is_assign_impl(pcontext * context)
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
	Token *t = util_read_first_token(context);
	int what;
	int r;
	Token *lookahead;
	if (!terminal_is_assign_op(t)) {
		return ASSIGN_IMPL_ONE_NODE;
	}
	util_shift_one_token(context);
	while (true) {
		switch (r = expr_is_unary(context)) {
		case RETURN_ACCEPT:
			t = util_read_first_token(context);
			if (terminal_is_assign_op(lookahead)) {
				what = TOKEN_TYPE(t);
				util_shift_one_token(context);
				pcontext_reduce_binary(context,
						       util_token_type_to_construct
						       (what));
				continue;
			}
			return ASSIGN_IMPL_THREE_NODE;
		case RETURN_REJECT:
		return}
		return ASSIGN_IMPL_ERROR;
	}
}

EXPR_IS_FUNC_DECLARE(assign)
{
	if (expr_is_unary(context)) {
		int r = (expr_is_assign_impl(context));
		if (r == ASSIGN_IMPL_ERROR) {
			die("assign: expected unary after assign operator");
		}
		pcontext_set_unary_ontop(context, true);
		if (expr_is_condition(context)) {
			switch (r) {
			case ASSIGN_IMPL_ONE_NODE:
				// the unary on the top was reduce to condition
				// directly.
				break;
			case ASSIGN_IMPL_THREE_NODE:
				// there is at least 3 nodes on top of stack;
				// and the top one is reduced to condition;
				reduce_binary(context);
				break;
			}
			return true;
		}
		die("assign: expected condition expression after assign operator");
	}
	return false;
}

EXPR_IS_FUNC_DECLARE(comma_assign_seq)
{
	pcontext_set_pfunction_name(context, "',' assignment expression");
	return util_is_sequence(context, util_is_comma, expr_is_assign,
				NULL);
}

EXPR_IS_FUNC_DECLARE(optional_constant)
{
	pcontext_set_pfunction_name(context,
				    "optional constant expression");
	return util_is_optional(context, expr_is_constant);
}

EXPR_IS_FUNC_DECLARE(optional_expr)
{
	pcontext_set_pfunction_name(context, "optional expression");
	return util_is_optional(context, expr_is_expression);
}

EXPR_IS_FUNC_DEFINE(timing, timing_op, cast, "timing", "cast");
EXPR_IS_FUNC_DEFINE(additive, additive_op, timing, "additive", "timing");
EXPR_IS_FUNC_DEFINE(shift, shift_op, additive, "shift", "additive");
EXPR_IS_FUNC_DEFINE(relation, relation_op, shift, "relation", "shift");
EXPR_IS_FUNC_DEFINE(equality, equality_op, relation, "equality",
		    "relation");
EXPR_IS_FUNC_DEFINE(bit_and, bit_and_op, equality, "bit_and", "equality");
EXPR_IS_FUNC_DEFINE(bit_xor, bit_xor_op, bit_and, "bit_xor", "bit_and");
EXPR_IS_FUNC_DEFINE(bit_or, bit_or_op, bit_xor, "bit_or", "bit_xor");
EXPR_IS_FUNC_DEFINE(log_and, log_and_op, bit_or, "log_and", "bit_or");
EXPR_IS_FUNC_DEFINE(log_or, log_or_op, log_and, "log_or", "log_and");
EXPR_IS_FUNC_DEFINE(expression, comma, assign, "expression", "assign");
EXPR_IS_FUNC_DECLARE(constant)
{
	pcontext_set_pfunction_name(context, "constant expression");
	return expr_is_condition(context);
}
