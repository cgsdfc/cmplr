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
 * C and JSON format. Since this is almost about dealing
 * with safely converting C types to JSON types, the design
 * was written out here.
 * 
 * 1. Take care of yourselves because this design uses `void const*'
 * so it is essentially unsafe. However, it provides enum value that
 * has one-to-one mapping from C type to JSON type so once you pass
 * parameter of the correct type to those create_funcs, your data
 * will be well typed and safe.
 * 
 * 2. For POD types, use the directly creating way. For POD I means
 * Plain Old Data including primary type like bool, int, size_t, double, float
 * and char const* and built-in array that contains POD and struct that
 * contains POD as members. By using directly creating way, you either call
 * those XXX_create or make a `utillib_json_object_field_t' to descript your
 * POD struct or make a `utillib_json_array_desc_t' to descript your POD array
 * and then call `utillib_json_object_create' or `utillib_json_array_create'.
 * Thus, `struct utillib_vector' is not POD and so on.
 * 
 * 3. For non POD types, uses the create_empty and push_back way. In this way,
 * you write your only create function that is unnecessarily restricted by 
 * a certain signature and build up the JSON value using those `push_back' API.
 * You gain maximum flexibility in this way.
 * 
 */

#include "enum.h"
#include "string.h"
#include "vector.h"
#include <stdarg.h> /* for va_list */
#include <stdio.h>

typedef struct utillib_json_value_t * (*utillib_json_value_create_func_t)(void const *);
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
  },

/**
 * \enum utillib_json_kind
 * Type field of `utillib_json_value_t'.
 * Currently supported C <=> JSON types.
 * \value UT_JSON_REAL double
 * \value UT_JSON_OBJECT struct
 * \value UT_JSON_ARRAY struct or POD array
 * \value UT_JSON_BOOL bool 
 * \value UT_JSON_NULL can be mapped to any thing
 * \value UT_JSON_LONG long
 * \value UT_JSON_SIZE_T size_t
 * \value UT_JSON_INT int
 * \value UT_JSON_STRING char const *
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
 * \field create_func Function to create
 * json value from each field of this struct.
 * \field key The name of this field. Serves
 * as the key in the json object.
 * \field offset offset of this field in
 * the struct.
 */
struct utillib_json_object_field_t {
  utillib_json_value_create_func_t create_func;
  char const *key;
  size_t offset;
};

/**
 * \struct utillib_json_array_desc_t
 * Meta information about a C array.
 * \field create_func Function to create json value from
 * each element.
 * \field elemsz The size of each element.
 * \field size The number of elements.
 */
struct utillib_json_array_desc_t {
  utillib_json_value_create_func_t create_func;
  size_t elemsz;
  size_t size;
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
    double as_double;
    long as_long;
    char const *as_str;
    int as_int;
    size_t as_size_t;
    void const *as_ptr;
  };
};

extern const struct utillib_json_value_t utillib_json_true;
extern const struct utillib_json_value_t utillib_json_false;
extern const struct utillib_json_value_t utillib_json_null;


/**
 * \function utillib_json_object_create
 * Creates a JOSN object from POD struct
 * \param base Base array of this struct.
 * \param fields Pointer to field descripting array.
 * \return a json value.
 */
struct utillib_json_value_t*
utillib_json_object_create(void const *base, struct utillib_json_object_field_t const *fields);
/**
 * \function utillib_json_array_create
 * Creates a JOSN array from a POD array.
 * \param base Base address of the array.
 * \param Sees above.
 * \return a json value.
 */
struct utillib_json_value_t*
utillib_json_array_create(void const *base, struct utillib_json_array_desc_t const *desc);
void utillib_json_array_desc_init(struct utillib_json_array_desc_t *self, size_t elemsz,
    size_t size, utillib_json_value_create_func_t create_func);

void utillib_json_value_destroy(void *_self);


struct utillib_json_value_t *utillib_json_object_create_empty(void);
struct utillib_json_value_t *utillib_json_array_create_empty(void);
void utillib_json_array_push_back(struct utillib_json_value_t *self,
                                  struct utillib_json_value_t const *value);

void utillib_json_object_push_back(struct utillib_json_value_t *self,
                                   char const *key,
                                   struct utillib_json_value_t const *value);
struct utillib_pair *
utillib_json_object_back(struct utillib_json_value_t *self);

struct utillib_json_value_t *
utillib_json_array_back(struct utillib_json_value_t *self);


void utillib_json_tostring(struct utillib_json_value_t const * self, struct utillib_string *str);

void utillib_json_pretty_print(struct utillib_json_value_t const *self, FILE *file);

struct utillib_json_value_t * utillib_json_string_create(void const *base);
struct utillib_json_value_t *utillib_json_real_create(void const *base);
struct utillib_json_value_t *utillib_json_size_t_create(void const *base);
struct utillib_json_value_t* utillib_json_int_create(void const *base);
struct utillib_json_value_t* utillib_json_bool_create(void const *base);
struct utillib_json_value_t*utillib_json_long_create(void const *base);
struct utillib_json_value_t* utillib_json_string_create(void const *base);
struct utillib_json_value_t*utillib_json_null_create(void);


#endif /* UTILLIB_JSON_H */
