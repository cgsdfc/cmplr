#include "utillib/vector.h"
#include <stdlib.h>		// calloc free
#include <string.h>		// memset
#include <assert.h>

bool
utillib_vector_empty (utillib_vector * v)
{
  return utillib_vector_size (v) == 0;
}

void
init_vector (utillib_vector * v)
{
  memset (v, 0, sizeof *v);
}

size_t
utillib_vector_size (utillib_vector * v)
{
  return v->end - v->begin;
}

void *
utillib_vector_at (utillib_vector * v, size_t pos)
{
  assert (pos < utillib_vector_size (v));
  return v->begin[pos];
}

static int
push_back_aux (utillib_vector * v, void *x)
{
  size_t size = utillib_vector_size (v);
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
utillib_vector_push_back (utillib_vector * v, void *x)
{
  if (v->end == v->stor_end)
    {
      return push_back_aux (v, x);
    }
  *(v->end)++ = x;
  return 0;
}
