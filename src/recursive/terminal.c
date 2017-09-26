#include "recursive/terminal.h"

static terminal_node *
make_terminal_node (Token * tk)
{
  terminal_node *node;
  node = malloc (sizeof *node);
  if (node)
    {
      init_node_base (&node->base, NODE_TAG_TERMINAL, "terminal_node");
      node->token = tk;
    }
  return node;
}

static TokenType
get_toke_type_from_node (void *node /* terminal_node */ )
{
  if (node)
    {
      terminal_node *tnode = node;
      return TOKEN_TYPE (tnode->token);
    }
  return TKT_UNKNOWN;
}

/* bool terminal_parser(parser_info * info) */
/* { */
/*   Token * tk = LexerGetToken(info->lexer); */
/*   void * node; */
/*   if (tk) { */
/*     node = make_terminal_node(tk); */
/*     if (node) { */
/*       info->node=node; */
/*       LexerConsume(info->lexer); */
/*       return true; */
/*     } */
/*   } */
/*   return false; */
/* } */
TERMINAL_DEFINE_PRED (int_const, TKT_INT_CONST);
TERMINAL_DEFINE_PRED (string_const, TKT_STRING_CONST);
TERMINAL_DEFINE_PRED (float_const, TKT_FLOAT_CONST);

TERMINAL_DEFINE_PRED (auto, TKT_KW_AUTO);
TERMINAL_DEFINE_PRED (static, TKT_KW_STATIC);
TERMINAL_DEFINE_PRED (typedef, TKT_KW_TYPEDEF);
TERMINAL_DEFINE_PRED (register, TKT_KW_REGISTER);

TERMINAL_DEFINE_PRED (const, TKT_KW_CONST);
TERMINAL_DEFINE_PRED (volatile, TKT_KW_VOLATILE);

TERMINAL_DEFINE_PRED (parenthesisL, TKT_LEFT_PARENTHESIS);
TERMINAL_DEFINE_PRED (parenthesisR, TKT_RIGHT_PARENTHESIS);
TERMINAL_DEFINE_PRED(identifier, TKT_IDENTIFIER);
TERMINAL_DEFINE_PRED(while, TKT_KW_WHILE);
TERMINAL_DEFINE_PRED(else, TKT_KW_ELSE);
TERMINAL_DEFINE_PRED(braceL, TKT_LEFT_BRACE);
TERMINAL_DEFINE_PRED(braceR, TKT_RIGHT_BRACE);

TERMINAL_DECLARE_PRED(type_qualifier)
{
  return terminal_is_const (t) || terminal_is_volatile (t);
}

TERMINAL_DECLARE_PRED(primary)
{
  return terminal_is_identifier (t)
    || terminal_is_int_const (t)
    || terminal_is_float_const (t) || terminal_is_string_const (t);
}

TERMINAL_DECLARE_PRED(storage_specifier)
{
  return terminal_is_auto (t)
    || terminal_is_static (t)
    || terminal_is_typedef (t) || terminal_is_register (t);
}

TERMINAL_DECLARE_PRED(additive_op)
{
  TokenType tt=TOKEN_TYPE(t);
  return  tt == TKT_PLUS || tt == TKT_MINUS;
}


TERMINAL_DECLARE_PRED(timing_op)
{
  TokenType tt=TOKEN_TYPE(t);
  return  tt == TKT_STAR || tt == TKT_BINARY_OP_DIV;
}

TERMINAL_DECLARE_PRED(unary_op)
{
  TokenType tt=TOKEN_TYPE(t);
  return tt == TKT_AMPERSAND || tt == TKT_STAR ||
  tt == TKT_PLUS || tt == TKT_MINUS || tt == TKT_UNARY_OP_BIT_NOT
  || tt == TKT_UNARY_OP_LOGICAL_NOT;
}

TERMINAL_DECLARE_PRED(shift_op)
{
  TokenType tt=TOKEN_TYPE(t);
  return tt == TKT_BINARY_OP_BIT_LEFT_SHIFT ||
  tt == TKT_BINARY_OP_BIT_RIGHT_SHIFT;
}

TERMINAL_DECLARE_PRED(relation_op)
{
  TokenType tt=TOKEN_TYPE(t);
  return tt == TKT_BINARY_OP_CMP_LESS ||
  tt == TKT_BINARY_OP_CMP_GREATER ||
  tt == TKT_BINARY_OP_CMP_LESS_EQUAL ||
  tt == TKT_BINARY_OP_CMP_GREATER_EQUAL;
}

TERMINAL_DECLARE_PRED(equality_op)
{
  return TOKEN_TYPE(t) == TKT_BINARY_OP_CMP_NOT_EQUAL ||
  TOKEN_TYPE(t) == TKT_BINARY_OP_CMP_EQUAL;
}

TERMINAL_DEFINE_PRED(bit_and_op,  TKT_BINARY_OP_BIT_AND);
TERMINAL_DEFINE_PRED(bit_or_op, TKT_BINARY_OP_BIT_OR);
TERMINAL_DEFINE_PRED(bit_xor_op, TKT_BINARY_OP_BIT_XOR);
TERMINAL_DEFINE_PRED(log_and_op,  TKT_BINARY_OP_LOGICAL_AND);
TERMINAL_DEFINE_PRED(log_or_op, TKT_BINARY_OP_LOGICAL_OR);
TERMINAL_DEFINE_PRED(comma, TKT_COMMA);
TERMINAL_DEFINE_PRED(colon, TKT_COLON);
TERMINAL_DEFINE_PRED(question, TKT_QUESTION);
TERMINAL_DECLARE_PRED(assign_op)
{
  TokenType tt=TOKEN_TYPE(t);
  return tt==TKT_BINARY_OP_ASSIGN || tt==TKT_BINARY_OP_MUL_ASSIGN
  || tt==TKT_BINARY_OP_DIV_ASSIGN || tt==TKT_BINARY_OP_ADD_ASSIGN
  || tt==TKT_BINARY_OP_SUB_ASSIGN || tt==TKT_BINARY_OP_BIT_LEFT_SHIFT_ASSIGN
  || tt==TKT_BINARY_OP_BIT_RIGHT_SHIFT_ASSIGN || tt==TKT_BINARY_OP_BIT_AND_ASSIGN
  || tt==TKT_BINARY_OP_BIT_XOR_ASSIGN || tt==TKT_BINARY_OP_BIT_XOR_ASSIGN
  ;
}


