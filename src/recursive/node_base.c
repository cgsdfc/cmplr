#include "recursive/node_base.h"

#define CREATE_NODE(TYPE, TAG)\
struct TYPE * node = calloc(sizeof *node, 1);\
init_node_base(&(node->base), ( TAG ))


void
init_node_base (node_base * base, node_tag tag)
{
  base->tag = tag;
}

node_base *
make_terminal_node (Token * tk)
{
  // for terminal_node,  is for LexerTerminalString;
  CREATE_NODE (terminal_node, NODE_TAG_TERMINAL);
  if (node)
    {
      node->token = tk;
    }
  return TO_NODE_BASE (node);
}

node_base *
make_binary_node (TokenType op)
{
  CREATE_NODE (binary_node, NODE_TAG_BINARY);
  if (node)
    {
      node->op = op;
    }
  return TO_NODE_BASE (node);
}

node_base *
make_unary_node (TokenType op)
{
  CREATE_NODE (unary_node, NODE_TAG_UNARY);
  if (node)
    {
      node->op = op;
    }
  return TO_NODE_BASE (node);
}

node_base *
make_ternary_node (void)
{
  CREATE_NODE (ternary_node, NODE_TAG_TERNARY);
  return TO_NODE_BASE (node);
}

node_base *
make_vector_node ()
{
  CREATE_NODE (vector_node, NODE_TAG_VECTOR);
  utillib_vector_init (&(node->vec));
  return TO_NODE_BASE (node);
}

int
vector_node_push_back (vector_node * node, node_base * x)
{
  return utillib_vector_push_back (&(node->vec), x);
}

node_base *
vector_node_at (struct vector_node * node, size_t pos)
{
  return utillib_vector_at (&(node->vec), pos);
}

size_t
vector_node_size (struct vector_node * node)
{
  return utillib_vector_size (&(node->vec));
}

node_base *
make_nullary_node (void)
{
  static node_base nullary = {.tag = NODE_TAG_NULLARY };
  return &nullary;
}

void
destroy_vector_node (vector_node * node)
{
  utillib_vector_destroy (&(node->vec));
  free (node);
}


void
visit_node (void *data, node_base * node, vfactory * factory)
{
  node_tag tag = NODE_TAG (node);
  vfunction *visit = factory (data, tag);
  return visit (data, node);
}
