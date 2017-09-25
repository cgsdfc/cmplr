#include "recursive/terminal.h"

#define TERMINAL_PREDICATE(FUNC, TYPE) \
bool terminal_ ## FUNC(Token * t) { return TOKEN_TYPE(t) == ( TYPE ); }

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
TERMINAL_PREDICATE (is_identifier, TKT_IDENTIFIER);
TERMINAL_PREDICATE (is_int_const, TKT_INT_CONST);
TERMINAL_PREDICATE (is_string_const, TKT_STRING_CONST);
TERMINAL_PREDICATE (is_float_const, TKT_FLOAT_CONST);

TERMINAL_PREDICATE (is_auto, TKT_KW_AUTO);
TERMINAL_PREDICATE (is_static, TKT_KW_STATIC);
TERMINAL_PREDICATE (is_typedef, TKT_KW_TYPEDEF);
TERMINAL_PREDICATE (is_register, TKT_KW_REGISTER);

TERMINAL_PREDICATE (is_const, TKT_KW_CONST);
TERMINAL_PREDICATE (is_volatile, TKT_KW_VOLATILE);

TERMINAL_PREDICATE (is_parenthesisL, TKT_LEFT_PARENTHESIS);
TERMINAL_PREDICATE (is_parenthesisR, TKT_RIGHT_PARENTHESIS);


bool
terminal_is_type_qualifier (Token * t)
{
  return terminal_is_const (t) || terminal_is_volatile (t);
}

bool
terminal_is_primary (Token * t)
{
  return terminal_is_identifier (t)
    || terminal_is_int_const (t)
    || terminal_is_float_const (t) || terminal_is_string_const (t);
}

bool
terminal_is_storage_specifier (Token * t)
{
  return terminal_is_auto (t)
    || terminal_is_static (t)
    || terminal_is_typedef (t) || terminal_is_register (t);
}

bool
terminal_is_additive_op(Token * t)
{
  TokenType tt=TOKEN_TYPE(t);
  return  tt == TKT_PLUS || tt == TKT_MINUS;
}

bool
terminal_is_timing_op(Token * t)
{
  TokenType tt=TOKEN_TYPE(t);
  return  tt == TKT_STAR || tt == TKT_BINARY_OP_DIV;
}

