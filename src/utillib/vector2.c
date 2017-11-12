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
#include "vector2.h"
#include "json.h"
#include <assert.h>
#include <stdlib.h>

/**
 * \function utillib_vector2_init
 * Initilizes a 2-dimension table with all slots
 * filled `NULL'.
 */
void utillib_vector2_init(struct utillib_vector2 *self, size_t nrow,
                          size_t ncol) {
  void const **array = calloc(nrow * ncol, sizeof *array);
  self->nrow = nrow;
  self->ncol = ncol;
  self->array = array;
}

/**
 * \macro
 * vector2_check
 * Checks the sanity of the ROW index and COLUMN index.
 */
#define vector2_check(self, row, col)                                          \
  do {                                                                         \
    assert(row < self->nrow && "Row index out of range");                      \
    assert(col < self->ncol && "Column index out of range");                   \
  } while (0)

/**
 * \macro
 * vector2_offset
 * Computes the offset in `self->array'
 * given row index and column index.
 */
#define vector2_offset(self, row, col) ((self)->ncol * row + col)

/**
 * \function utillib_vector2_at
 * Accesses table element.
 * Checks index with `assert'.
 */
void *utillib_vector2_at(struct utillib_vector2 const *self, size_t row,
                         size_t col) {
  vector2_check(self, row, col);
  return (void *)self->array[vector2_offset(self, row, col)];
}

/**
 * \function utillib_vector2_set
 * Alters table element and returns the old one.
 * Checks index with `assert'.
 */
void *utillib_vector2_set(struct utillib_vector2 *self, size_t row, size_t col,
                          void const *data) {
  vector2_check(self, row, col);
  size_t offset = vector2_offset(self, row, col);
  void const *old_data = self->array[offset];
  self->array[offset] = data;
  return (void *)old_data;
}

/**
 * \function utillib_vector2_destroy
 * Free up memory.
 */
void utillib_vector2_destroy(struct utillib_vector2 *self) {
  free(self->array);
  self->array = NULL;
}

/*
 * JSON interfaces
 */

struct utillib_json_value_t *utillib_vector2_json_array_create(
    struct utillib_vector2 const *self,
    utillib_json_value_create_func_t create_func) {
  struct utillib_json_value_t *array = utillib_json_array_create_empty();
  for (int i = 0; i < self->nrow; ++i) {
    struct utillib_json_value_t *row_array = utillib_json_array_create_empty();
    for (int j = 0; j < self->ncol; ++j) {
      void const *data = self->array[vector2_offset(self, i, j)];
      struct utillib_json_value_t *json_data = (data && create_func)
                                            ? create_func(data, sizeof data)
                                            : utillib_json_null_create();
      utillib_json_array_push_back(row_array, json_data);
    }
    utillib_json_array_push_back(array, row_array);
  }
  return array;
}
