#include "recursive/error.h"
#include "recursive/expr.h"
#include "recursive/terminal.h"

bool expr_is_primary (Lexer *lexer)
{
  Token *t = LexerGetToken(lexer);
  if (terminal_is_primary (t)) {
    LexerConsume(lexer);
    return true;
  }
  if (terminal_is_parenthesisL(t)) {
    if (expr_is_expression(lexer)) {
      Token *expected= LexerGetToken(lexer);
      if (terminal_is_parenthesisR(expected)) {
        return true;
      }
      die("error: primary: expected right parenthesis");
    }
    die("error: primary: expected expression");
  }
  return false;
}

bool expr_is_expression(Lexer *lexer)
{
  return true;
}
