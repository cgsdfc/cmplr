#ifndef ABSTRACT_SYNTAX_TREE
#define ABSTRACT_SYNTAX_TREE 1
#include "stack_buffer.h"

typedef stack_buffer_entry ast_node;

typedef struct ast_node_value {
  int type;
  struct ast_node **children;
  int nchild;
  int limit;
} ast_node_value;

#endif
