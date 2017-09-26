#ifndef RECURSIVE_NODE_BASE_H
#define RECURSIVE_NODE_BASE_H
#include "lexer/lexer.h"
#include "recursive/vector.h"
#define TO_NODE_BASE(NODE) ((node_base*) NODE)
typedef enum node_tag
{
  NODE_TAG_TERMINAL,
  NODE_TAG_UNARY,
  NODE_TAG_BINARY,
  NODE_TAG_TERNARY,
  NODE_TAG_VECTOR,
} node_tag;

typedef struct node_base
{
  node_tag tag;
  const char *name;
} node_base;
typedef struct terminal_node
{
  node_base base;
  Token *token;
} terminal_node;

typedef struct binary_node
{
  node_base base;
  TokenType op;
  node_base *lhs, *rhs;
} binary_node;

typedef struct unary_node
{
  node_base base;
  TokenType op;
  node_base *operand;
} unary_node;

typedef struct vector_node
{
  node_base base;
  vector vec;
} vector_node;

typedef struct ternary_node
{
  node_base base;
  node_base *cond, *first, *second;
} ternary_node;

void init_node_base (struct node_base *, node_tag, const char *);

node_base *make_ternary_node (void);
node_base *make_unary_node (TokenType, const char *);
node_base *make_binary_node (TokenType, const char *);
node_base *make_terminal_node (Token *);
node_base *make_vector_node (const char *);
int vector_node_push_back (vector_node *, node_base *);
#endif // RECURSIVE_NODE_BASE_H
