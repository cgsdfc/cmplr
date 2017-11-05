#include "valarray.h"
#include <stdarg.h>
#include <stdlib.h>

static void valarray_initV(struct utillib_valarray *self, size_t nextend,
                           va_list ap) {
  size_t *extends = malloc(sizeof *extends * nextend);
  size_t *nalloc = 1;
  utillib_element_t *array;
  for (size_t i = 0; i < nextend; ++i) {
    extends[i] = va_arg(ap, size_t);
    nalloc *= extends[i];
  }
  array = malloc(sizeof *array * nalloc);
  self->nextend = nextend;
  self->extends = extends;
  self->array = array;
}

/**
 * \function utillib_valarray_init
 * Initializes a valarray with `nextend' number of
 * extends and the size of each extend is given via
 * `...'.
 * Notes that you should use `ul' postfix if passing in
 * integral literals since `utillib_valarray_init' assumes
 * that and takes `...' as a list of `size_t' variables.
 */
void utillib_valarray_init(struct utillib_valarray *self, size_t nextend, ...) {
  va_list ap;
  valarray_initV(self, nextend, ap);
  va_end(ap);
}

static size_t valarray_getpos(struct utillib_valarray *self, va_list ap) {
  size_t pos = 0;
}

utillib_element_t valarray_getV(struct utillib_valarray *self, va_list ap) {}

utillib_element_t valarray_setV(struct utillib_valarray *self,
                                utillib_element_t data...);

utillib_element_t utillib_valarray_get(struct utillib_valarray *self, ...);
utillib_element_t utillib_valarray_set(struct utillib_valarray *self,
                                       utillib_element_t data...);
void utillib_valarray_destroy(struct utillib_valarray *self);
