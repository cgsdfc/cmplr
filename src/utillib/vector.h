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
#include "config.h" /* for bool */
#include <stddef.h> /* size_t */

/**
 * \macro UTILLIB_VECTOR_FOREACH
 * Inline triversal code for vector.
 */

#define UTILLIB_VECTOR_FOREACH(X, V)                                           \
  for (void **_begin = (V)->begin, **_end = (V)->end;                    \
       _begin != _end && ((X = *_begin) || 1); ++_begin)

/** \brief the element of utillib_vector is generic pointer */
struct utillib_vector {
  void **begin;
  void **end;
  void **stor_end;
};

/** \brief constructor destructor */
void utillib_vector_init(struct utillib_vector *self);
void utillib_vector_init_fill(struct utillib_vector *self, size_t,
                              void const *);
void utillib_vector_destroy(struct utillib_vector *self);
void utillib_vector_destroy_owning(struct utillib_vector *self,
                                   void (*destroy)(void *));

/** \brief observer */
size_t utillib_vector_size(struct utillib_vector const *self);
size_t utillib_vector_capacity(struct utillib_vector const *self);
bool utillib_vector_empty(struct utillib_vector const *self);
void *utillib_vector_at(struct utillib_vector const *self, size_t);
void *utillib_vector_back(struct utillib_vector const *self);
void *utillib_vector_front(struct utillib_vector const *self);

/** \brief modifier */
void utillib_vector_push_back(struct utillib_vector *self, void const *);
void utillib_vector_pop_back(struct utillib_vector *self);
void utillib_vector_reserve(struct utillib_vector *self, size_t);
void utillib_vector_set(struct utillib_vector *self, size_t, void const *);
void utillib_vector_clear(struct utillib_vector *self);
void utillib_vector_fill(struct utillib_vector *self, void const *data);
void utillib_vector_append(struct utillib_vector *self, 
    struct utillib_vector const* other);

struct utillib_json_value *utillib_vector_json_array_create(
    struct utillib_vector const *self,
    struct utillib_json_value *(*create_func)(void const *));

#endif /* UTILLIB_VECTOR_H */
