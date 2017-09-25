#include "recursive/error.h"
#include "recursive/expr.h"
#include "recursive/terminal.h"

  bool
expr_is_primary (Lexer * lexer)
{
  Token *t = LexerGetToken (lexer);
  if (terminal_is_primary (t))
  {
    LexerConsume (lexer);
    return true;
  }
  if (terminal_is_parenthesisL (t))
  {
    LexerConsume(lexer);
    if (expr_is_expression (lexer))
    {
      Token *expected = LexerGetToken (lexer);
      if (terminal_is_parenthesisR (expected))
      {
        return true;
      }
      die ("error: primary: expected right parenthesis");
    }
    die ("error: primary: expected expression");
  }
  return false;
}
  static void
expr_optional_list(Lexer *lexer)
{

}

  static bool
expr_postfix_list(Lexer * lexer)
{
  Token *t = LexerGetToken(lexer);
  TokenType expected;
  switch (TOKEN_TYPE(t)) {
    case TKT_LEFT_BRACKET:
      LexerConsume(lexer); // '['
      if (expr_is_expression(lexer)) {
        expected=TKT_RIGHT_BRACKET;
      } 
      break;
    case TKT_LEFT_PARENTHESIS:
      LexerConsume(lexer); // '('
      expr_optional_list(lexer); // optional list of arguments
      break;
    case TKT_DOT:
    case TKT_BINARY_OP_MEMBER_ARROW:
      LexerConsume(lexer); // '.', '->'
      expected=TKT_IDENTIFIER;
      break;
    case TKT_UNARY_OP_PLUS_PLUS: // '++'
    case TKT_UNARY_OP_MINUS_MINUS: // '--'
      LexerConsume(lexer);
      return true;
    default:
      return false;
  }
  t=LexerGetToken(lexer);
  if (TOKEN_TYPE(t) == expected) {
    switch(expected) {
      case TKT_RIGHT_BRACKET:
      case TKT_RIGHT_BRACE:
        return true;
      case TKT_IDENTIFIER:
        return true;
    }
  }

  die("expected identifier, ']' or ')' as part of postfix");
}

  bool
expr_is_postfix(Lexer * lexer)
{
  if (expr_is_primary(lexer)) {
    while (expr_postfix_list(lexer));
    return true;
  }
  return false;
}


bool
expr_is_expression (Lexer * lexer)
{
  return true;
}
