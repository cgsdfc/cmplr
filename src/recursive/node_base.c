#include "recursive/node_base.h"

#define CREATE_NODE(TYPE, TAG, NAME)\
struct TYPE * node = calloc(sizeof *node, 1);\
init_node_base(&(node->base), ( TAG ), (NAME)?(# NAME):(NAME) )


void
init_node_base (node_base * base, node_tag tag, const char *name)
{
  base->tag = tag;
  base->name = name;
}

node_base *
make_terminal_node (Token * tk)
{
  // for terminal_node, name is for LexerTerminalString;
  CREATE_NODE (terminal_node, NODE_TAG_TERMINAL, LexerTerminalString (tk));
  if (node)
    {
      node->token = tk;
    }
  return TO_NODE_BASE (node);
}

node_base *
make_binary_node (TokenType op, const char *name)
{
  CREATE_NODE (binary_node, NODE_TAG_BINARY, name);
  if (node)
    {
      node->op = op;
    }
  return TO_NODE_BASE (node);
}

node_base *
make_unary_node (TokenType op, const char *name)
{
  CREATE_NODE (unary_node, NODE_TAG_UNARY, name);
  if (node)
    {
      node->op = op;
    }
  return TO_NODE_BASE (node);
}

node_base *
make_ternary_node (void)
{
  CREATE_NODE (ternary_node, NODE_TAG_TERNARY, "operator?");
  return TO_NODE_BASE (node);
}

node_base *
make_vector_node (const char *name)
{
  CREATE_NODE (vector_node, NODE_TAG_VECTOR, name);
  return TO_NODE_BASE (node);
}

int
vector_node_push_back (vector_node * node, node_base * x)
{
  return vector_push_back (&(node->vec), x);
}
