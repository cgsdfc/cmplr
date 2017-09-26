#include "recursive/error.h"
#include "recursive/stmt.h"
#include "recursive/expr.h"
#include "recursive/terminal.h"
static void
optional_expression(Lexer *lexer)
{
  expr_is_expression(lexer);
}

static void
stmt_declaraton_list(Lexer *lexer)
{

}
static void
stmt_statement_list(Lexer *lexer)
{

}
STMT_IS_FUNC_DECLARE(jump)
{
  Token *t=LexerGetToken(lexer);
  switch(TOKEN_TYPE(t)) {
    case TKT_KW_GOTO:
      LexerConsume(lexer);
      t=LexerGetToken(lexer);
      if(terminal_is_identifier(t)) {
        LexerConsume(lexer);
        break;
      }
      die("jump: expected identifier after goto token");
    case TKT_KW_CONTINUE:
    case TKT_KW_BREAK:
      LexerConsume(lexer);
      break;
    case TKT_KW_RETURN:
      LexerConsume(lexer);
      optional_expression(lexer);
      break;
    default:
      die("jump: expected goto, continue, break or return token");
  }
  t=LexerGetToken(lexer);
  if (terminal_is_semicolon(t)) {
    LexerConsume(lexer);
    return true;
  }
  die("jump: expected ';' at the end of statement");
}

static bool
stmt_is_expr_in_parenthesis(Lexer *lexer)
{
  Token *t=LexerGetToken(lexer);
  if (terminal_is_parenthesisL(t)) {
    LexerConsume(lexer);
    if (expr_is_expression(lexer)) {
      t=LexerGetToken(lexer);
      if (terminal_is_parenthesisR(t)) {
        LexerConsume(lexer);
        return true;
      }
    }
  } return false;
}

static bool
stmt_is_expr_for(Lexer *lexer)
{
  Token *t=LexerGetToken(lexer);
  if (terminal_is_parenthesisL(t)) {
    LexerConsume(lexer);
    optional_expression(lexer); // for (expr
    for (int i=0;i<2;++i) {
      t=LexerGetToken(lexer);
      if (terminal_is_semicolon(t)) {
        LexerConsume(lexer);
        optional_expression(lexer);
        continue;
      } die("for: expected semicolon");
    }
    t=LexerGetToken(lexer);
    if (terminal_is_parenthesisR(t)) {
      LexerConsume(lexer);
      return true;
    }
  } return false;
}

static bool
stmt_is_expr_do(Lexer *lexer)
{
  Token *t=LexerGetToken(lexer);
  if (terminal_is_while(t)) {
    LexerConsume(lexer);
    return stmt_is_expr_in_parenthesis(lexer);
  }
  return false;
}

STMT_IS_FUNC_DECLARE(iterate)
{
  Token *t=LexerGetToken(lexer);
  switch(TOKEN_TYPE(t)) {
    case TKT_KW_WHILE:
      if (stmt_is_expr_in_parenthesis(lexer)) {
        return stmt_is_statement(lexer);
      } die("while: expected '(' expr ')' after 'while' token");
    case TKT_KW_FOR:
      if (stmt_is_expr_for(lexer)) {
        return stmt_is_statement(lexer);
      } die("for: expected '(' [expr];[expr];[expr] ')' after 'for' token");
    case TKT_KW_DO:
      if (stmt_is_statement(lexer)) {
        if (stmt_is_expr_do(lexer)) {
          t=LexerGetToken(lexer);
          if (terminal_is_semicolon(t)) {
            LexerConsume(lexer);
            return true;
          } die("do-while: expected ';' after while(expr)");
        } die("do-while: expected 'while(expr);' after do '{' statement '}'");
      } die("do-while: expected statement after 'do'");
    default:
      return false;
  }
}

STMT_IS_FUNC_DECLARE(select)
{
  Token *t=LexerGetToken(lexer);
  switch(TOKEN_TYPE(t)) {
    case TKT_KW_IF:
      if (stmt_is_expr_in_parenthesis(lexer)) {
        if (stmt_is_statement(lexer)) {
          t=LexerGetToken(lexer);
          if (terminal_is_else(t)) {
            LexerConsume(lexer);
            return stmt_is_statement(lexer);
          } return true;
        } die("if: expected statement after 'if (expr)'");
      } die("if: expected '(' expr ')' after 'if' token");
    case TKT_KW_SWITCH:
      if (stmt_is_expr_in_parenthesis(lexer)) {
        return stmt_is_statement(lexer);
      } die("switch: expected '(' expr ')' after 'switch' token");
    default:
      return false;
  }
}

STMT_IS_FUNC_DECLARE(label)
{
  Token *t=LexerGetToken(lexer);
  switch(TOKEN_TYPE(t)) {
    case TKT_KW_CASE:
      LexerConsume(lexer);
      if (expr_is_constant(lexer)) {
        break;
      } die("case: expected constant expression after 'case' token");
    case TKT_KW_DEFAULT:
      LexerConsume(lexer);
      break;
    case TKT_IDENTIFIER:
      LexerConsume(lexer);
      break;
    default:
      return false;
  }
  t=LexerGetToken(lexer);
  if (terminal_is_colon(t)) {
   LexerConsume(lexer);
    return stmt_is_statement(lexer);
  }
 die("label: expected ':' after default, case expr or identifier"); 
}

STMT_IS_FUNC_DECLARE(exprstmt)
{
  optional_expression(lexer);
  Token *t=LexerGetToken(lexer);
  if (terminal_is_semicolon(t)) {
    LexerConsume(lexer);
    return true;
  } die("exprstmt: expected ';' after expression");
}

STMT_IS_FUNC_DECLARE(compound)
{
  Token *t=LexerGetToken(lexer);
  if (terminal_is_braceL(t)) {
    LexerConsume(lexer);
    stmt_declaraton_list(lexer);
    stmt_statement_list(lexer);
    t=LexerGetToken(lexer);
    if (terminal_is_braceR(t)) {
      LexerConsume(lexer);
      return true;
    } die("compound: expected '}' after statement list");
  } return false;
}

STMT_IS_FUNC_DECLARE(statement)
{
  if (stmt_is_iterate(lexer)) { return true; }
  if (stmt_is_select(lexer)) { return true; }
  if (stmt_is_jump(lexer)) { return true; }
  if (stmt_is_label(lexer)) { return true; }
  if (stmt_is_compound(lexer)) { return true; }
  return stmt_is_exprstmt(lexer);
}
