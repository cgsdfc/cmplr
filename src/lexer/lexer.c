#include "lexer/lexer.h"
#include "lexer/char_buffer.h"
#include "lexer/chcl.h"
#include "lexer/dfa.h"
#include "lexer/token_type.h"

typedef enum char_source
{
  CHAR_SOURCE_FILE,
  CHAR_SOURCE_STRING
} char_source;

static Lexer *
after_create_char_buffer (char_buffer * cb)
{
  Lexer *tb = malloc (sizeof *tb);
  if (!tb)
    {
      free (cb);
      return NULL;
    }
  init_token_list (tb, cb);
  return tb;
}

static char_buffer *
before_create_char_buffer (char *data, char_source src)
{
  char_buffer *cb = malloc (sizeof *cb);
  if (!cb)
    {
      return NULL;
    }
  int r;
  switch (src)
    {
    case CHAR_SOURCE_FILE:
      r = init_char_buffer_from_file (cb, data);
      break;
    case CHAR_SOURCE_STRING:
      r = init_char_buffer_from_string (cb, data);
      break;
    }
  if (r == 0)
    {
      return cb;
    }
  free (cb);
  return NULL;
}

Lexer *
CreateLexerFromFile (char *file)
{
  char_buffer *cb = before_create_char_buffer (file, CHAR_SOURCE_FILE);
  if (!cb)
    return NULL;
  return after_create_char_buffer (cb);

}

Lexer *
CreateLexerFromString (char *str)
{
  char_buffer *cb = before_create_char_buffer (str, CHAR_SOURCE_STRING);
  if (!cb)
    return NULL;
  return after_create_char_buffer (cb);
}

void
LexerError (Lexer * lexer)
{
  tknzr_error_handle (lexer->cb);
}

Token *
LexerGetToken (Lexer * lexer)
{
  static Token eof = {.type = TKT_EOF,.string = 0 };
  static Token unknown = {.type = TKT_UNKNOWN,.string = 0 };
  Token *tk;
  switch (get_token (lexer, &tk))
    {
    case 0:
      return tk;
      break;
    case EOF:
      return &eof;
      break;
    case 1:
    default:
      return &unknown;
      break;
    }
}

void
DestroyLexer (Lexer * lexer)
{
  destroy_char_class ();
  destroy_token_list (lexer);
  destroy_all_dfa ();
  destroy_char_buffer (lexer->cb);
  free (lexer->cb);
  free (lexer);
}

int
LexerPrintToken (Lexer * lexer)
{
  /* token *tk; */
  token *tk;
  while (true)
    {
      switch (get_token (lexer, &tk))
	{
	case 0:
	  puts (format_token (tk));
	  continue;
	case EOF:
	  return 0;
	case 1:
	  LexerError (lexer);
	  return 1;
	}
    }
  return 0;
}

const char **
LexerTerminalTab (void)
{
  return NULL;
}

const char *
LexerTerminalString (Token * t)
{
  return token_type_tostring (TOKEN_TYPE (t));
}
