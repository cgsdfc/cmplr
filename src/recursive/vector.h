#ifndef UTILLIB_VECTOR_H
#define UTILLIB_VECTOR_H
#include <stddef.h>		// size_t
#include <stdbool.h>
typedef struct utillib_vector
{
  void **begin, **end, **stor_end;
} utillib_vector;

bool utillib_vector_empty (utillib_vector *);
void utillib_init_vector (utillib_vector *);
size_t utillib_vector_size (utillib_vector *);
void *utillib_vector_at (utillib_vector *, size_t);
int utillib_vector_push_back (utillib_vector *, void *);

#endif // UTILLIB_VECTOR_H
