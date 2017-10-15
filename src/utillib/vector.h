#ifndef UTILLIB_VECTOR_H
#define UTILLIB_VECTOR_H
#include <stdbool.h>
#include <stddef.h> // size_t
#define UTILLIB_VECTOR_FOREACH(T, X, V)                                        \
  T X;                                                                         \
  for (void **_begin = (V)->begin, **_end = (V)->end;                          \
       _begin != _end && (X = *_begin); ++_begin)
typedef void **utillib_vector_impl_t;
typedef struct utillib_vector {
  utillib_vector_impl_t begin, end, stor_end;
} utillib_vector;

bool utillib_vector_empty(utillib_vector *);
void utillib_vector_init(utillib_vector *);
size_t utillib_vector_size(utillib_vector *);
void *utillib_vector_at(utillib_vector *, size_t);
void utillib_vector_push_back(utillib_vector *, void *);
void utillib_vector_destroy(utillib_vector *);
void utillib_vector_pop_back(utillib_vector *);
void *utillib_vector_back(utillib_vector *);
void *utillib_vector_front(utillib_vector *);
void utillib_vector_reserve(utillib_vector *, size_t);
void utillib_vector_set(utillib_vector *, size_t, void *);
#endif // UTILLIB_VECTOR_H
