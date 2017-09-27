#ifndef RECURSIVE_NODE_BASE_H
#define RECURSIVE_NODE_BASE_H
#include "lexer/lexer.h"
#include "recursive/vector.h"
#define TO_NODE_BASE(NODE) ((node_base*) NODE)
#define NODE_TAG(NODE) ((NODE)->tag)
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

void init_node_base (struct node_base *, node_tag);

node_base *make_ternary_node (void);
node_base *make_unary_node (TokenType);
node_base *make_binary_node (TokenType);
node_base *make_terminal_node (Token *);
node_base *make_vector_node (void);
int vector_node_push_back (struct vector_node *, struct node_base *);
node_base *vector_node_at (struct vector_node *, size_t);
size_t vector_node_size (struct vector_node *);
#endif // RECURSIVE_NODE_BASE_H
