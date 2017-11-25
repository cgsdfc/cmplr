#ifndef UTILLIB_GRAPH_H
#define UTILLIB_GRAPH_H
#include "vector.h"
#include "enum.h"
#include "json.h"

struct utillib_edge {
  size_t srcid;
  size_t dstid;
  void const * value;
};

/**
 * \struct utillib_edge_list
 * A list of edges which merely
 * holds the all the relationships between
 * nodes and provides traversal over all
 * the nodes and all the edges. 
 * This is useful when a graph is the 
 * output of some algorithms ready to
 * be processed by another one. It therefore
 * assumes nothing about how clients will use
 * its data.
 * To that minimum functionality, it does assign
 * each node an integer to identify it. 
 * It also allows you to walk through all the edges
 * visiting value on edges and getting value on nodes
 * by their IDs.
 */

struct utillib_edge_list {
  struct utillib_vector nodes;
  struct utillib_vector edges;
};

/**
 * \function utillib_edge_list_init
 * Initializes an empty edge list with no nodes or edges.
 */
void utillib_edge_list_init(struct utillib_edge_list *self);

/**
 * \function utillib_edge_list_destroy
 * Destroys the edge list but does not destroy 
 * the value on nodes and edges.
 */
void utillib_edge_list_destroy(struct utillib_edge_list *self);

/**
 * \function utillib_edge_list_destroy_owning
 * Destroys the edge list using the callbacks from client.
 * Either of `node_destroy' or `edge_destroy' can be `NULL',
 * which will not be called.
 * This is useful if you store primary types into the pointers.
 * If both is NULL, use `utillib_edge_list_destroy' instead.
 */
void utillib_edge_list_destroy_owning(struct utillib_edge_list *self,
    void (*node_destroy) (void*), 
    void (*edge_destroy) (void*));

/**
 * \function utillib_edge_list_add_edge
 * Adds an edge to edge list. An edge is considered as a triple
 * of ID(src) and ID(dst) and possible value on this edge.
 * It returns the index at which the newly added edge is stored
 * so this edge can be retrieved by this ID later.
 * Notice that there is no checking for whether one of the ends of edge
 * came from `utillib_edge_list_add_node' so if you can add edges directly
 * without adding any nodes.
 */
size_t  utillib_edge_list_add_edge(struct utillib_edge_list *self,
    size_t srcid, size_t dstid, void const * value);

/**
 * \function utillib_edge_list_add_node
 * Adds an node to the edge list. A node is merely some value
 * at certain place that can be retrieved by their IDs. In this
 * way you can traversal all the nodes at once or visit them when
 * visiting edges.
 * It returns the ID of the newly added node.
 */
size_t utillib_edge_list_add_node(struct utillib_edge_list *self, void const * value);

/*
 * JSON interface
 */
struct utillib_json_value * 
utillib_edge_list_json_object_create(struct utillib_edge_list const *self,
    struct utillib_json_value * (*node_create) (void const *node),
    struct utillib_json_value * (*edge_create) (void const *edge));

#endif /* UTILLIB_GRAPH_H */
