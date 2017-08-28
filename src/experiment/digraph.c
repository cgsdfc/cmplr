#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// algorithm: digraph
// input: a set X, a relation on X, a data per element of X
// output: a properly unioned data per element of X
// end
#define DIGRAPH_BUFSIZ 50
#define DIGRAPH_SET_RELATION(self, X, Y) ((self)->m_relation[(X)][(Y)]=1)
#define DIGRAPH_SET_DIMENSION(self, D) ((self)->m_dimension=(D))
#define DIGRAPH_SET_DATA(self, data) ((self)->m_data=(data))

enum
{
  a, b, c, d, e, f, N
};

const char *name[]={
  "a","b","c","d","e","f"
};

struct digraph_struct;
typedef void (*digraph_union_func) (struct digraph_struct *, int, int);
typedef void (*digraph_nullary_init_func) (struct digraph_struct *);
typedef digraph_nullary_init_func digraph_relation_init_func;
void digraph_init_trivial (struct digraph_struct *);

typedef struct digraph_struct
{
  int m_stack_array[DIGRAPH_BUFSIZ];
  int m_stack_top;

  int m_index_map[DIGRAPH_BUFSIZ];

  int m_relation[DIGRAPH_BUFSIZ][DIGRAPH_BUFSIZ];
  int m_dimension;

  int m_data[DIGRAPH_BUFSIZ];
  int m_unioned_data[DIGRAPH_BUFSIZ];

} digraph_struct;


void
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


void
init_stack (digraph_struct * self)
{
  self->m_stack_top = 0;
}

void
init_relation (digraph_struct * self)
{
  self->m_dimension = 0;
  memset (self->m_relation, 0, sizeof self->m_relation);
  memset (self->m_index_map, 0, sizeof self->m_index_map);
  digraph_init_trivial (self);
  memcpy (self->m_unioned_data, self->m_data, sizeof self->m_unioned_data);
}

void
init_digraph (digraph_struct * self)
{
  init_stack (self);
  init_relation (self);
}

void
push_stack (digraph_struct * self, int x)
{
  self->m_stack_array[self->m_stack_top++] = x;
}

int
depth_of_stack (digraph_struct * self)
{
  return self->m_stack_top;
}

void
pop_stack (digraph_struct * self)
{
  if (self->m_stack_top > 0)
    {
      --self->m_stack_top;
    }
}

int
top_stack (digraph_struct * self)
{
  return self->m_stack_array[self->m_stack_top-1];
}


void
digraph_data_set (digraph_struct * self, int left_hand, int right_hand)
{
  self->m_unioned_data[left_hand] = self->m_unioned_data[right_hand];
}

void
digraph_data_union (digraph_struct * self, int left_hand, int right_hand)
{
  self->m_unioned_data[left_hand] |= self->m_unioned_data[right_hand];
}


void
traverse (digraph_struct * self, int x)
{
  int depth, top;
  int *xid, *yid;

  push_stack (self, x);
  depth = depth_of_stack (self);
  xid = &self->m_index_map[x];
  *xid = depth;
  printf ("F(%s) <- F'(%s)\n", name[x], name[ x ]);
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
	      printf ("F(%s) |= F(%s)\n", name[x], name[y]);
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
	  printf ("F(%s) <- F(%s)\n", name[top], name[x]);
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
main (int argc, char **argv)
{
  digraph_struct d;
  puts ("digraph demo");
  init_digraph (&d);
  digraph (&d);
  return 0;
}
