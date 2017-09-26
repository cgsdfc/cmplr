#include "recursive/util.h"
#include "recursive/context.h"

#include <stdarg.h>
#define UTIL_DEFINE_SURROUND(NAME, LEFT, RIGHT)\
  static const util_surround NAME = {.left=LEFT, .right=RIGHT }

UTIL_DEFINE_SURROUND (PAIR_OF_PAREN, TKT_LEFT_PARENTHESIS,
		      TKT_RIGHT_PARENTHESIS);
UTIL_DEFINE_SURROUND (PAIR_OF_BRACE, TKT_LEFT_BRACE, TKT_RIGHT_BRACE);
UTIL_DEFINE_SURROUND (PAIR_OF_BRACKET, TKT_LEFT_BRACKET, TKT_RIGHT_BRACKET);

bool
util_is_optional (pfunction * parse, pcontext * context)
{
  if (!parse (context))
    {
      pcontext_push_node (context, NULL);
      // place holder;
    }
  return true;
}

bool
util_is_list (pfunction * parse, pcontext * context)
{
  while (parse (context))
    ;
  return true;
}

static bool
util_is_surrounded (pfunction * parse, pcontext * context,
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

bool
util_is_in_parentheses (pfunction * parse, struct pcontext * context)
{
  return util_is_surrounded (parse, context, &PAIR_OF_PAREN);
}

bool
util_is_in_braces (pfunction * parse, struct pcontext * context)
{
  return util_is_surrounded (parse, context, &PAIR_OF_BRACE);
}

bool
util_is_in_brackets (pfunction * parse, struct pcontext * context)
{
  return util_is_surrounded (parse, context, &PAIR_OF_BRACKET);
}

bool
util_is_sequence (pcontext * context, ...)
{
  va_list ap;
  va_start (ap, context);
  while (true)
    {
      pfunction *parse = va_arg (ap, pfunction *);
      if (!parse)
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
      if (!parse)
	{
	  return false;
	}
      if (parse (context))
	{
	  return true;
	}
    }
}

bool
util_is_terminal (pcontext * context, TokenType tt)
{
  if (tt == TOKEN_TYPE (pcontext_read_token (context, 0)))
    {
      pcontext_shift_token (context, 1);
      return true;
    }
  return false;
}
