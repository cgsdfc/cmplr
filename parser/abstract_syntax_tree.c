#include "abstract_syntax_tree.h"
static stack_buffer *node_allocator;


ast_node *alloc_ast_node(void)
{
  ast_node *node;

  if (!node_allocator)
  {
    node_allocator=alloc_stack_buffer(sizeof(ast_node));
    if (!node_allocator)
    {
      return NULL;
    }
  }

  if (stack_buffer_alloc(node_allocator, &node)<0)
    return NULL;
  return node;
}

void dealloc_ast_node(ast_node *node)
{
  stack_buffer_dealloc(node_allocator, node);
}
  
