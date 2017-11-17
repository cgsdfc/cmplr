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
#include "bitset.h"
#include "json.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h> // memcmp

/**
 * \function utillib_bitset_init
 * Initilizes a bitset with enough memory
 * to hold `N' elements.
 */
void utillib_bitset_init(struct utillib_bitset *self, size_t N) {
  /* Compiler may optimize the division with shift. */
  size_t *bits;
  size_t size = 1 + N / sizeof *bits;
  bits = calloc(size, sizeof *bits);
  self->bits = bits;
  self->size = size;
  self->N = N;
}

/**
 * \function utillib_bitset_destroy
 * Frees the memory used by the bitset.
 */
void utillib_bitset_destroy(struct utillib_bitset *self) {
  free(self->bits);
  self->bits = NULL;
}

#define bit_index(pos, self) ((pos) / sizeof(self)->bits[0])
#define bit_offset(pos, self) ((pos) % sizeof(self)->bits[0])
#define bitset_index_check(pos, self)                                          \
  assert(pos < self->N && "Index out of range")

/**
 * \function utillib_bitset_contains
 * Tests membership of element at `pos' against the bitset.
 */
bool utillib_bitset_contains(struct utillib_bitset const *self, size_t pos) {
  bitset_index_check(pos, self);
  return self->bits[bit_index(pos, self)] & (1 << bit_offset(pos, self));
}

/**
 * \function utillib_bitset_set
 * Marks that element at `pos' belongs to the bitset.
 */
void utillib_bitset_insert(struct utillib_bitset *self, size_t pos) {
  bitset_index_check(pos, self);
  self->bits[bit_index(pos, self)] |= (1 << bit_offset(pos, self));
}

/**
 * \function utillib_bitset_reset
 * Erases element from the bitset.
 */
void utillib_bitset_remove(struct utillib_bitset *self, size_t pos) {
  bitset_index_check(pos, self);
  self->bits[bit_index(pos, self)] &= ~(1 << bit_offset(pos, self));
}

/*
 * \function utillib_bitset_union
 * Unions `other' into `self.
 */
void utillib_bitset_union(struct utillib_bitset *self,
                          struct utillib_bitset const *other) {
  for (int i = 0; i < self->size; ++i) {
    self->bits[i] |= other->bits[i];
  }
}

/*
 * The following interfaces with `_updated' as postfix
 * do the same thing as their counterparts except that
 * they returns boolean to indicate whether `self' was
 * updated by these operations.
 * They are useful for fixed-pointed computation, but will
 * be slower than their counterparts.
 */

bool utillib_bitset_insert_updated(struct utillib_bitset *self, size_t value) {
  if (utillib_bitset_contains(self, value))
    return false;
  utillib_bitset_insert(self, value);
  return true;
}

bool utillib_bitset_remove_updated(struct utillib_bitset *self, size_t value) {
  if (!utillib_bitset_contains(self, value))
    return false;
  utillib_bitset_remove(self, value);
  return true;
}

/**
 * \function utillib_bitset_union_updated
 * Unions `other' into `self' and returns
 * true if `self' is updated by this operation.
 * \return True if `other' updates `self'.
 */
bool utillib_bitset_union_updated(struct utillib_bitset *self,
                                  struct utillib_bitset const *other) {
  bool changed = false;
  for (size_t i = 0; i < self->size; ++i) {
    size_t prev = self->bits[i];
    self->bits[i] |= other->bits[i];
    if (prev != self->bits[i])
      changed = true;
  }
  return changed;
}

bool utillib_bitset_equal(struct utillib_bitset const *self,
                          struct utillib_bitset const *other) {
  return 0 ==
         memcmp(self->bits, other->bits, sizeof self->bits[0] * self->size);
}

bool utillib_bitset_is_intersect(struct utillib_bitset const *self,
                                 struct utillib_bitset const *other) {
  for (size_t i = 0; i < self->size; ++i)
    if (self->bits[i] & other->bits[i])
      return true;
  return false;
}

/*
 * Implements JSON format interface.
 */

/**
 * \function utillib_bitset_json_array_create
 * Dumps the elements that is in the bitset.
 */
struct utillib_json_value_t *utillib_bitset_json_array_create(void const *base,
                                                              size_t offset) {
  struct utillib_bitset const *self = base;
  struct utillib_json_value_t *array = utillib_json_array_create_empty();
  for (size_t i = 0; i < self->N; ++i) {
    if (utillib_bitset_contains(self, i)) {
      utillib_json_array_push_back(array,
                                   utillib_json_size_t_create(&i, sizeof i));
    }
  }
  return array;
}
