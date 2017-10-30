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
#include "json.h"
#include "pair.h"
#include "print.h" // for utillib_static_sprintf
#include "string.h"
#include <stdarg.h> // for va_list
#include <stdlib.h> // for free
#include <string.h>
/**
 * \file utillib/json.c
 * Provides mapping between C structure and JSON
 * data format.
 */

/**
 * \variable static_json_null
 */

static utillib_json_value_t static_json_null = {.kind = UT_JSON_NULL};

UTILLIB_ETAB_BEGIN(utillib_json_kind_t)
UTILLIB_ETAB_ELEM(UT_JSON_REAL)
UTILLIB_ETAB_ELEM(UT_JSON_OBJECT)
UTILLIB_ETAB_ELEM(UT_JSON_ARRAY)
UTILLIB_ETAB_ELEM(UT_JSON_BOOL)
UTILLIB_ETAB_ELEM(UT_JSON_NULL)
UTILLIB_ETAB_ELEM(UT_JSON_LONG)
UTILLIB_ETAB_ELEM(UT_JSON_STRING)
UTILLIB_ETAB_END(utillib_json_kind_t)

/**
 * \function json_object_register
 */
static void json_object_register(utillib_json_object_t *self, char *base,
                                 size_t not_used,
                                 const utillib_json_object_field_t *fields) {
  for (const utillib_json_object_field_t *field = fields;
       field->create_func != NULL; ++field) {
    utillib_json_value_t *value =
        field->create_func(base + field->offset, field->size);
    utillib_pair_t *mem = utillib_make_pair(field->key, value);
    utillib_vector_push_back(&self->members, mem);
  }
}

/**
 * \function json_array_register
 * \param base A pointer to the base address of the array.
 * \param size A scalar such that [base, base+size) is
 * the valid memory range for the array. Notes that this is
 * not the number of elements of the array.
 * \param desc Description of this array type.
 */
static void json_array_register(utillib_json_array_t *self, char *base,
                                size_t size,
                                const utillib_json_array_desc_t *desc) {
  for (size_t offset = 0; offset < size; offset += desc->elemsz) {
    utillib_json_value_t *elem = desc->create_func(base + offset, desc->elemsz);
    utillib_vector_push_back(&self->elements, elem);
  }
}

/**
 * \function utillib_json_object_init
 */

static void json_object_init(utillib_json_object_t *self) {
  utillib_vector_init(&self->members);
}

/**
 * \function utillib_json_array_init
*/
static void json_array_init(utillib_json_array_t *self) {
  utillib_vector_init(&self->elements);
}

/**
 * \function json_object_destroy
 * Destructs a single member of a larger object.
 * Recursively desctructs the value of the member.
 */
static void json_object_member_destroy(utillib_pair_t *self) {
  utillib_json_value_destroy(UTILLIB_PAIR_SECOND(self));
  free(self);
}

/**
 * \function utillib_json_object_destroy
 * Destructs a JSON object.
 */
static void json_object_destroy(utillib_json_object_t *self) {
  utillib_vector_destroy_owning(
      &self->members, (utillib_destroy_func_t *)json_object_member_destroy);
  free(self);
}

/**
 * \function json_array_destroy
 * Destructs a JSON array.
 * 1. Destructs each element of it as a `utillib_json_value_t'.
 * 2. And the vector holding them.
 */
static void json_array_destroy(utillib_json_array_t *self) {
  utillib_vector_destroy_owning(
      &self->elements, (utillib_destroy_func_t *)utillib_json_value_destroy);
  free(self);
}

/**
 * \function utillib_json_value_destroy
 * Destructs a JSON value according to its type.
 * For primary type like `UT_JSON_BOOL' or `UT_JSON_NULL' it
 * just free itself.
 */
void utillib_json_value_destroy(utillib_json_value_t *self) {
  switch (self->kind) {
  case UT_JSON_ARRAY:
    json_array_destroy(self->as_ptr);
    free(self);
    return;
  case UT_JSON_OBJECT:
    json_object_destroy(self->as_ptr);
    free(self);
    return;
  case UT_JSON_NULL:
    /* static */
    return;
  default:
    free(self);
    return;
  }
}

/**
 * \function json_value_create_ptr
 */
static utillib_json_value_t *json_value_create_ptr(int kind, void *data) {
  utillib_json_value_t *self = malloc(sizeof *self);
  self->as_ptr = data;
  self->kind = kind;
  return self;
}

/**
 * \malloc UTILLIB_JSON_PRIMARY_CREATE_FUNC_DEFINE
 * Generates a `utillib_json_<primary>_create' function.
 */
#define UTILLIB_JSON_PRIMARY_CREATE_FUNC_DEFINE(NAME, FIELD, TYPE, KIND)       \
  utillib_json_value_t *NAME(void *base, size_t not_used) {                    \
    utillib_json_value_t *self = malloc(sizeof *self);                         \
    self->FIELD = *(TYPE *)base;                                               \
    self->kind = KIND;                                                         \
    return self;                                                               \
  }

/**
 * \malloc UTILLIB_JSON_PRIMARY_ARRAY_CREATE_FUNC_DEFINE
 * Generates a `utillib_json_<primary>_array_create' function.
 */
#define UTILLIB_JSON_PRIMARY_ARRAY_CREATE_FUNC_DEFINE(ABBR_NAME, ELEM_TYEP)    \
  utillib_json_value_t *ABBR_NAME##_array_create(void *base, size_t size) {    \
    const static utillib_json_array_desc_t static_desc = {                     \
        .elemsz = sizeof(ELEM_TYEP), .create_func = ABBR_NAME##_create};       \
    return utillib_json_array_create(base, size, &static_desc);                \
  }

UTILLIB_JSON_PRIMARY_CREATE_FUNC_DEFINE(utillib_json_real_create, as_double,
                                        double, UT_JSON_REAL)
UTILLIB_JSON_PRIMARY_CREATE_FUNC_DEFINE(utillib_json_bool_create, as_bool, bool,
                                        UT_JSON_BOOL)
UTILLIB_JSON_PRIMARY_CREATE_FUNC_DEFINE(utillib_json_long_create, as_long, long,
                                        UT_JSON_LONG)
UTILLIB_JSON_PRIMARY_CREATE_FUNC_DEFINE(utillib_json_string_create, as_str,
                                        char const *, UT_JSON_STRING)

UTILLIB_JSON_PRIMARY_ARRAY_CREATE_FUNC_DEFINE(utillib_json_real, double)
UTILLIB_JSON_PRIMARY_ARRAY_CREATE_FUNC_DEFINE(utillib_json_bool, bool)
UTILLIB_JSON_PRIMARY_ARRAY_CREATE_FUNC_DEFINE(utillib_json_long, long)
UTILLIB_JSON_PRIMARY_ARRAY_CREATE_FUNC_DEFINE(utillib_json_string, char const *)

/**
 * \function utillib_json_null_array_create
 * Map anything to a JSON array of `null'.
 * \param base Not used. Can even be `NULL'.
 * \param size The number of `null' in the array to create.
 * Notes that the `size' parameter here has different meaning
 * from the more general `utillib_json_array_create'.
 * As if sizeof null == 1.
 */
/* UTILLIB_JSON_PRIMARY_ARRAY_CREATE_FUNC_DEFINE(utillib_json_null, void *) */
utillib_json_value_t *utillib_json_null_array_create(void *not_used,
                                                     size_t size) {
  utillib_json_array_t *self = malloc(sizeof *self);
  utillib_vector_init(&self->elements);
  for (size_t i = 0; i < size; ++i) {
    utillib_vector_push_back(&self->elements, &static_json_null);
  }
  return json_value_create_ptr(UT_JSON_ARRAY, self);
}

/**
 * \function utillib_json_null_create
 */

utillib_json_value_t *utillib_json_null_create(void *not_used_1,
                                               size_t not_used_2) {
  return &static_json_null;
}

/**
 * \function utillib_json_object_create
 */

utillib_json_value_t *
utillib_json_object_create(void *data, size_t size,
                           const utillib_json_object_field_t *fields) {
  utillib_json_object_t *self = malloc(sizeof *self);
  json_object_init(self);
  json_object_register(self, data, size, fields);
  return json_value_create_ptr(UT_JSON_OBJECT, self);
}

/**
 * \function utillib_json_object_pointer_create
 * Replacement of `utillib_json_object_create' when the first argument
 * is not a pointer to a struct but a pointer to pointer to struct.
 * \param pointer Points to another pointer that points to a struct.
 * \param size Not used since the size of a pointer is known.
 * \param fields Forward to `utillib_json_object_create'.
 */
utillib_json_value_t *
utillib_json_object_pointer_create(void *data, size_t size,
                                   const utillib_json_object_field_t *fields) {
  void *pointee = *(void **)data;
  return utillib_json_object_create(pointee, size, fields);
}

/**
 * \function utillib_json_array_pointer_create
 * Replacement of `utillib_json_array_create' when the C array under considered
 * is expressed as a pointer to that array and a `size' variable of the the
 * number
 * of elements of the array.
 * \param pointer The pointer to the C array.
 * \size The number of elements of the C array.
 * \desc The description about the elements.
 */
utillib_json_value_t *
utillib_json_array_pointer_create(void *data, size_t size,
                                  const utillib_json_array_desc_t *desc) {
  return utillib_json_array_create(data, size * desc->elemsz, desc);
}

/**
 * \function utillib_json_array_create
 */

utillib_json_value_t *
utillib_json_array_create(void *data, size_t size,
                          const utillib_json_array_desc_t *desc) {
  utillib_json_array_t *self = malloc(sizeof *self);
  json_array_init(self);
  json_array_register(self, data, size, desc);
  return json_value_create_ptr(UT_JSON_ARRAY, self);
}

/**
 * \function utillib_json_value_create
 * General form to create `utillib_json_value_t'.
 * Wrapper around all those `utillib_json_<type>_create' function.
 *
 * \param kind Take value from `utillib_json_kind_t'.
 *
 * \param ... Determined by `kind'.
 * When `kind' is among those primary types, i.e. real, long, bool, etc,
 * the signature becomes `utillib_json_value_create(kind, type*)'.
 * When `kind' is object or array, the signature becomes
 * `utillib_json_value_create(kind, base, size, desc)'
 * where `kind' is as above, `base' is the base address of your
 * C structure data or array base address, and size is actually the size of it
 * (if it is
 * a struct, use `sizeof *base'). Or if it is an array, `size' is
 *
 * The `desc' is a piece of meta data about `base' and `size' which
 * guides how to interprete the data. In case of JSON object, you need
 * to write a null-terminated array of `utillib_json_object_field_t'
 * to give name, size in struct and function to create the corresponding
 * `utillib_json_value_t' from it.
 * In case of JSON array, since there are already predefined
 * `utillib_json_<primary>_array_create'
 * to help to create array of primary types, you still find it useful to provide
 * a `utillib_json_array_desc_t' for array of your own type.
 *
 * As a general suggestion, use specific create function when you know the
 * underlying
 * C type well. Only use the general form when you want to do some wrapping.
 */
utillib_json_value_t *utillib_json_value_createV(int kind, va_list ap) {
  void *data;
  size_t size;
  void *desc;
  switch (kind) {
  case UT_JSON_NULL:
    return utillib_json_null_create(NULL, 0);
  case UT_JSON_LONG:
    data = va_arg(ap, long *);
    return utillib_json_long_create(data, 0);
  case UT_JSON_BOOL:
    data = va_arg(ap, bool *);
    return utillib_json_bool_create(data, 0);
  case UT_JSON_REAL:
    data = va_arg(ap, double *);
    return utillib_json_real_create(data, 0);
  case UT_JSON_STRING:
    data = va_arg(ap, char const **);
    return utillib_json_string_create(data, 0);
  case UT_JSON_OBJECT:
    data = va_arg(ap, void *);
    size = va_arg(ap, size_t);
    desc = va_arg(ap, void *);
    return utillib_json_object_create(data, size, desc);
  case UT_JSON_ARRAY:
    data = va_arg(ap, void *);
    size = va_arg(ap, size_t);
    desc = va_arg(ap, void *);
    return utillib_json_array_create(data, size, desc);
  }
}

/**
 * \function utillib_json_value_create
 * The same as `utillib_json_value_createV'.
 * Provided for completeness.
 */
utillib_json_value_t *utillib_json_value_create(int kind, ...) {
  va_list ap;
  va_start(ap, kind);
  utillib_json_value_t *self = utillib_json_value_createV(kind, ap);
  va_end(ap);
  return self;
}

/*
 * forward declaration.
 */

static void json_value_tostring(utillib_json_value_t *, utillib_string *);
/**
 * \function json_array_tostring
 */

static void json_array_tostring(utillib_json_array_t *self,
                                utillib_string *string) {
  utillib_string_append(string, "[");
  UTILLIB_VECTOR_FOREACH(utillib_json_value_t *, elem, &self->elements) {
    json_value_tostring(elem, string);
    utillib_string_append(string, ",");
  }
  utillib_string_erase_last(string);
  utillib_string_append(string, "]");
}

/**
 * \function json_object_tostring
 */
static void json_object_tostring(utillib_json_object_t *self,
                                 utillib_string *string) {
  utillib_string_append(string, "{");
  UTILLIB_VECTOR_FOREACH(utillib_pair_t *, mem, &self->members) {
    utillib_string_append(string, "\"");
    utillib_string_append(string, (char const *)UTILLIB_PAIR_FIRST(mem));
    utillib_string_append(string, "\":");
    json_value_tostring(UTILLIB_PAIR_SECOND(mem), string);
    utillib_string_append(string, ",");
  }
  // XXX use `utillib_string_replace_last' ??
  utillib_string_erase_last(string);
  utillib_string_append(string, "}");
}

/**
 * \function json_value_tostring
 */
static void json_value_tostring(utillib_json_value_t *self,
                                utillib_string *string) {
  char const *str;
  switch (self->kind) {
  case UT_JSON_REAL:
    str = utillib_static_sprintf("%lf", self->as_double);
    utillib_string_append(string, str);
    return;
  case UT_JSON_LONG:
    str = utillib_static_sprintf("%ld", self->as_long);
    utillib_string_append(string, str);
    return;
  case UT_JSON_BOOL:
    str = self->as_bool ? "true" : "false";
    utillib_string_append(string, str);
    return;
  case UT_JSON_NULL:
    utillib_string_append(string, "null");
    return;
  case UT_JSON_STRING:
    str = utillib_static_sprintf("\"%s\"", (char const *)self->as_str);
    utillib_string_append(string, str);
    return;
  case UT_JSON_ARRAY:
    json_array_tostring(self->as_ptr, string);
    return;
  case UT_JSON_OBJECT:
    json_object_tostring(self->as_ptr, string);
    return;
  }
}

void utillib_json_tostring(utillib_json_value_t *self, utillib_string *str) {
  json_value_tostring(self, str);
}

/**
 * \function utillib_json_array_create_from_vector
 */

utillib_json_value_t *utillib_json_array_create_from_vector(
    void *vec, size_t not_used, utillib_json_value_create_func_t *create_func) {
  utillib_vector *self = vec;
  utillib_json_array_t *array = malloc(sizeof *array);
  json_array_init(array);
  for (utillib_element_pointer_t p = self->begin; p != self->end; ++p) {
    utillib_json_value_t *val = create_func(*p, sizeof *p);
    utillib_vector_push_back(&array->elements, val);
  }
  return json_value_create_ptr(UT_JSON_ARRAY, array);
}

/**
 * \function utillib_json_pretty_print
 * Convenient function combining `utillib_json_tostring'
 * and `utillib_printer_print_json'.
 */
void utillib_json_pretty_print(utillib_json_value_t *self,
                               utillib_printer_t *printer) {
  utillib_string json;
  utillib_string_init(&json);
  utillib_json_tostring(self, &json);
  utillib_printer_print_json(printer, utillib_string_c_str(&json));
  utillib_string_destroy(&json);
}
