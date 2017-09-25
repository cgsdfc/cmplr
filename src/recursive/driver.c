#include "recursive/terminal.h"
#include "recursive/expr.h"

int main() {
  const char * code = "index";
  Lexer *lexer = CreateLexerFromString(code);
  assert (expr_is_primary(lexer));
  DestroyLexer(lexer);
}
