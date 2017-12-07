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
#ifndef UTILLIB_JSON_IMPL_H
#define UTILLIB_JSON_IMPL_H

#include "vector.h"
#include <assert.h>

/*
 * Since client never see
 * the json object and array
 * struct but always see json
 * value, they can be hidden.
 */
#define _JSON_ARRAY(VALUE)                                                     \
  &(((struct utillib_json_array *)(VALUE)->as_ptr)->elements)
#define _JSON_OBJECT(VALUE)                                                    \
  &(((struct utillib_json_object *)(VALUE)->as_ptr)->members)

/**
 * \struct utillib_json_object
 * Represents a JSON object.
 */

struct utillib_json_object {
  struct utillib_vector members;
};

/**
 * \struct utillib_json_array
 * Represents a JSON array.
 */
struct utillib_json_array {
  struct utillib_vector elements;
};

#define json_value_check_kind(VAL, KIND)                                       \
  do {                                                                         \
    assert((VAL)->kind == (KIND));                                             \
  } while (0)
#define json_check_create_func(FUNC)                                           \
  do {                                                                         \
    assert(FUNC && "create_func should not be NULL");                          \
  } while (0)

#endif /* UTILLIB_JSON_IMPL_H */
