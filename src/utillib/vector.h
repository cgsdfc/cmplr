#ifndef UTILLIB_VECTOR_H
#define UTILLIB_VECTOR_H

/** \file utillib/vector
 * A automatically resizable array.
 * Currently implementation uses three
 * generic pointers to keep track of
 * the head of the array for random access,
 * the tail of the array for back insertion
 * and storage tail for resizing.
 */

#include "typedef.h" // for utillib_element_pointer_t, utillib_element_t
#include <stdbool.h> // for bool
#include <stddef.h>  // size_t

#define UTILLIB_VECTOR_HAS_NEXT(B, E) ((B) != (E))
#define UTILLIB_VECTOR_BEGIN(V) ((V)->begin)
#define UTILLIB_VECTOR_END(V) ((V)->end)
#define UTILLIB_VECTOR_FOREACH(T, X, V)                                        \
  T X;                                                                         \
  for (utillib_element_pointer_t _begin = (V)->begin, _end = (V)->end;         \
       _begin != _end && ((X = *_begin) || 1); ++_begin)

/** \brief the element of utillib_vector is generic pointer */
typedef struct utillib_vector {
  utillib_element_pointer_t begin, end, stor_end;
} utillib_vector;

typedef struct utillib_vector_iterator {
  utillib_element_pointer_t iter_begin, iter_end;
} utillib_vector_iterator;

/* \brief utillib_vector_iterator */
void utillib_vector_iterator_init(utillib_vector_iterator *, utillib_vector *);
bool utillib_vector_iterator_has_next(utillib_vector_iterator *);
void utillib_vector_iterator_next(utillib_vector_iterator *);
utillib_element_t utillib_vector_iterator_get(utillib_vector_iterator *);

/** \brief constructor destructor */
void utillib_vector_init(utillib_vector *);
void utillib_vector_destroy(utillib_vector *);
void utillib_vector_destroy_owning(utillib_vector *, utillib_destroy_func_t *);

/** \brief observer */
size_t utillib_vector_size(utillib_vector *);
bool utillib_vector_empty(utillib_vector *);
utillib_element_t utillib_vector_at(utillib_vector *, size_t);
utillib_element_t utillib_vector_back(utillib_vector *);
utillib_element_t utillib_vector_front(utillib_vector *);

/** \brief modifier */
void utillib_vector_push_back(utillib_vector *, utillib_element_t);
void utillib_vector_pop_back(utillib_vector *);
void utillib_vector_reserve(utillib_vector *, size_t);
void utillib_vector_set(utillib_vector *, size_t, utillib_element_t);
void utillib_vector_clear(utillib_vector *);
#endif // UTILLIB_VECTOR_H
