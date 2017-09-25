#ifndef RECURSIVE_TERMINAL_H
#define RECURSIVE_TERMINAL_H

#include "recursive/node_base.h"
#include "lexer/lexer.h"

typedef struct terminal_node
{
  node_base base;
  Token *token;
} terminal_node;

bool terminal_is_type_qualifier (Token *);
bool terminal_is_primary (Token *);
bool terminal_is_storage_specifier (Token *);
bool terminal_is_parenthesisL (Token *);
bool terminal_is_parenthesisR (Token *);
bool terminal_is_additive_op (Token *);


#endif // RECURSIVE_TERMINAL_H
