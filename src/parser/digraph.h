#ifndef DIGRAPH_H
#define DIGRAPH_H 1
#include <stdio.h>
#define DIGRAPH_BUFSIZ 3000
#define DIGRAPH_SET_RELATION(self, X, Y) ((self)->m_relation[(X)][(Y)] = 1)
#define DIGRAPH_SET_DIMENSION(self, D) ((self)->m_dimension = (D))
#define DIGRAPH_SET_DATA(self, data) ((self)->m_data = (data))

#ifdef DEBUG_DIGRAPH
enum { a, b, c, d, e, f, N };

extern const char *name[];
#endif

typedef struct digraph_struct {
  int m_stack_array[DIGRAPH_BUFSIZ];
  int m_stack_top;

  int m_index_map[DIGRAPH_BUFSIZ];

  int m_relation[DIGRAPH_BUFSIZ][DIGRAPH_BUFSIZ];
  int m_dimension;
  int m_edges;

  void *m_data[DIGRAPH_BUFSIZ];
  int m_nonterm_tran2node[DIGRAPH_BUFSIZ][DIGRAPH_BUFSIZ];

} digraph_struct;

void digraph(digraph_struct *);

void init_digraph(digraph_struct *);

int alloc_nonterm_tran_node(digraph_struct *, int, int);

#endif
