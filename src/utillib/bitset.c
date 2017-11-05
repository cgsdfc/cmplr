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
#include <stdlib.h>

/**
 * \function utillib_bitset_init
 * Initilizes a bitset with enough memory
 * to hold `N' elements.
 */
void utillib_bitset_init(struct utillib_bitset *self, size_t N)
{
  /* Compiler may optimize the division with shift. */
  size_t * bits;
  size_t size=1 + N/sizeof *bits;
  bits=calloc(size, sizeof *bits);
  self->bits=bits;
  self->size=size;

}

/**
 * \function utillib_bitset_destroy
 * Frees the memory used by the bitset.
 */
void utillib_bitset_destroy(struct utillib_bitset*self)
{
  free (self->bits);
}

#define bit_index(pos, self) ((pos) / sizeof *(self)->bits)
#define bit_offset(pos, self) ((pos) % sizeof *(self)->bits)

/**
 * \function utillib_bitset_test
 * Tests membership of element at `pos' against the bitset.
 */
bool utillib_bitset_test(struct utillib_bitset const*self, size_t pos)
{
  return self->bits[bit_index(pos, self)] & (1 << bit_offset(pos, self));
}

/**
 * \function utillib_bitset_set
 * Marks that element at `pos' belongs to the bitset.
 */
void utillib_bitset_set(struct utillib_bitset*self, size_t pos)
{
  self->bits[bit_index(pos, self)] |= (1 << bit_offset(pos, self));
}

/**
 * \function utillib_bitset_reset
 * Erases element from the bitset.
 */
void utillib_bitset_reset(struct utillib_bitset*self, size_t pos)
{
  self->bits[bit_index(pos, self)] &= ~(1 << bit_offset(pos, self));
}

/**
 * \function utillib_bitset_union
 * Puts all the bits of `other' into `self'.
 * \return Whether the content of `self' was different
 * from before the union was done.
 */
bool utillib_bitset_union(struct utillib_bitset *self, struct utillib_bitset const * other)
{
  bool changed=false;
  for (size_t i=0; i< self->size; ++i) {
    size_t prev=self->bits[i];
    self->bits[i] |= other->bits[i];
    if (prev != self->bits[i])
      changed=true;
  }
  return changed;
}

/*
 * Implements JSON format interface.
 */

/**
 * \function utillib_bitset_json_array_create_rawbits
 * Dumps the bitset in rawbits.
 */
utillib_json_value_t * utillib_bitset_json_array_create_rawbits(void *base, size_t offset)
{
  struct utillib_bitset *self=base;
  UTILLIB_JSON_ARRAY_DESC(Bitset_ArrayDesc, sizeof *self->bits, utillib_json_size_t_create);
  return utillib_json_array_pointer_create(self->bits, self->size, &Bitset_ArrayDesc);
}

utillib_json_value_t * utillib_bitset_json_array_create_elements(void *base, size_t offset)
{
  struct utillib_bitset const *self=base;
  utillib_json_value_t * array=utillib_json_array_create_empty();
  for (size_t i=0; i<offset; ++i) {
    if (utillib_bitset_test(self, i)) {
      utillib_json_array_push_back(array, 
          utillib_json_size_t_create(&i, sizeof i));
    }
  }
  return array;
}


struct utillib_bitset * utillib_bitset_create(size_t N)
{
  struct utillib_bitset *self=malloc(sizeof *self);
  utillib_bitset_init(self, N);
  return self;
}

