#include "recursive/parser.h"
#include "recursive/decl.c"
#include "recursive/error.h"
#include "recursive/expr.h"
#include "recursive/stmt.h"
#include <stdlib.h> // malloc

Parser *CreateParserFromString(char *string) {
  Lexer *lexer = CreateLexerFromString(string);
  Parser *parser = malloc(sizeof *parser);
  pcontext_init(parser, lexer);
  return parser;
}

Parser *CreateParserFromFile(char *file) {
  Lexer *lexer = CreateLexerFromFile(file);
  Parser *parser = malloc(sizeof *parser);
  pcontext_init(parser, lexer);
  return parser;
}

void DestroyParser(Parser *parser) {
  DestroyLexer(parser->lexer);
  free(parser);
}

static bool ParserImpl(Parser *parser) { return decl_is_declare(parser); }

bool ParserDoParsing(Parser *parser) {
  if (!ParserImpl(parser))
    return false;
  Token *t = pcontext_read_token(parser, 0);
  return TOKEN_TYPE(t) == TKT_EOF;
}

AstNode *ParserGetRoot(Parser *parser) {
  assert(pcontext_node_size(parser) == 1);
  return pcontext_top_node(parser);
}
