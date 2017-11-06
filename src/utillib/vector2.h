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
#ifndef UTILLIB_VECTOR2_H
#define UTILLIB_VECTOR2_H
#include "types.h"
#include "json.h"
#include <stddef.h>

struct utillib_vector2 {
  utillib_element_t *array;
  size_t nrow;
  size_t ncol;
};

void utillib_vector2_init(struct utillib_vector2 *, size_t, size_t);
utillib_element_t utillib_vector2_at(struct utillib_vector2  const*, size_t, size_t);
utillib_element_t utillib_vector2_set(struct utillib_vector2 *, size_t, size_t,
                                      utillib_element_t);
void utillib_vector2_destroy(struct utillib_vector2 *);
utillib_json_value_t * utillib_vector2_json_array_create(
  struct utillib_vector2 const * self,
  utillib_json_value_create_func_t * create_func);

#endif // UTILLIB_VECTOR2_H
