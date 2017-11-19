/*
   Cmplr Library
   Copyright (C) 2017-2018 Cong Feng <cgsdfc@126.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301 USA

*/
#include "vector.h"
#include "json.h"
#include <assert.h>
#include <stdlib.h> // calloc free
#include <string.h> // memset

/**
 * \function push_back_aux
 * Does an insertion at the end of self regardless of
 * its capacity.
 * \param self.
 * \param x the element to push back.
 * \return void.
 */

static void push_back_aux(struct utillib_vector *self, void const *x) {
  *(self->end)++ = x;
}

/**
 * \function do_realloc
 * Does a reallocation to ensure that self has capacity `new_cap'
 * and its elements are unchanged.
 * \param new_cap the new capacity to satisfy.
 * \return void.
 */

static void do_realloc(struct utillib_vector *self, size_t new_cap) {
  size_t size = utillib_vector_size(self);
  void const **newspace = realloc(self->begin, new_cap * sizeof *newspace);
  self->begin = newspace;
  self->end = size + self->begin;
  self->stor_end = self->begin + new_cap;
}

/**
 * \function clear_self
 * Clear the memory of self to zero.
 * \param self.
 * \return void.
 */

static void clear_self(struct utillib_vector *self) {
  memset(self, 0, sizeof *self);
}

void utillib_vector_init_fill(struct utillib_vector *self, size_t N,
                              void const *data) {
  void const **begin = malloc(N * sizeof *begin);
  self->begin = begin;
  self->end = begin + N;
  for (self->stor_end = begin; self->stor_end != self->end; ++self->stor_end) {
    *self->stor_end = data;
  }
}

/**
 * \function utillib_vector_empty
 * Query emptiness of utillib_vector
 * \param self.
 * \return true if self has no element else false.
 */

bool utillib_vector_empty(struct utillib_vector const *self) {
  return utillib_vector_size(self) == 0;
}

/**
 * \function utillib_vector_init
 * Construct a utillib_vector
 * \param self.
 * \return void
 */

void utillib_vector_init(struct utillib_vector *self) { clear_self(self); }

/**
 * \function utillib_vector_size
 * Returns the number of elements held by self.
 * \param self.
 * \return size of self.
 */

size_t utillib_vector_size(struct utillib_vector const *self) {
  return self->end - self->begin;
}

/**
 * \function utillib_vector_at
 * Return the element at a position of self.
 * \param self.
 * \param pos the position to query, it should be in range 0,
 * utillib_vector_size(self).
 * \return the element at `pos'.
 */

void *utillib_vector_at(struct utillib_vector const *self, size_t pos) {
  assert(pos < utillib_vector_size(self));
  return (void *)self->begin[pos];
}

/**
 * \function utillib_vector_push_back
 * Adds an element to the end of self. Does a reallocation
 * if necessary.
 * \param self.
 * \param x the element to push back.
 * \return void.
 */

void utillib_vector_push_back(struct utillib_vector *self, void const *x) {
  if (self->end == self->stor_end) {
    do_realloc(self, (1 + utillib_vector_size(self)) << 1);
  }
  push_back_aux(self, x);
}

/**
 * \function utillib_vector_destroy
 * Deallocates the memory held by self->begin without destructing
 * its elements. Assumes they are not owned by self.
 * \param self.
 * \return void.
 */

void utillib_vector_destroy(struct utillib_vector *self) {
  free(self->begin);
  self->begin = NULL;
}

/**
 * \function utillib_vector_destroy_owning
 * Does as utillib_vector_destroy but also applies `destroy' to
 * every element.
 * \param self.
 * \param destroy a function pointer doing destruction.
 * \return void.
 */

void utillib_vector_destroy_owning(struct utillib_vector *self,
                                   void (*destroy)(void *)) {
  UTILLIB_VECTOR_FOREACH(void const *, elem, self) { destroy((void *)elem); }
  free(self->begin);
}

/**
 * \function utillib_vector_front
 * Returns the first element of self assuming self has at least
 * one element.
 * \param self.
 * \return the element at the front.
 */

void *utillib_vector_front(struct utillib_vector const *self) {
  return (void *)self->begin[0];
}

/**
 * \function utillib_vector_pop_back
 * Removes the last element of self assuming self has at least
 * one element.
 * \param self.
 * \return void.
 */

void utillib_vector_pop_back(struct utillib_vector *self) {
  assert(utillib_vector_size(self) > 0);
  --(self->end);
}

/**
 * \function utillib_vector_back
 * Returns the last element of self assuming self has at least
 * one element.
 * \param self.
 * \return the element at the back.
 */

void *utillib_vector_back(struct utillib_vector const *self) {
  assert(utillib_vector_size(self) > 0);
  return (void *)*(self->end - 1);
}

/**
 * \function utillib_vector_set
 * Set the element at `pos' to `data'.
 * \param self.
 * \param pos the position at which the element is set.
 * \param data the data to set at `pos'.
 * \return void.
 */

void utillib_vector_set(struct utillib_vector *self, size_t pos,
                        void const *data) {
  assert(pos < utillib_vector_size(self));
  self->begin[pos] = data;
}

/**
 * \function utillib_vector_capacity
 * Returns the capacity of self. That is the number of actually
 * allocated elements.
 * \param self.
 * \return capacity of self.
 */

size_t utillib_vector_capacity(struct utillib_vector const *self) {
  return self->stor_end - self->begin;
}

/**
 * \function utillib_vector_clear
 * Removes all the elements in one operation without freeing
 * any memory.
 * \param self.
 */

void utillib_vector_clear(struct utillib_vector *self) {
  self->end = self->begin;
}

/**
 * \function utillib_vector_reserve
 * Ensures the capacity of self is at least `new_cap'.
 * \param self.
 * \param new_cap.
 * \return void.
 */

void utillib_vector_reserve(struct utillib_vector *self, size_t new_cap) {
  if (utillib_vector_capacity(self) < new_cap) {
    do_realloc(self, new_cap);
  }
}

/**
 * \function utillib_vector_iterator_init
 * Initializes self with the utillib_vector to traversal.
 * \param self.
 * \param cont the container. if it is empty, self does not
 * point to any data.
 * \return void.
 */

void utillib_vector_iterator_init(struct utillib_vector_iterator *self,
                                  struct utillib_vector *cont) {
  self->iter_begin = cont->begin;
  self->iter_end = cont->end;
}

/**
 * \function utillib_vector_iterator_has_next
 * Tells whether self points to a valid element
 * and has a valid successor. Note that the pass-the-end
 * iterator is a valid successor of any other valid iterator
 * that points to real data, but itself not points to valid
 * data and has no valid successor.
 * \para self.
 * \return whether self has a valid successor.
 */

bool utillib_vector_iterator_has_next(struct utillib_vector_iterator *self) {
  return self->iter_begin != self->iter_end;
}

/**
 * \function utillib_vector_iterator_get
 * Get the data pointed to by self.
 * \param self.
 * \return if self is not a valid iterator, it yeilds
 * undefined behaviour.
 */

void *utillib_vector_iterator_get(struct utillib_vector_iterator *self) {
  return (void *)*self->iter_begin;
}

/**
 * \function utillib_vector_iterator_next
 * Advances self to its successor. If self has
 * no valid successor, it yeilds undefined behaviour.
 * \param self.
 * \return void.
 */

void utillib_vector_iterator_next(struct utillib_vector_iterator *self) {
  ++self->iter_begin;
}

/**
 * JSON interfaces
 */
/**
 * \function utillib_json_array_create_from_vector
 */
struct utillib_json_value *
utillib_vector_json_array_create(struct utillib_vector const *self,
    utillib_json_value_create_func_t create_func)
{
  struct utillib_json_value *array = utillib_json_array_create_empty();
  for (void const **pelem = self->begin; pelem != self->end; ++pelem) {
    struct utillib_json_value const *val = (*pelem && create_func)
                                           ? create_func(*pelem)
                                           : &utillib_json_null;
    utillib_json_array_push_back(array, val);
  }
  return array;
}
