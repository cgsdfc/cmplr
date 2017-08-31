
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "digraph.h"
#include "bitset.h"

// algorithm: digraph
// input: a set X, a relation on X, a data per element of X
// output: a properly unioned data per element of X
// end

# ifdef DEBUG_DIGRAPH
const char *name[]={
  "a","b","c","d","e","f"
};

static void
digraph_init_trivial (digraph_struct * self)
{
  DIGRAPH_SET_DIMENSION (self, N);
  DIGRAPH_SET_RELATION (self, a, b);
  DIGRAPH_SET_RELATION (self, b, c);
  DIGRAPH_SET_RELATION (self, c, a);
  DIGRAPH_SET_RELATION (self, c, f);
  DIGRAPH_SET_RELATION (self, c, d);
  DIGRAPH_SET_RELATION (self, e, d);
  DIGRAPH_SET_RELATION (self, f, b);
  DIGRAPH_SET_RELATION (self, d, c);
}
#endif 

static void
init_stack (digraph_struct * self)
{
  self->m_stack_top = 0;
}

static void
init_relation (digraph_struct * self)
{
  self->m_dimension = 0;
  self->m_edges=0;
  memset (self->m_relation, 0, sizeof self->m_relation);
  memset (self->m_index_map, 0, sizeof self->m_index_map);
  memset (self->m_nonterm_tran2node, -1, sizeof self->m_nonterm_tran2node);
}

void
init_digraph (digraph_struct * self)
{
  init_stack (self);
  init_relation (self);
}

static void
push_stack (digraph_struct * self, int x)
{
  self->m_stack_array[self->m_stack_top++] = x;
}

static int
depth_of_stack (digraph_struct * self)
{
  return self->m_stack_top;
}

static void
pop_stack (digraph_struct * self)
{
  if (self->m_stack_top > 0)
    {
      --self->m_stack_top;
    }
}

static int
top_stack (digraph_struct * self)
{
  return self->m_stack_array[self->m_stack_top-1];
}


static void
digraph_data_set (digraph_struct * self, int left_hand, int right_hand)
{
  bitset * lhs = self->m_data[left_hand]; 
  bitset * rhs = self->m_data[right_hand];
  bitset_copy(lhs, rhs);
}

static void
digraph_data_union (digraph_struct * self, int left_hand, int right_hand)
{
  bitset * lhs = self->m_data[left_hand]; 
  bitset * rhs = self->m_data[right_hand];
  union_bitset_inplace(lhs, rhs);
}


static void
traverse (digraph_struct * self, int x)
{
  int depth, top;
  int *xid, *yid;

  push_stack (self, x);
  depth = depth_of_stack (self);
  xid = &self->m_index_map[x];
  *xid = depth;
#ifdef DEBUG_DIGRAPH
  printf ("F(%d) <- F'(%d)\n", x, x);
#endif
  digraph_data_set (self, x, x);

  for (int y = 0; y < self->m_dimension; ++y)
    {
      if (self->m_relation[x][y])
	{
	  if (!self->m_index_map[y])
	    {
	      traverse (self, y);
	      yid = &self->m_index_map[y];
	      *xid = *xid < *yid ? *xid : *yid;
# ifdef DEBUG_DIGRAPH
	      printf ("F(%d) |= F(%d)\n", x, y);
# endif
	      digraph_data_union (self, x, y);
	    }
	}
    }
  if (*xid == depth) // if x is the root of SCC
    {
      // pop the members of SCC
      while (!((top = top_stack (self)) == x))
	{
	  self->m_index_map[top] = INT_MAX;
# ifdef DEBUG_DIGRAPH
	  printf ("F(%d) <- F(%d)\n", top, x);
# endif
	  digraph_data_set (self, top, x);
	  pop_stack (self);
	}
      // mark the root as done
      self->m_index_map[top]=INT_MAX;
      // pop the root
      pop_stack(self);
    }
}


void
digraph (digraph_struct * self)
{
  int m_dimension = self->m_dimension;
  int *m_index_map = self->m_index_map;

  for (int x = 0; x < m_dimension; ++x)
    {
      if (!m_index_map[x])
	{
	  traverse (self, x);
	}
    }
}

int 
alloc_nonterm_tran_node(digraph_struct * self,
    int from,
    int to)
{
  int *node_id=&self->m_nonterm_tran2node[from][to];
  if (*node_id != -1)
    return *node_id;
  return *node_id=self->m_dimension++;
}

