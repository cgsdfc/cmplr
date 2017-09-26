#include "recursive/terminal.h"
#include "recursive/expr.h"

int
main ()
{
  const char *code = 
    "id--"
    ;
  Lexer *lexer = CreateLexerFromString (code);
  int i=0;
  while (expr_is_postfix(lexer)) {
    printf("case %d\n", i);
    i++;
  }
}
