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
#include "string.h"
#include "vector.h"
#include <stdarg.h> /* for va_list */
#include <stdio.h>

typedef struct utillib_json_value_t *(utillib_json_value_create_func_t)(void const *,
                                                                 size_t);
/**
 * \macro UTILLIB_JSON_OBJECT_FIELD_BEGIN
 * Begins to define an array of `struct utillib_json_object_field_t'.
 */
#define UTILLIB_JSON_OBJECT_FIELD_BEGIN(NAME)                                  \
  static const struct utillib_json_object_field_t NAME[] = {
#define UTILLIB_JSON_OBJECT_FIELD_END(NAME)                                    \
  { 0 }                                                                        \
  }                                                                            \
  ;

/**
 * \macro UTILLIB_JSON_OBJECT_FIELD_ELEM
 * Initializer for `struct utillib_json_object_field_t'.
 * \param STRUCT The identifier as the name of the struct.
 * \param KEY A string literal to name this field. It is not
 * necessarily the name of the field.
 * \param FIELD The identifier as the name of the field.
 * \param FUNC A function pointer that should create a `struct
 * utillib_json_value_t'
 * when passed in the base address and offset of this field.
 */
#define UTILLIB_JSON_OBJECT_FIELD_ELEM(STRUCT, KEY, FIELD, FUNC)               \
  {.create_func = (FUNC),                                                      \
   .key = (KEY),                                                               \
   .offset = offsetof(STRUCT, FIELD),                                          \
   .size = sizeof((STRUCT *)NULL)->FIELD},

#define UTILLIB_JSON_ARRAY_DESC(NAME, ELEMSZ, FUNC)                            \
  static const struct utillib_json_array_desc_t NAME = {.create_func = (FUNC), \
                                                        .elemsz = (ELEMSZ)};
/**
 * \macro UTILLIB_JSON_CREATE_FUNC_DECLARE
 * Shortcut for declaring creating function.
 */
#define UTILLIB_JSON_CREATE_FUNC_DECLARE(NAME)                                 \
  struct utillib_json_value_t *NAME(void const *, size_t);

/**
 * \enum utillib_json_kind
 * Type field of `utillib_json_value_t'.
 * Currently supported C <=> JSON types.
 */

UTILLIB_ENUM_BEGIN(utillib_json_kind)
UTILLIB_ENUM_ELEM(UT_JSON_REAL)
UTILLIB_ENUM_ELEM(UT_JSON_OBJECT)
UTILLIB_ENUM_ELEM(UT_JSON_ARRAY)
UTILLIB_ENUM_ELEM(UT_JSON_BOOL)
UTILLIB_ENUM_ELEM(UT_JSON_NULL)
UTILLIB_ENUM_ELEM(UT_JSON_LONG)
UTILLIB_ENUM_ELEM(UT_JSON_SIZE_T)
UTILLIB_ENUM_ELEM(UT_JSON_INT)
UTILLIB_ENUM_ELEM(UT_JSON_STRING)
UTILLIB_ENUM_END(utillib_json_kind);

/**
 * \struct utillib_json_object_field_t
 * Meta information about a C structure.
 */
struct utillib_json_object_field_t {
  utillib_json_value_create_func_t *create_func;
  char const *key;
  size_t offset;
  size_t size;
};

/**
 * struct utillib_json_array_desc_t
 * Meta information about a C array.
 */
struct utillib_json_array_desc_t {
  utillib_json_value_create_func_t *create_func;
  size_t elemsz;
};

/**
 * \struct utillib_json_object_t
 * Represents a JSON object.
 */

struct utillib_json_object_t {
  struct utillib_vector members;
};

/**
 * \struct utillib_json_array_t
 * Represents a JSON array.
 */
struct utillib_json_array_t {
  struct utillib_vector elements;
};

/**
 * \struct utillib_json_value_t
 * A type-feild based polynormial representation
 * of JSON's different kinds value.
 */
struct utillib_json_value_t {
  /* Takes value from `utillib_json_kind'. */
  int kind;
  union {
    bool as_bool;
    double as_double;
    long as_long;
    char const *as_str;
    int as_int;
    size_t as_size_t;
    /**
     * kind:`UT_JSON_ARRAY' && as_ptr:`struct utillib_json_array_t'.
     * kind:`UT_JSON_OBJECT' && as_ptr:`struct utillib_json_object_t'.
     */
    void *as_ptr;
  };
};

struct utillib_json_value_t *
utillib_json_object_create(void const *, size_t,
                           struct utillib_json_object_field_t const *);

struct utillib_json_value_t *
utillib_json_object_pointer_create(void const *, size_t,
                                   struct utillib_json_object_field_t const *);
struct utillib_json_value_t *
utillib_json_array_create(void const *, size_t,
                          struct utillib_json_array_desc_t const *);

struct utillib_json_value_t *utillib_json_value_create(int, ...);

struct utillib_json_value_t *utillib_json_value_createV(int, va_list);

void utillib_json_value_destroy(struct utillib_json_value_t *);

struct utillib_json_value_t *utillib_json_null_create(void);

struct utillib_json_value_t *utillib_json_null_array_create(size_t);

struct utillib_json_value_t *
utillib_json_array_pointer_create(void const *, size_t,
                                  struct utillib_json_array_desc_t const *);

void utillib_json_tostring(struct utillib_json_value_t const *,
                           struct utillib_string *);

void utillib_json_pretty_print(struct utillib_json_value_t const *, FILE *);

void utillib_json_array_push_back(struct utillib_json_value_t *self,
                                  struct utillib_json_value_t const *value);

void utillib_json_object_push_back(struct utillib_json_value_t *self,
                                   char const *key,
                                   struct utillib_json_value_t const *value);

struct utillib_json_value_t *utillib_json_object_create_empty(void);

struct utillib_json_value_t *utillib_json_array_create_empty(void);

UTILLIB_JSON_CREATE_FUNC_DECLARE(utillib_json_real_create)
UTILLIB_JSON_CREATE_FUNC_DECLARE(utillib_json_bool_create)
UTILLIB_JSON_CREATE_FUNC_DECLARE(utillib_json_long_create)
UTILLIB_JSON_CREATE_FUNC_DECLARE(utillib_json_string_create)
UTILLIB_JSON_CREATE_FUNC_DECLARE(utillib_json_size_t_create)
UTILLIB_JSON_CREATE_FUNC_DECLARE(utillib_json_int_create)

UTILLIB_JSON_CREATE_FUNC_DECLARE(utillib_json_real_array_create)
UTILLIB_JSON_CREATE_FUNC_DECLARE(utillib_json_bool_array_create)
UTILLIB_JSON_CREATE_FUNC_DECLARE(utillib_json_long_array_create)
UTILLIB_JSON_CREATE_FUNC_DECLARE(utillib_json_string_array_create)
UTILLIB_JSON_CREATE_FUNC_DECLARE(utillib_json_int_array_create)
UTILLIB_JSON_CREATE_FUNC_DECLARE(utillib_json_size_t_array_create)
#endif /* UTILLIB_JSON_H */
