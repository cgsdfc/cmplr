#include <stdlib.h>
#include <string.h>

#include "lexer/token.h"
#include "lexer/token_type.h"
#include "lexer/error.h"

int
alloc_buffer (token * tk, position * begin)
{
  tk->string = malloc (sizeof (char) * (TOKEN_VARLEN_INIT_LEN + 1));
  tk->len = 0;
  tk->max = TOKEN_VARLEN_INIT_LEN;
  if (!tk->string)
    {
      tknzr_error_set (ER_NOMEM);
      return -1;
    }
  memcpy (&tk->begin, begin, sizeof (position));
  return 0;
}


int
collect_char (token * tk, char ch)
{
  char *newspace;
  if (tk->len == tk->max - 1)
    {
      tk->max = 2 * (tk->len);
      newspace = malloc (sizeof (char) * tk->max);
      if (!newspace)
	{
	  tknzr_error_set (ER_NOMEM);
	  return -1;
	}

      memcpy (newspace, tk->string, sizeof (char) * (tk->len));
      newspace[tk->len++] = ch;
      free (tk->string);
      tk->string = newspace;
      return 0;
    }
  tk->string[tk->len++] = ch;
  return 0;

}

int
accept_char (token * tk, char ch)
{
  TERMINATE_STRING (tk);
  tk->type = TKT_CHAR_CONST;
  return 0;
}

int
accept_operator (token * tk, char ch)
{
  int type;
  TERMINATE_STRING (tk);
  type = lookup_special (FIRST_OPER, LAST_OPER, tk->string);
  if (type == TKT_UNKNOWN)
    {
      return -1;
    }
  tk->type = type;
  free (tk->string);
  MARK_NO_STRING (tk);
  return 0;
}

int
accept_identifier (token * tk, char ch)
{
  int type;
  TERMINATE_STRING (tk);
  type = lookup_special (FIRST_KW, LAST_KW, tk->string);
  if (type == TKT_UNKNOWN)
    {
      type = TKT_IDENTIFIER;
    }
  else
    {
      free (tk->string);
      MARK_NO_STRING (tk);
    }
  tk->type = type;
  return 0;
}

int
accept_float (token * tk, char ch)
{
  TERMINATE_STRING (tk);
  tk->type = TKT_FLOAT_CONST;
  return 0;
}

int
accept_integer (token * tk, char ch)
{
  TERMINATE_STRING (tk);
  tk->type = TKT_INT_CONST;
  return 0;
}

int
accept_punctuation (token * tk, char ch)
{
  int type;
  TERMINATE_STRING (tk);
  type = lookup_special (FIRST_PUNC, LAST_PUNC, tk->string);
  if (type == TKT_UNKNOWN)
    {
      return -1;
    }
  tk->type = type;
  free (tk->string);
  MARK_NO_STRING (tk);

  return 0;
}


int
accept_string (token * tk, char ch)
{
  TERMINATE_STRING (tk);
  tk->type = TKT_STRING_CONST;
  return 0;
}

char *
format_token (token * tk)
{
  assert (tk);
  extern const char *token_tab[];
  static char buf[BUFSIZ];
  const char *type_string = token_tab[TOKEN_TYPE (tk)];
  position *begin = &tk->begin;
  char *string = TOKEN_STRING (tk);

  snprintf (buf, BUFSIZ, "<%s> <%s> <%d>",
	    type_string, string, begin->lineno);

  return buf;
}

void
fini_token (token * tk)
{
  char *string;
  if (tk->string)
    {
      free (tk->string);
    }
  free (tk);
}

void
destroy_token (token * tk)
{
  free (tk->string);
  TERMINATE_STRING (tk);
}
