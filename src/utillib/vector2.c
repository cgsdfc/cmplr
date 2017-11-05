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
#include <stdlib.h>

void utillib_vector2_init(struct utillib_vector2 *self, size_t nrow,
                          size_t ncol) {
  utillib_element_t *array = calloc(nrow * ncol, sizeof *array);
  self->nrow = nrow;
  self->ncol = ncol;
  self->array = array;
}

#define vector2_offset(self, row, col) ((self)->ncol * row + col)

utillib_element_t utillib_vector2_at(struct utillib_vector2 const *self, size_t row,
                                     size_t col) {
  return self->array[vector2_offset(self, row, col)];
}

utillib_element_t utillib_vector2_set(struct utillib_vector2 *self, size_t row,
                                      size_t col, utillib_element_t data) {
  size_t offset = vector2_offset(self, row, col);
  utillib_element_t old_data = self->array[offset];
  self->array[offset] = data;
  return old_data;
}

void utillib_vector2_destroy(struct utillib_vector2 *self) {
  free(self->array);
}
