#include "recursive/util.h"
#include "recursive/context.h"
#include "terminal.h"		// make_terminal_node
#include <stdarg.h>
#define UTIL_DEFINE_SURROUND(NAME, LEFT, RIGHT)\
  static const util_surround NAME = {.left=LEFT, .right=RIGHT }

typedef struct util_surround
{
  TokenType left, right;
} util_surround;

UTIL_DEFINE_SURROUND (PAIR_OF_PAREN, TKT_LEFT_PARENTHESIS,
		      TKT_RIGHT_PARENTHESIS);
UTIL_DEFINE_SURROUND (PAIR_OF_BRACE, TKT_LEFT_BRACE, TKT_RIGHT_BRACE);
UTIL_DEFINE_SURROUND (PAIR_OF_BRACKET, TKT_LEFT_BRACKET, TKT_RIGHT_BRACKET);

static bool
util_is_surrounded (pcontext * context, pfunction * parse,
		    const util_surround * sur)
{
  Token *t = pcontext_read_token (context, 0);
  if (TOKEN_TYPE (t) == sur->left)
    {
      pcontext_shift_token (context, 1);
      if (parse (context))
	{
	  t = pcontext_read_token (context, 0);
	  if (TOKEN_TYPE (t) == sur->right)
	    {
	      pcontext_shift_token (context, 1);
	      return true;
	    }			// error
	}			// error
    }
  return false;
}

void
util_push_node_null (pcontext * context)
{
  // place holder;
  pcontext_push_node (context, make_nullary_node());
}

bool
util_is_optional (pcontext * context, pfunction * parse)
{
  if (parse (context))
    {
      return true;
    }
  util_push_node_null (context);
  return true;
}

static void
reduce_vector (pcontext * context, size_t size)
{
  vector_node *v = (vector_node *) make_vector_node ();
  for (int i = 0; i < size; ++i)
    {
      node_base *x = pcontext_pop_node (context);
      vector_node_push_back (v, x);
    }
  pcontext_push_node (context, TO_NODE_BASE (v));
}

bool
util_is_list (pcontext * context, pfunction * parse, bool allow_empty)
{
  size_t i = 0;
  while (parse (context))
    {
      i++;
    }
  if (i) { reduce_vector(context, i); return true; }
  if (allow_empty) {
    util_push_node_null(context); // place holder;
    return true;
  } return false;
}

bool
util_is_in_parentheses (pcontext * context, pfunction * parse)
{
  return util_is_surrounded (context, parse, &PAIR_OF_PAREN);
}

bool
util_is_in_braces (pcontext * context, pfunction * parse)
{
  return util_is_surrounded (context, parse, &PAIR_OF_BRACE);
}

bool
util_is_in_brackets (pcontext * context, pfunction * parse)
{
  return util_is_surrounded (context, parse, &PAIR_OF_BRACKET);
}

bool
util_is_sequence (pcontext * context, ...)
{
  va_list ap;
  va_start (ap, context);
  while (true)
    {
      pfunction *parse = va_arg (ap, pfunction *);
      if (NULL == parse)	// end of parser
	{
	  return true;
	}
      if (!parse (context))
	{
	  return false;
	}
    }
}

bool
util_is_one_of (pcontext * context, ...)
{
  va_list ap;
  va_start (ap, context);
  while (true)
    {
      pfunction *parse = va_arg (ap, pfunction *);
      if (NULL == parse)	// end of parser
	{
	  return false;
	}
      if (parse (context))
	{
	  return true;
	}
    }
}

static bool
util_is_terminal_success (pcontext * context, Token * t, bool pushing)
{
  pcontext_shift_token (context, 1);
  if (pushing)
    {
      pcontext_push_node (context, make_terminal_node (t));
    }
  return true;
}

Token *
util_read_first_token (pcontext * context)
{
  return pcontext_read_token (context, 0);
}

void
util_shift_one_token (pcontext * context)
{
  return pcontext_shift_token (context, 1);
}

bool				/* NOTICE: simple predicate can use this */
util_is_terminal (pcontext * context, TokenType expected, bool pushing)
{
  Token *t = util_read_first_token (context);
  if (expected == TOKEN_TYPE (t))
    {
      return util_is_terminal_success (context, t, pushing);
    }
  return false;
}

bool
util_is_terminal_pred (struct pcontext * context, tfunction *
		       pred, bool pushing)
{
  Token *t = util_read_first_token (context);
  if (pred (t))
    {
      return util_is_terminal_success (context, t, pushing);
    }
  return false;
}

bool
util_is_identifier (pcontext * context)
{
  return util_is_terminal (context, TKT_IDENTIFIER, true);
}

bool
util_is_semicolon (pcontext * context)
{
  return util_is_terminal (context, TKT_SEMICOLON, false);
}

bool
util_is_nonshortcut_or (pcontext * context, pfunction * first,
			pfunction * second)
{
  // this is a non-shortcut OR operation
  if (first (context))
    {
      if (second (context))
	{
	  // first, second;
	  return true;
	}
      else
	{
	  // first, NULL;
	  util_push_node_null (context);
	  return true;
	}
    }
  else
    {
      // NULL;
      util_push_node_null (context);
      if (second (context))
	{
	  // NULL, second;
	  return true;
	}
      else
	{
	  // the stack is unchanged
	  pcontext_pop_node (context);
	  /* pop null */
	  return false;
	}
    }
}

bool util_is_comma (pcontext *context)
{
  // discard the colon
  return util_is_terminal (context, TKT_COMMA, false /* pushing */ );
}

bool util_is_colon(pcontext *context)
{
  // discard the colon
  return util_is_terminal (context, TKT_COLON, false /* pushing */ );
}

