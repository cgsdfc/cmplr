#include "graph.h"
#include <stdlib.h>

void utillib_edge_list_init(struct utillib_edge_list *self)
{
  utillib_vector_init(&self->nodes);
  utillib_vector_init(&self->edges);
}

void utillib_edge_list_destroy(struct utillib_edge_list *self)
{
  utillib_vector_destroy(&self->nodes);
  utillib_vector_destroy_owning(&self->edges, free);
}

void utillib_edge_list_destroy_owning(struct utillib_edge_list *self,
    void (*node_destroy) (void*),
    void (*edge_destroy) (void*) )
{
  if (node_destroy) {
    UTILLIB_VECTOR_FOREACH(void *, node, &self->nodes) {
      node_destroy(node);
    }
  }
  if (edge_destroy) {
    UTILLIB_VECTOR_FOREACH(struct utillib_edge *, edge, &self->edges) {
      edge_destroy(edge->value);
    }
  }
  utillib_edge_list_destroy(self);
}

size_t 
utillib_edge_list_add_edge(struct utillib_edge_list *self,
    size_t srcid, size_t dstid, void const * value)
{
  struct utillib_vector * edges=&self->edges;
  struct utillib_edge * new_edge=malloc(sizeof *new_edge);
  new_edge->srcid=srcid;
  new_edge->dstid=dstid;
  new_edge->value=value;
  size_t edge_id=utillib_vector_size(edges);
  utillib_vector_push_back(edges, new_edge);
  return edge_id;
}

size_t 
utillib_edge_list_add_node(struct utillib_edge_list *self, void const * value)
{
  struct utillib_vector * nodes=&self->nodes;
  size_t node_id=utillib_vector_size(nodes);
  utillib_vector_push_back(nodes, value);
  return node_id;
}

struct utillib_json_value * 
utillib_edge_list_json_object_create(struct utillib_edge_list const *self,
    struct utillib_json_value * (*node_create) (void const *node),
    struct utillib_json_value * (*edge_create) (void const *edge))
{
  struct utillib_json_value *object=utillib_json_object_create_empty();
  if (node_create) {
    utillib_json_object_push_back(object, "nodes",
        utillib_vector_json_array_create(&self->nodes, node_create));
  }
  if (edge_create) {
    struct utillib_json_value * edge_array=utillib_json_array_create_empty();
    UTILLIB_VECTOR_FOREACH(struct utillib_edge const *, edge, &self->edges) {
      struct utillib_json_value * edge_object=utillib_json_object_create_empty();
      utillib_json_object_push_back(edge_object, "srcid",
          utillib_json_size_t_create(&edge->srcid));
      utillib_json_object_push_back(edge_object, "dstid",
          utillib_json_size_t_create(&edge->dstid));
      utillib_json_object_push_back(edge_object, "value",
          edge_create(edge->value));
      utillib_json_array_push_back(edge_array, edge_object);
    }
    utillib_json_object_push_back(object, "edges", edge_array);
  }
  return object;
}
