#include "recursive/util.h"
#include "recursive/context.h"
#include "terminal.h"		// make_terminal_node
#include "construct.h"
#include <stdarg.h>

static char const *
util_token_type_tostring (int tt)
{
  switch (tt)
    {
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

static bool
util_is_surrounded (pcontext * context, pfunction * parse,
		    int left, int right)
{
  if (util_is_terminal (context, left, false))
    {
      if (parse (context))
	{
	  if (util_is_terminal (context, right, false))
	    {
	      return true;
	    }
	}
    }
  return false;
}

void
util_push_node_null (pcontext * context)
{
  pcontext_push_node (context, make_nullary_node ());
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

bool
util_is_list (pcontext * context, pfunction * parse, bool allow_empty)
{
  size_t i = 0;
  vector_node *v = (vector_node *) make_vector_node ();
  while (parse (context))
    {
      node_base *x = pcontext_pop_node (context);
      vector_node_push_back (v, x);
      i++;
    }
  if (i)
    {
      pcontext_push_node (context, TO_NODE_BASE (v));
      return true;
    }
  if (allow_empty)
    {
      util_push_node_null (context);	// place holder;
      destroy_vector_node (v);
      return true;
    }
  return false;
}

bool
util_is_in_parentheses (pcontext * context, pfunction * parse)
{
  return util_is_surrounded (context, parse, TKT_LEFT_PARENTHESIS,
			     TKT_RIGHT_PARENTHESIS);
}

bool
util_is_in_braces (pcontext * context, pfunction * parse)
{
  return util_is_surrounded (context, parse, TKT_LEFT_BRACE, TKT_RIGHT_BRACE);
}

bool
util_is_in_brackets (pcontext * context, pfunction * parse)
{
  return util_is_surrounded (context, parse, TKT_LEFT_BRACKET,
			     TKT_RIGHT_BRACKET);
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

bool
util_is_comma (pcontext * context)
{
  // discard the colon
  return util_is_terminal (context, TKT_COMMA, false /* pushing */ );
}

bool
util_is_colon (pcontext * context)
{
  // discard the colon
  return util_is_terminal (context, TKT_COLON, false /* pushing */ );
}

bool
util_is_separated_list (pcontext * context, TokenType sep, pfunction * each,
			bool allow_empty)
{
  // this parser repeatedly applies util_is_terminal(sep, false)
  // and the parser specified by each.
  // it fails if one of the called to each failed, and succeeds if
  // one of the called to parse sep failed.
  if (each (context))
    {
      vector_node *v = (vector_node *) make_vector_node ();
      node_base *x = pcontext_pop_node (context);
      vector_node_push_back (v, x);
      while (util_is_terminal (context, sep, false /* pushing */ ))
	{
	  if (each (context))
	    {
	      x = pcontext_pop_node (context);
	      vector_node_push_back (v, x);
	      continue;
	    }
	  return false;
	}
      pcontext_push_node (context, TO_NODE_BASE (v));
      return true;
    }
  if (allow_empty)
    {
      util_push_node_null (context);
      return true;
    }
  return false;
}

bool
util_is_comma_sep_list (pcontext * context, pfunction * parse,
			bool allow_empty)
{
  return util_is_separated_list (context, TKT_COMMA, parse, allow_empty);
}

bool
util_is_parenthesisL (pcontext * context)
{
  return util_is_terminal (context, TKT_LEFT_PARENTHESIS, false);	// pushing
}

bool
util_is_parenthesisR (pcontext * context)
{
  return util_is_terminal (context, TKT_RIGHT_PARENTHESIS, false);	// pushing
}

bool
util_is_braceL (pcontext * context)
{
  return util_is_terminal (context, TKT_LEFT_BRACE, false);	// pushing
}

bool
util_is_braceR (pcontext * context)
{
  return util_is_terminal (context, TKT_RIGHT_BRACE, false);	// pushing
}


bool
util_is_bracketL (pcontext * context)
{
  return util_is_terminal (context, TKT_LEFT_BRACKET, false);	// pushing
}

bool
util_is_bracketR (pcontext * context)
{
  return util_is_terminal (context, TKT_RIGHT_BRACKET, false);	// pushing
}

bool
util_is_question (pcontext * context)
{
  return util_is_terminal (context, TKT_QUESTION, false /* pushing */ );
}

int
util_token_type_to_construct (int tt, int tag)
{

}
