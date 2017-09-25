#include "recursive/terminal.h"
#include "recursive/expr.h"

int
main ()
{
  const char *code = 
    "identifier   \"hello\"    111111    2.3444 ()"
    ;
  Lexer *lexer = CreateLexerFromString (code);
  int i=0;
  while (expr_is_primary(lexer)) {
    printf("case %d\n", i);
    i++;
  }
}
