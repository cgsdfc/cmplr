#include "vector.h"
#include <assert.h>
#include <stdlib.h> // calloc free
#include <string.h> // memset

bool utillib_vector_empty(utillib_vector *self) {
  return utillib_vector_size(self) == 0;
}

void utillib_vector_init(utillib_vector *self) { memset(self, 0, sizeof *self); }

size_t utillib_vector_size(utillib_vector *self) { return self->end - self->begin; }

void *utillib_vector_at(utillib_vector *self, size_t pos) {
  assert(pos < utillib_vector_size(self));
  return self->begin[pos];
}

static int push_back_aux(utillib_vector *self, void *x) {
  size_t size = utillib_vector_size(self);
  size_t new_size = (size + 1 << 1);
  void **newspace = calloc(sizeof x, new_size);
  if (!newspace) {
    return -1;
  }
  memcpy(newspace, self->begin, sizeof x * size);
  free(self->begin);
  self->begin = newspace;
  self->end = newspace + size;
  self->stor_end = newspace + new_size;
  *(self->end)++ = x;
  return 0;
}

int utillib_vector_push_back(utillib_vector *self, void *x) {
  if (self->end == self->stor_end) {
    return push_back_aux(self, x);
  }
  *(self->end)++ = x;
  return 0;
}

void utillib_vector_destroy(utillib_vector *self) { free(self->begin); }

void *utillib_vector_pop_back(utillib_vector *self) {
  assert(utillib_vector_size(self) > 0);
  return *--(self->end);
}

void *utillib_vector_back(utillib_vector *self) {
  assert(utillib_vector_size(self) > 0);
  return *(self->end - 1);
}
void utillib_vector_set(utillib_vector *self, size_t pos, void *data) {
  assert(pos < utillib_vector_size(self));
  self->begin[pos]=data;
}

int utillib_vector_reserve(utillib_vector *self, size_t size) {}
