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
#include <stdlib.h>
#include <assert.h>

void utillib_vector2_init(struct utillib_vector2 *self, size_t nrow,
                          size_t ncol) {
  utillib_element_t *array = calloc(nrow * ncol, sizeof *array);
  self->nrow = nrow;
  self->ncol = ncol;
  self->array = array;
}

#define vector2_check(self, row, col) do {\
  assert (row < self->nrow && "Row index out of range");\
  assert (col < self->ncol && "Column index out of range");\
}while (0)

#define vector2_offset(self, row, col) ((self)->ncol * row + col)

utillib_element_t utillib_vector2_at(struct utillib_vector2 const *self, size_t row,
                                     size_t col) {
  vector2_check(self, row, col);
  return self->array[vector2_offset(self, row, col)];
}

utillib_element_t utillib_vector2_set(struct utillib_vector2 *self, size_t row,
                                      size_t col, utillib_element_t data) {
  vector2_check(self, row, col);
  size_t offset = vector2_offset(self, row, col);
  utillib_element_t old_data = self->array[offset];
  self->array[offset] = data;
  return old_data;
}

void utillib_vector2_destroy(struct utillib_vector2 *self) {
  free(self->array);
  self->array=NULL;
}

/*
 * JSON interfaces
 */

utillib_json_value_t * utillib_vector2_json_array_create(
  struct utillib_vector2 const * self,
  utillib_json_value_create_func_t * create_func)
{
  struct utillib_json_value_t * array = utillib_json_array_create_empty();
  for (int i=0 ; i<self->nrow; ++i) {
    struct utillib_json_value_t * row_array=utillib_json_array_create_empty();
    for (int j=0; j<self->ncol; ++j) {
      utillib_element_t data=self->array[vector2_offset(self, i, j)];
      utillib_json_value_t * json_data= (data && create_func) ?
      create_func(data, sizeof data)
      :utillib_json_null_create(NULL, 0);
      utillib_json_array_push_back(row_array, json_data);
    }
    utillib_json_array_push_back(array, row_array);
  }
  return array;
}

