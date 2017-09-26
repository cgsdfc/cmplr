#include "recursive/node_base.h"

void
init_node_base (node_base * base, node_tag tag, char *name)
{
  base->tag = tag;
  base->name = name;
}
