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
#ifndef UTILLIB_JSON_H
#define UTILLIB_JSON_H
/**
 * \file utiilib/json.h
 * Provides mechanism for mapping between structure in
 * C and JSON format.
 */

#include "enum.h"
#include "typedef.h"
#include "unordered_map.h"
#include "vector.h"
#define UTILLIB_JSON_CREATE_FUNC_DECLARE(NAME)                                 \
  utillib_json_value_t *NAME(void *, size_t);

/**
 * \enum utillib_json_kind_t
 * Type field of `utillib_json_value_t'.
 * Currently supported C <=> JSON types.
 */

UTILLIB_ENUM_BEGIN(utillib_json_kind_t)
UTILLIB_ENUM_ELEM(UT_JSON_REAL)
UTILLIB_ENUM_ELEM(UT_JSON_OBJECT)
UTILLIB_ENUM_ELEM(UT_JSON_ARRAY)
UTILLIB_ENUM_ELEM(UT_JSON_BOOL)
UTILLIB_ENUM_ELEM(UT_JSON_NULL)
UTILLIB_ENUM_ELEM(UT_JSON_LONG)
UTILLIB_ENUM_ELEM(UT_JSON_STRING)
UTILLIB_ENUM_END(utillib_json_kind_t)

/**
 * struct utillib_json_object_field_t
 * Meta information about a C structure.
 */
typedef struct utillib_json_object_field_t {
  utillib_json_value_create_func_t *create_func;
  char const *key;
  size_t offset;
  size_t size;
} utillib_json_object_field_t;

/**
 * struct utillib_json_array_desc_t
 * Meta information about a C array.
 */
typedef struct utillib_json_array_desc_t {
  utillib_json_value_create_func_t *create_func;
  size_t elemsz;
} utillib_json_array_desc_t;

/**
 * \struct utillib_json_object_t
 * Represents a JSON object.
 */

typedef struct utillib_json_object_t {
  utillib_unordered_map members;
} utillib_json_object_t;

/**
 * \struct utillib_json_array_t
 * Represents a JSON array.
 */
typedef struct utillib_json_array_t {
  utillib_vector elements;
} utillib_json_array_t;

/**
 * \struct utillib_json_value_t
 * A type-feild based polynormial representation
 * of JSON's different kinds value.
 */
typedef struct utillib_json_value_t {
  /* Takes value from `utillib_json_kind_t'. */
  int kind;
  union {
    bool as_bool;
    /* Effective when kind == `UT_JSON_REAL' */
    double as_double;
    /* Effective when kind == `UT_JSON_INTEGER' */
    long as_long;
    /**
     * kind:`UT_JSON_ARRAY' && as_ptr:`utillib_json_array_t'.
     * kind:`UT_JSON_OBJECT' && as_ptr:`utillib_json_object_t'.
     * kind:`UT_JSON_STRING' && as_ptr:`char const*'.
     */
    void *as_ptr;
  };
} utillib_json_value_t;

utillib_json_value_t *
utillib_json_object_create(void *, size_t, const utillib_json_object_field_t *);
utillib_json_value_t *
utillib_json_array_create(void *, size_t, const utillib_json_array_desc_t *);
utillib_json_value_t *utillib_json_value_create(int, ...);
utillib_json_value_t *utillib_json_value_createV(int, va_list);
void utillib_json_value_destroy(utillib_json_value_t *);
utillib_json_value_t *utillib_json_null_create(void *, size_t);
utillib_json_value_t *utillib_json_null_array_create(void *, size_t);

UTILLIB_JSON_CREATE_FUNC_DECLARE(utillib_json_real_create)
UTILLIB_JSON_CREATE_FUNC_DECLARE(utillib_json_bool_create)
UTILLIB_JSON_CREATE_FUNC_DECLARE(utillib_json_long_create)
UTILLIB_JSON_CREATE_FUNC_DECLARE(utillib_json_string_create)

UTILLIB_JSON_CREATE_FUNC_DECLARE(utillib_json_real_array_create)
UTILLIB_JSON_CREATE_FUNC_DECLARE(utillib_json_bool_array_create)
UTILLIB_JSON_CREATE_FUNC_DECLARE(utillib_json_long_array_create)
UTILLIB_JSON_CREATE_FUNC_DECLARE(utillib_json_string_array_create)

#endif // UTILLIB_JSON_H
