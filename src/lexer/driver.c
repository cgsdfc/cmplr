#include "lexer/lexer.h"

int main(int ac, char **av) {
  if (ac != 2) {
    printf("Usage: lexer <file>\n");
    exit(0);
  }
  Lexer *lexer = CreateLexerFromFile(av[1]);
  exit(LexerPrintToken(lexer));
}
