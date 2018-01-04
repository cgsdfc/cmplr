#include "Parser.h"
#include "Token.h"
#include "Scanner.h"
#include "Symbol.h"
#include "AST.h"
#include "Error.h"
#include <stdlib.h>

extern "C" {
#include "Grammar.c"
}

Parser::Parser(Option const *option, ErrorManager *errorManager)
  :option(option), errorManager(errorManager), program(nullptr) {
  lemonImpl=ParseAlloc(malloc);
  ParseTrace(stderr, "(Lemon) ");
}

int Parser::ParseAll(Scanner * scanner) {
  int token;
  TokenValue *tokenValue;
  while ((token=scanner->GetToken())) {
    tokenValue=scanner->GetTokenValue(token);
    Parse(lemonImpl, token, tokenValue, this);
  }
  Parse(lemonImpl, 0, nullptr, this);
  if (errorManager->HasError())
    return -1;
  return 0;
}

Parser::~Parser() {
  delete program;
  ParseFree(lemonImpl, free);
}
