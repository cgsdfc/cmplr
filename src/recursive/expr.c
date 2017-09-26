#include "recursive/error.h"
#include "recursive/expr.h"
#include "recursive/terminal.h"
#include "lexer/lexer.h"
#include <stdarg.h>


EXPR_IS_FUNC_DECLARE(primary)
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
        LexerConsume(lexer);
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
      expected=TKT_RIGHT_PARENTHESIS;
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
    LexerConsume(lexer);
    switch(expected) {
      case TKT_RIGHT_BRACKET: // ']'
      case TKT_RIGHT_PARENTHESIS: // ')'
        return true;
      case TKT_IDENTIFIER:
        return true;
    }
  }
  die("expected identifier, ']' or ')' as part of postfix");
}

EXPR_IS_FUNC_DECLARE(postfix)
{
  if (expr_is_primary(lexer)) {
    while (expr_postfix_list(lexer));
    return true;
  }
  return false;
}

static bool
expr_unary_list(Lexer *lexer)
{

}

static bool
expr_is_sequence(Lexer *lexer, unsigned num, ...) 
{
  va_list ap;
  va_start(ap, num);
  for (int i=0;i<num;++i) {
    Token *t=LexerGetToken(lexer);
    TokenType expected=va_arg(ap, TokenType);
    if (expected==TOKEN_TYPE(t)) {
      LexerConsume(lexer);
      continue;
    }
    va_end(ap);
    return false;
  }
  va_end(ap);
  return true;
}

EXPR_IS_FUNC_DECLARE(unary)
{
  if (expr_is_primary(lexer)) {
    return true;
  }
  Token *t=LexerGetToken(lexer);
  if (terminal_is_unary_op(t)) {
    LexerConsume(lexer);
    if (expr_is_cast (lexer)) {
      return true;
    }
    die("unary: expected cast expression");
  }
  switch(TOKEN_TYPE(t)) {
      case TKT_UNARY_OP_PLUS_PLUS:
      case TKT_UNARY_OP_MINUS_MINUS:
        // ('++' | '--') unary
        LexerConsume(lexer);
        break;
      case TKT_KW_SIZEOF:
        LexerConsume(lexer);
        t=LexerGetToken(lexer);
        if (terminal_is_parenthesisL(t)) {
          if (expr_is_sequence(lexer, 2,
                TKT_IDENTIFIER, TKT_RIGHT_PARENTHESIS))
            // sizeof ( identifier )
          {
            return true;
          }
          die("unary: expected sizeof (identifier)");
        } 
        // sizeof unary
        break;
    }
  return expr_is_unary(lexer);
}

EXPR_IS_FUNC_DECLARE(cast)
{
  if (expr_is_unary(lexer)) {
    return true;
  }
  if (expr_is_sequence(lexer, 3,
        TKT_LEFT_PARENTHESIS,
        TKT_IDENTIFIER,
        TKT_RIGHT_PARENTHESIS))
  {
    return expr_is_cast(lexer);
  }
  return false;
}

EXPR_IS_FUNC_DEFINE(timing, timing_op, cast, "timing", "cast");
EXPR_IS_FUNC_DEFINE(additive, additive_op, timing, "additive", "timing");
EXPR_IS_FUNC_DEFINE(shift, shift_op, additive, "shift", "additive");
EXPR_IS_FUNC_DEFINE(relation,relation_op, shift, "relation", "shift");
EXPR_IS_FUNC_DEFINE(equality, equality_op, relation, "equality", "relation");
EXPR_IS_FUNC_DEFINE(bit_and, bit_and_op, equality, "bit_and", "equality");
EXPR_IS_FUNC_DEFINE(bit_xor, bit_xor_op, bit_and, "bit_xor", "bit_and");
EXPR_IS_FUNC_DEFINE(bit_or, bit_or_op, bit_xor, "bit_or", "bit_xor");
EXPR_IS_FUNC_DEFINE(log_and, log_and_op, bit_or, "log_and", "bit_or");
EXPR_IS_FUNC_DEFINE(log_or, log_or_op, log_and, "log_or", "log_and");
EXPR_IS_FUNC_DECLARE(condition)
{
  if (expr_is_log_or(lexer)) {
    Token *t=LexerGetToken(lexer);
    if (terminal_is_question(t)) {
      LexerConsume(lexer);
      if (expr_is_expression(lexer)) {
        t=LexerGetToken(lexer);
        if (terminal_is_colon(t)) {
          LexerConsume(lexer);
          return expr_is_condition(lexer);
        } else {
          die("condition: expected ':' after expression");
        }
      } else {
        die("condition: expected expression after '?' token");
      }
    } else {
      return true;
    }
  }
  return false;
}
EXPR_IS_FUNC_DECLARE(assign)
{
  if (expr_is_condition(lexer)) {
    return true;
  }
  if (expr_is_unary(lexer)) {
    Token *t=LexerGetToken(lexer);
    if (terminal_is_assign_op(t)){
      LexerConsume(lexer);
      return expr_is_assign(lexer);
    } else {
      die("assign: expected assign_op");
    }
  }
  return false;
}
EXPR_IS_FUNC_DEFINE(expression, comma, assign, "expression", "assign");
