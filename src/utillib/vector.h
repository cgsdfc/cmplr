#ifndef UTILLIB_VECTOR_H
#define UTILLIB_VECTOR_H
#include <stddef.h>		// size_t
#include <stdbool.h>
typedef struct vector
{
  void **begin, **end, **stor_end;
} vector;

bool vector_empty (vector *);
void init_vector (vector *);
size_t vector_size (vector *);
void *vector_at (vector *, size_t);
int vector_push_back (vector *, void *);

#endif // UTILLIB_VECTOR_H
