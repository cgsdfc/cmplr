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
#ifndef UTILLIB_VECTOR_H
#define UTILLIB_VECTOR_H

/** \file utillib/vector.h
 * An automatically resizable array.
 * Currently implementation uses three
 * generic pointers to keep track of
 * the head of the array for random access,
 * the tail of the array for back insertion
 * and storage tail for resizing.
 */

#include "types.h"		// for utillib_element_pointer_t, utillib_element_t
#include <stdbool.h>		// for bool
#include <stddef.h>		// size_t

#define UTILLIB_VECTOR_HAS_NEXT(B, E) ((B) != (E))
#define UTILLIB_VECTOR_BEGIN(V) ((V)->begin)
#define UTILLIB_VECTOR_END(V) ((V)->end)

/**
 * \macro UTILLIB_VECTOR_FOREACH
 * Inline triversal code for vector.
 * Notes that it is not suitable for
 * nested invoke.
 */

#define UTILLIB_VECTOR_FOREACH(T, X, V)                                        \
  T X;                                                                         \
  for (utillib_element_pointer_t _begin = (V)->begin, _end = (V)->end;         \
       _begin != _end && ((X = (T)*_begin) || 1); ++_begin)

/** \brief the element of utillib_vector is generic pointer */
struct utillib_vector {
	utillib_element_pointer_t begin, end, stor_end;
} ;

/**
 * \struct utillib_vector_iterator
 * Keeps `begin' and `end' pointers of
 * the vector to allow forward triversal.
 */
struct utillib_vector_iterator {
	utillib_element_pointer_t iter_begin, iter_end;
};

/** \brief utillib_vector_iterator */
void utillib_vector_iterator_init(struct utillib_vector_iterator *,
				  struct utillib_vector *);
bool utillib_vector_iterator_has_next(struct utillib_vector_iterator *);
void utillib_vector_iterator_next(struct utillib_vector_iterator *);
utillib_element_t utillib_vector_iterator_get(struct utillib_vector_iterator *);

/** \brief constructor destructor */
void utillib_vector_init(struct utillib_vector *);
void utillib_vector_init_fill(struct utillib_vector *, size_t, utillib_element_t);
void utillib_vector_destroy(struct utillib_vector *);
void utillib_vector_destroy_owning(struct utillib_vector *,
				   utillib_destroy_func_t *);

/** \brief observer */
size_t utillib_vector_size(struct utillib_vector const*);
size_t utillib_vector_capacity(struct utillib_vector *);
bool utillib_vector_empty(struct utillib_vector *);
utillib_element_t utillib_vector_at(struct utillib_vector const*, size_t);
utillib_element_t utillib_vector_back(struct utillib_vector const*);
utillib_element_t utillib_vector_front(struct utillib_vector const*);
bool utillib_vector_find(struct utillib_vector *self, utillib_element_t data, utillib_equal_func_t *eq);

/** \brief modifier */
void utillib_vector_push_back(struct utillib_vector *, utillib_element_t);
void utillib_vector_pop_back(struct utillib_vector *);
void utillib_vector_reserve(struct utillib_vector *, size_t);
void utillib_vector_set(struct utillib_vector *, size_t, utillib_element_t);
void utillib_vector_clear(struct utillib_vector *);
void utillib_vector_back_insert(struct utillib_vector *self, struct utillib_vector *other);
void utillib_vector_fill(struct utillib_vector *self, utillib_element_t data);

#endif				// UTILLIB_VECTOR_H
