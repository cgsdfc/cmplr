#ifndef UTILLIB_VECTOR_H
#define UTILLIB_VECTOR_H
#include <stdbool.h>
#include <stddef.h> // size_t
typedef struct utillib_vector {
  void **begin, **end, **stor_end;
} utillib_vector;

bool utillib_vector_empty(utillib_vector *);
void utillib_vector_init(utillib_vector *);
size_t utillib_vector_size(utillib_vector *);
void *utillib_vector_at(utillib_vector *, size_t);
int utillib_vector_push_back(utillib_vector *, void *);
void utillib_vector_destroy(utillib_vector *);
void *utillib_vector_pop_back(utillib_vector *);
void *utillib_vector_back(utillib_vector *);
#endif // UTILLIB_VECTOR_H
