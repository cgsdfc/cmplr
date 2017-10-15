#include "vector.h"
#include "error.h"
#include <assert.h>
#include <stdlib.h> // calloc free
#include <string.h> // memset

bool utillib_vector_empty(utillib_vector *self) {
  return utillib_vector_size(self) == 0;
}

void utillib_vector_init(utillib_vector *self) {
  memset(self, 0, sizeof *self);
}

size_t utillib_vector_size(utillib_vector *self) {
  return self->end - self->begin;
}

void *utillib_vector_at(utillib_vector *self, size_t pos) {
  assert(pos < utillib_vector_size(self));
  return self->begin[pos];
}

static void do_realloc(utillib_vector *self, size_t new_cap) {
  size_t size = utillib_vector_size(self);
  utillib_vector_impl_t newspace =
      realloc(self->begin, sizeof *newspace * new_cap);
  if (newspace) {
    self->begin = newspace;
    self->end = size + self->begin;
    self->stor_end = self->begin + new_cap;
    return;
  }
  utillib_die("utillib_vector: realloc failed");
}

static void push_back_aux(utillib_vector *self, void *x) { *(self->end)++ = x; }

void utillib_vector_push_back(utillib_vector *self, void *x) {
  if (self->end == self->stor_end) {
    do_realloc(self, (1 + utillib_vector_size(self)) << 1);
  }
  push_back_aux(self, x);
}

void utillib_vector_destroy(utillib_vector *self) { free(self->begin); }

void *utillib_vector_front(utillib_vector *self) { return self->begin[0]; }

void utillib_vector_pop_back(utillib_vector *self) {
  assert(utillib_vector_size(self) > 0);
  --(self->end);
}

void *utillib_vector_back(utillib_vector *self) {
  assert(utillib_vector_size(self) > 0);
  return *(self->end - 1);
}
void utillib_vector_set(utillib_vector *self, size_t pos, void *data) {
  assert(pos < utillib_vector_size(self));
  self->begin[pos] = data;
}
size_t utillib_vector_capacity(utillib_vector *self) {
  return self->stor_end - self->begin;
}

void utillib_vector_reserve(utillib_vector *self, size_t new_cap) {
  if (utillib_vector_capacity(self) < new_cap) {
    do_realloc(self, new_cap);
  }
}
