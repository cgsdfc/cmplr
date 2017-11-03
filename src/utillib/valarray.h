#ifndef UTILLIB_VALARRAY_H
#define UTILLIB_VALARRAY_H
#include "types.h"

/**
 * \struct utillib_valarray
 * A multi-dimensional array of fixed 
 * extend of each dimensions.
 */
struct utillib_valarray {
  size_t nextend;
  size_t * extends;
  utillib_element_t * array;
};

void utillib_valarray_init(struct utillib_valarray *self, 
    size_t nextend, ...);
utillib_element_t utillib_valarray_get(struct utillib_valarray *self, ...);
utillib_element_t utillib_valarray_set(struct utillib_valarray *self, ...);
void utillib_valarray_destroy(struct utillib_valarray *self);

#endif // UTILLIB_VALARRAY_H
