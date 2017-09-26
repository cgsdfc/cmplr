#ifndef RECURSIVE_NODE_BASE_H
#define RECURSIVE_NODE_BASE_H

typedef enum node_tag
{
  NODE_TAG_TERMINAL,
} node_tag;

typedef struct node_base
{
  node_tag tag;
  char *name;
} node_base;

void init_node_base (struct node_base *, node_tag, char *);

#endif // RECURSIVE_NODE_BASE_H
