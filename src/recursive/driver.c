#include "recursive/terminal.h"
#include "recursive/expr.h"

int main() {
  const char * code = "1234";
  Lexer *lexer = CreateLexerFromString(code);
  assert (expr_is_primary(lexer));
}
