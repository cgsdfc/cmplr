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
#ifndef UTILLIB_BITSET_H
#define UTILLIB_BITSET_H

#include "json.h"
#include <stdbool.h> /* bool */
#include <stddef.h>  /* size_t */
#define utillib_bitset_N(self) ((self)->N)

/**
 * \struct utillib_bitset
 * A bitset implementation using dynamic memory.
 * Bitsets provides a way to "test-and-set" a range
 * of elements which can be represented by integers.
 * In terms of memory usage, bitset is absolutely more
 * conpact than any hash set implementation but please
 * notice that the modulo and division it takes to locate
 * a single bit may make the operations slower.
 * Also notice that it can only representes non-negative
 * integers.
 */
struct utillib_bitset {
  size_t *bits;
  size_t size;
  size_t N;
};

struct utillib_bitset *utillib_bitset_create(size_t N);
void utillib_bitset_init(struct utillib_bitset *self, size_t N);
void utillib_bitset_destroy(struct utillib_bitset *self);
bool utillib_bitset_test(struct utillib_bitset const *self, size_t pos);
void utillib_bitset_set(struct utillib_bitset *self, size_t pos);
void utillib_bitset_reset(struct utillib_bitset *self, size_t pos);
bool utillib_bitset_union(struct utillib_bitset *self,
                          struct utillib_bitset const *other);
bool utillib_bitset_is_intersect(struct utillib_bitset const *self,
                                 struct utillib_bitset const *other);
bool utillib_bitset_equal(struct utillib_bitset const *self,
                          struct utillib_bitset const *other);
utillib_json_value_t *utillib_bitset_json_array_create(void const*base,
                                                       size_t offset);
#endif /* UTILLIB_BITSET_H */
