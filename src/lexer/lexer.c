#include "lexer/lexer.h"
#include "lexer/char_buffer.h"
typedef enum char_source
{
  CHAR_SOURCE_FILE,
  CHAR_SOURCE_STRING
} char_source;

static Lexer *
after_create_char_buffer (char_buffer * cb)
{
  if (!cb)
    return NULL;
  Lexer *tb = malloc (sizeof *tb);
  if (!tb)
    {
      free (cb);
      return NULL;
    }
  if (0 != init_token_list (tb, cb))
    {
      free (cb);
      free (tb);
      return NULL;
    }
  return tb;
}

static char_buffer *
before_create_char_buffer (const char *data, char_source src)
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
    default:
      return NULL;
    }
  if (r == 0)
    {
      return cb;
    }
  free (cb);
  return NULL;
}

static Lexer *
create_lexer_from_file (const char *file)
{
  char_buffer *cb = before_create_char_buffer (file, CHAR_SOURCE_FILE);
  return after_create_char_buffer (cb);

}

static Lexer *
create_lexer_from_string (const char *str)
{
  char_buffer *cb = before_create_char_buffer (str, CHAR_SOURCE_STRING);
  return after_create_char_buffer (cb);
}

inline Lexer *
CreateLexerFromFile (const char *file)
{
  return create_lexer_from_file (file);
}

inline Lexer *
CreateLexerFromString (const char *str)
{
  return create_lexer_from_string (str);
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
LexerConsume (Lexer * lexer)
{
  consume_token (lexer);
}

void
DestroyLexer (Lexer * lexer)
{

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
	  consume_token (lexer);
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
