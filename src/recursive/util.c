#include "recursive/util.h"
#include "construct.h"
#include "recursive/context.h"
#include "terminal.h" // make_terminal_node
#include <stdarg.h>

static char const *util_token_type_tostring(int tt) {
  switch (tt) {
  case TKT_LEFT_PARENTHESIS:
    return "(";
  case TKT_RIGHT_PARENTHESIS:
    return ")";
  case TKT_LEFT_BRACE:
    return "{";
  case TKT_RIGHT_BRACE:
    return "}";
  case TKT_LEFT_BRACKET:
    return "[";
  case TKT_RIGHT_BRACKET:
    return "]";
  default:
    return "(null)";
  }
}

static int util_is_surrounded(pcontext *context, pfunction *parse, int left,
                              int right) {
  if (util_is_terminal(context, left, false)) // pushing
  {
    switch (parse->apply(context)) {
    case RETURN_ERROR:
      return RETURN_ERROR;
    case RETURN_ACCEPT:
      break;
    case RETURN_REJECT:
      pcontext_push_expected_error(
          context, ERR_EXPECTED, pcontext_get_pfunction_name(context),
          ERR_AFTER, util_token_type_tostring(left), NULL, NULL);
      return RETURN_ERROR;
    }
    if (util_is_terminal(context, right, false)) {
      return RETURN_ACCEPT;
    }
    pcontext_push_expected_error(
        context, ERR_EXPECTED, util_token_type_tostring(right), ERR_AFTER,
        pcontext_get_pfunction_name(context), NULL, NULL);
    return RETURN_ERROR;
  }
  return RETURN_REJECT; // left unmatched, rejected
} // end of util_is_surrounded

void util_push_node_null(pcontext *context) {
  pcontext_push_node(context, make_nullary_node());
}

int util_is_optional(pcontext *context, pfunction *parse) {
  switch (parse(context)) {
  case RETURN_ACCEPT:
    return RETURN_ACCEPT;
  case RETURN_REJECT:
    util_push_node_null(context);
    return RETURN_ACCEPT;
  case RETURN_ERROR:
    return RETURN_ERROR;
  default:
    assert(false);
  }
}

int util_is_list(pcontext *context, pfunction *parse, size_t least) {
  while (true) {
    switch (parse(context)) {
      case size_t i = 0;
      vector_node *v = (vector_node *) make_vector_node();
      while (parse(context)) {
	node_base *x = pcontext_pop_node(context);
	vector_node_push_back(v, x);
	i++;
      }
      if (i) {
	pcontext_push_node(context, TO_NODE_BASE(v));
	return true;
      }
      if (allow_empty) {
	util_push_node_null(context);	// place holder;
	destroy_vector_node(v);
	return true;
      }
      return false;
    }

    int util_is_in_parentheses(pcontext * context, pfunction * parse) {
      return util_is_surrounded(context, parse,
				TKT_LEFT_PARENTHESIS,
				TKT_RIGHT_PARENTHESIS);
    }

    int util_is_in_braces(pcontext * context, pfunction * parse) {
      return util_is_surrounded(context, parse,
				TKT_LEFT_BRACE, TKT_RIGHT_BRACE);
    }

    int util_is_in_brackets(pcontext * context, pfunction * parse) {
      return util_is_surrounded(context, parse,
				TKT_LEFT_BRACKET, TKT_RIGHT_BRACKET);
    }

    int util_is_sequence(pcontext * context, ...) {
      va_list ap;
      va_start(ap, context);
      pfunction *parse = va_arg(ap, pfunction *);
      while (parse != NULL) {
	switch (parse(context)) {
	case RETURN_ERROR:
        return RETURN_ERROR;
      case RETURN_ACCEPT:
        continue;
      case RETURN_REJECT:
        return RETURN_ACCEPT;
      }
      parse = va_arg(ap, pfunction *);
  }
  return RETURN_ACCEPT;
}

int util_is_one_of(pcontext *context, ...) {
  va_list ap;
  va_start(ap, context);
  while (true) {
    pfunction *parse = va_arg(ap, pfunction *);
    if (NULL == parse) // end of parser
    {
      return false;
    }
    if (parse(context)) {
      return true;
    }
  }
}

static int util_is_terminal_success(pcontext *context, Token *t, int pushing) {
  pcontext_shift_token(context, 1);
  if (pushing) {
    pcontext_push_node(context, make_terminal_node(t));
  }
  return true;
}

Token *util_read_first_token(pcontext *context) {
  return pcontext_read_token(context, 0);
}

void util_shift_one_token(pcontext *context) {
  return pcontext_shift_token(context, 1);
}

int /* NOTICE: simple predicate can use this */
    util_is_terminal(pcontext *context, TokenType expected, int pushing) {
  Token *t = util_read_first_token(context);
  if (expected == TOKEN_TYPE(t)) {
    return util_is_terminal_success(context, t, pushing);
  }
  return false;
}

int util_is_terminal_pred(struct pcontext *context, tfunction *pred,
                          int pushing) {
  Token *t = util_read_first_token(context);
  if (pred(t)) {
    return util_is_terminal_success(context, t, pushing);
  }
  return false;
}

int util_is_identifier(pcontext *context) {
  return util_is_terminal(context, TKT_IDENTIFIER, true);
}

int util_is_semicolon(pcontext *context) {
  return util_is_terminal(context, TKT_SEMICOLON, false);
}

int util_is_comma(pcontext *context) {
  // discard the colon
  return util_is_terminal(context, TKT_COMMA, false /* pushing */);
}

int util_is_colon(pcontext *context) {
  // discard the colon
  return util_is_terminal(context, TKT_COLON, false /* pushing */);
}

int util_is_separated_list_reduce_binary(pcontext *context, TokenType sep,
                                         pfunction *each) {
  // this parser repeatedly applies util_is_terminal(sep, false)
  // and the parser specified by each.
  // it fails if one of the called to each failed, and succeeds if
  // one of the called to parse sep failed.
  switch (each(context)) {
  case RETURN_ERROR:
    return RETURN_ERROR;
  case RETURN_REJECT:
    return RETURN_REJECT;
  case RETURN_ACCEPT:
    while (util_is_terminal(context, sep)) {
      switch (each(context)) {
      case RETURN_REJECT:
        return RETURN_ACCEPT;
      case RETURN_ACCEPT:
        pcontext_reduce_binaryT(context, tag);
        continue;
      case RETURN_ERROR:
        return RETURN_ERROR;
      }
    }
  }
}

    static void
     reduce_vector(int
		   util_is_comma_sep_list(pcontext * context,
					  pfunction * parse,
					  int allow_empty) {
  return util_is_separated_list(context, TKT_COMMA, parse, allow_empty);}

		   int util_is_parenthesisL(pcontext * context) {
  return util_is_terminal(context, TKT_LEFT_PARENTHESIS, false); // pushing
		   }

		   int util_is_parenthesisR(pcontext * context) {
  return util_is_terminal(context, TKT_RIGHT_PARENTHESIS, false); // pushing
		   }

		   int util_is_braceL(pcontext * context) {
  return util_is_terminal(context, TKT_LEFT_BRACE, false); // pushing
		   }

		   int util_is_braceR(pcontext * context) {
  return util_is_terminal(context, TKT_RIGHT_BRACE, false); // pushing
		   }


		   int util_is_bracketL(pcontext * context) {
  return util_is_terminal(context, TKT_LEFT_BRACKET, false); // pushing
		   }

		   int util_is_bracketR(pcontext * context) {
  return util_is_terminal(context, TKT_RIGHT_BRACKET, false); // pushing
		   }

		   int util_is_question(pcontext * context) {
  return util_is_terminal(context, TKT_QUESTION, false /* pushing */
                          );}

#define UTIL_TKT_TO_CTR_BEGIN(E) switch (E) {
#define UTIL_TKT_TO_CTR_END(E)                                                 \
  default:                                                                     \
    assert(false);                                                             \
    }
#define UTIL_TKT_TO_CTR_CASE(LABEL, RETURN)                                    \
  case (LABEL):                                                                \
    return (RETURN);

		   static int
		   util_token_type_to_construct_impl(int tt, int tag) {
  UTIL_TKT_TO_CTR_BEGIN(tt)
case TKT_AMPERSAND:
  UTIL_TKT_TO_CTR_BEGIN(tag)
  UTIL_TKT_TO_CTR_CASE(NODE_TAG_UNARY, OP_ADDROF)
  UTIL_TKT_TO_CTR_CASE(NODE_TAG_BINARY, OP_BIT_AND)
  UTIL_TKT_TO_CTR_END(tag)
case TKT_STAR:
  UTIL_TKT_TO_CTR_BEGIN(tag)
  UTIL_TKT_TO_CTR_CASE(NODE_TAG_UNARY, OP_DEREF)
  UTIL_TKT_TO_CTR_CASE(NODE_TAG_BINARY, OP_MUL)
  UTIL_TKT_TO_CTR_END(tag)
case TKT_PLUS:
  UTIL_TKT_TO_CTR_BEGIN(tag)
  UTIL_TKT_TO_CTR_CASE(NODE_TAG_UNARY, OP_POSIV)
  UTIL_TKT_TO_CTR_CASE(NODE_TAG_BINARY, OP_ADD)
  UTIL_TKT_TO_CTR_END(tag)
case TKT_MINUS:
  UTIL_TKT_TO_CTR_BEGIN(tag)
  UTIL_TKT_TO_CTR_CASE(NODE_TAG_UNARY, OP_NEGAV)
  UTIL_TKT_TO_CTR_CASE(NODE_TAG_BINARY, OP_SUB)
  UTIL_TKT_TO_CTR_END(tag)
  UTIL_TKT_TO_CTR_END(tt)}

		   int util_token_type_to_construct(int tt) {
  return util_token_type_to_construct_impl(tt, -1);}

		   int util_token_type_to_construct_op(int tt, int tag) {
  return util_token_type_to_construct_impl(tt, tag);}

/* UTILLIB_ENUM_BEGIN (construct) */
/* UTILLIB_ENUM_ELEM (TYPE_S_CHAR) */
/* UTILLIB_ENUM_ELEM (TYPE_U_CHAR) */
/* UTILLIB_ENUM_ELEM (TYPE_S_INT) */
/* UTILLIB_ENUM_ELEM (TYPE_U_INT) */
/* UTILLIB_ENUM_ELEM (TYPE_S_SHORT) */
/* UTILLIB_ENUM_ELEM (TYPE_U_SHORT) */
/* UTILLIB_ENUM_ELEM (TYPE_S_LONG) */
/* UTILLIB_ENUM_ELEM (TYPE_U_LONG) */
/* UTILLIB_ENUM_ELEM (TYPE_S_LL) */
/* UTILLIB_ENUM_ELEM (TYPE_U_LL) */
/* UTILLIB_ENUM_ELEM (TYPE_FLOAT) */
/* UTILLIB_ENUM_ELEM (TYPE_DOUBLE) */
/* UTILLIB_ENUM_ELEM (TYPE_LDOUBLE) */
/* UTILLIB_ENUM_ELEM (TYPE_VOID) */
/* UTILLIB_ENUM_ELEM (TYPE_ENUM) */
/* UTILLIB_ENUM_ELEM (TYPE_UNION) */
/* UTILLIB_ENUM_ELEM (TYPE_STRUCT) */
/* UTILLIB_ENUM_ELEM (STMT_FOR) */
/* UTILLIB_ENUM_ELEM (STMT_WHILE) */
/* UTILLIB_ENUM_ELEM (STMT_DO_WHILE) */
/* UTILLIB_ENUM_ELEM (STMT_SWITCH) */
/* UTILLIB_ENUM_ELEM (STMT_IF) */
/* UTILLIB_ENUM_ELEM (STMT_ELSE) */
/* UTILLIB_ENUM_ELEM (STMT_CASE) */
/* UTILLIB_ENUM_ELEM (STMT_DEFAULT) */
/* UTILLIB_ENUM_ELEM (STMT_GOTO) */
/* UTILLIB_ENUM_ELEM (STMT_RETURN) */
/* UTILLIB_ENUM_ELEM (STMT_CONTINUE) */
/* UTILLIB_ENUM_ELEM (STMT_BREAK) */
/* UTILLIB_ENUM_ELEM (STMT_COMPOUND) */
/* UTILLIB_ENUM_ELEM (STMT_LABEL) */
/* UTILLIB_ENUM_ELEM (DECL_FUNCTION) */
/* UTILLIB_ENUM_ELEM (DECL_DECLARE) */
/* UTILLIB_ENUM_ELEM (OP_INVOKE) */
/* UTILLIB_ENUM_ELEM (OP_SUBSCRIPT) */
/* UTILLIB_ENUM_ELEM (OP_PTR_ACCESS) */
/* UTILLIB_ENUM_ELEM (OP_DOT_ACCESS) */
/* UTILLIB_ENUM_ELEM (OP_POST_INC) */
/* UTILLIB_ENUM_ELEM (OP_PREF_INC) */
/* UTILLIB_ENUM_ELEM (OP_POST_DEC) */
/* UTILLIB_ENUM_ELEM (OP_PREF_DEC) */
/* UTILLIB_ENUM_ELEM (OP_SIZEOF_TYPE) */
/* UTILLIB_ENUM_ELEM (DECL_TYPENAME) */
/* UTILLIB_ENUM_ELEM (OP_SIZEOF_UNARY) */
/* UTILLIB_ENUM_ELEM (OP_C_CAST) */
/* UTILLIB_ENUM_ELEM (OP_ADD) */
/* UTILLIB_ENUM_ELEM (OP_DEREF) UTILLIB_ENUM_END (construct) */
/* UTILLIB_ENUM_ELEM (EXPR_PRIMARY) */
/* #endif // RECURSIVE_ENUM_H */
