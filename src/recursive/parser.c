#include "recursive/parser.h"
#include "recursive/expr.h"
#include "recursive/stmt.h"
#include "recursive/error.h"
#include <stdlib.h>		// malloc

Parser *
CreateParserFromString (char *string)
{
  Lexer *lexer = CreateLexerFromString (string);
  Parser *parser = malloc (sizeof *parser);
  pcontext_init (parser, lexer);
  return parser;
}

Parser *
CreateParserFromFile (char *file)
{
  Lexer *lexer = CreateLexerFromFile (file);
  Parser *parser = malloc (sizeof *parser);
  pcontext_init (parser, lexer);
  return parser;
}

void
DestroyParser (Parser * parser)
{
  DestroyLexer (parser->lexer);
  free (parser);
}

bool
ParserDoParsing (Parser * parser)
{
  if (!expr_is_expression (parser))
    return false;
  Token *t = pcontext_read_token (parser, 0);
  return TOKEN_TYPE (t) == TKT_EOF;
}

AstNode *
ParserGetRoot (Parser * parser)
{
  return pcontext_top_node (parser);
}
