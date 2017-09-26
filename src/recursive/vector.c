#include "recursive/vector.h"
#include <stdlib.h>		// calloc free
#include <string.h>		// memset
#include <assert.h>

bool
vector_empty (vector * v)
{
  return vector_size (v) == 0;
}

void
init_vector (vector * v)
{
  memset (v, 0, sizeof *v);
}

size_t
vector_size (vector * v)
{
  return v->end - v->begin;
}

void *
vector_at (vector * v, size_t pos)
{
  assert (pos < vector_size (v));
  return v->begin[pos];
}

static int
push_back_aux (vector * v, void *x)
{
  size_t size = vector_size (v);
  size_t new_size = (size + 1 << 1);
  void **newspace = calloc (sizeof x, new_size);
  if (!newspace)
    {
      return -1;
    }
  memcpy (newspace, v->begin, sizeof x * size);
  free (v->begin);
  v->begin = newspace;
  v->end = newspace + size;
  v->stor_end = newspace + new_size;
  *(v->end)++ = x;
  return 0;
}

int
vector_push_back (vector * v, void *x)
{
  if (v->end == v->stor_end)
    {
      return push_back_aux (v, x);
    }
  *(v->end)++ = x;
  return 0;
}
