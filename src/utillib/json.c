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
#define _BSD_SOURCE
#include "json.h"
#include <stdarg.h>
#include <string.h>
/**
 * \file utillib/json.c
 * Provides mapping between C structure and JSON
 * data format.
 */

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
                                 const utillib_json_object_field_t *fields) {
  for (const utillib_json_object_field_t *field = fields;
       field->create_func != NULL; ++field) {
    base += field->offset;
    utillib_value_t value = field->create_func(base, field->size);
    utillib_unordered_map_emplace(&self->members, field->key, value);
  }
}

/**
 * \function json_array_register
 */
static void json_array_register(utillib_json_array_t *self, char *base,
                                size_t size,
                                const utillib_json_array_desc_t *desc) {
  size_t offset = 0;
  for (; offset < size; offset += desc->elemsz) {
    utillib_json_value_t *elem = desc->create_func(base + offset, desc->elemsz);
    utillib_vector_push_back(&self->elements, elem);
  }
}

/**
 * \function utillib_json_object_init
 */

static void json_object_init(utillib_json_object_t *self, void *base,
                             size_t offset,
                             const utillib_json_object_field_t *fields) {
  utillib_unordered_map_init(&self->members,
                             utillib_unordered_map_const_charp_ft());
  json_object_register(self, base, fields);
}

/**
 * \function utillib_json_array_init
*/
static void json_array_init(utillib_json_array_t *self, void *base, size_t size,
                            const utillib_json_array_desc_t *desc) {
  utillib_vector_init(&self->elements);
  json_array_register(self, base, size, desc);
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
  utillib_unordered_map_destroy_owning(&self->members,
 (utillib_destroy_func_t*)json_object_member_destroy);
  free(self);
}

/**
 * \function json_array_destroy
 * Destructs a JSON array.
 * 1. Destructs each element of it as a `utillib_json_value_t'.
 * 2. And the vector holding them.
 */
static void json_array_destroy(utillib_json_array_t *self) {
  utillib_vector_destroy_owning(&self->elements, (utillib_destroy_func_t*)utillib_json_value_destroy);
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
    return;
  case UT_JSON_OBJECT:
    json_object_destroy(self->as_ptr);
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
  utillib_json_value_t *ABBR_NAME##_array_create(void *base, size_t offset) {  \
    const static utillib_json_array_desc_t static_desc = {                     \
        .elemsz = sizeof(ELEM_TYEP), .create_func = ABBR_NAME##_create};       \
    return utillib_json_array_create(base, offset, &static_desc);              \
  }

UTILLIB_JSON_PRIMARY_CREATE_FUNC_DEFINE(utillib_json_real_create, as_double,
                                        double, UT_JSON_REAL)
UTILLIB_JSON_PRIMARY_CREATE_FUNC_DEFINE(utillib_json_bool_create, as_bool, bool,
                                        UT_JSON_BOOL)
UTILLIB_JSON_PRIMARY_CREATE_FUNC_DEFINE(utillib_json_long_create, as_long, long,
                                        UT_JSON_LONG)

/**
 * \function utillib_json_string_create
 * A C string is in fact a pointer to some data in .text section
 * and the current implementation does not `strdup' it.
 */
utillib_json_value_t * utillib_json_string_create(void *base, size_t not_used) {
    return json_value_create_ptr(UT_JSON_STRING, base);
}

UTILLIB_JSON_PRIMARY_ARRAY_CREATE_FUNC_DEFINE(utillib_json_real, double)
UTILLIB_JSON_PRIMARY_ARRAY_CREATE_FUNC_DEFINE(utillib_json_bool, bool)
UTILLIB_JSON_PRIMARY_ARRAY_CREATE_FUNC_DEFINE(utillib_json_long, long)



/**
 * \function utillib_json_null_array_create
 * Maps a C array of pointers to an array of JSON null.
 */
UTILLIB_JSON_PRIMARY_ARRAY_CREATE_FUNC_DEFINE(utillib_json_null, void *)

/**
 * \variable static_json_null
 */

static utillib_json_value_t static_json_null;
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
utillib_json_object_create(void *data, size_t offset,
                           const utillib_json_object_field_t *fields) {
  utillib_json_object_t *self = malloc(sizeof *self);
  json_object_init(self, data, offset, fields);
  return json_value_create_ptr(UT_JSON_OBJECT, self);
}

/**
 * \function utillib_json_array_create
 */

utillib_json_value_t *
utillib_json_array_create(void *data, size_t offset,
                          const utillib_json_array_desc_t *desc) {
  utillib_json_array_t *self = malloc(sizeof *self);
  json_array_init(self, data, offset, desc);
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
 * `utillib_json_value_create(kind, base, offset, desc)'
 * where `kind' is as above, `base' is the base address of your
 * C structure data or array base address, and offset is actually the size of it
 * (if it is
 * a struct, use `sizeof *base'). Or if it is an array, `offset' is
 * the number of elements of it (that is, sizeof(A)/sizeof(A[0])).
 * The `desc' is a piece of meta data about `base' and `offset' which
 * guides how to interprete the data. In case of JSON object, you need
 * to write a null-terminated array of `utillib_json_object_field_t'
 * to give name, offset in struct and function to create the corresponding
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
  size_t offset;
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
    data = va_arg(ap, void *);
    return utillib_json_string_create(data, 0);
  case UT_JSON_OBJECT:
    data = va_arg(ap, void *);
    offset = va_arg(ap, size_t);
    desc = va_arg(ap, void *);
    return utillib_json_object_create(data, offset, desc);
  case UT_JSON_ARRAY:
    data = va_arg(ap, void *);
    offset = va_arg(ap, size_t);
    desc = va_arg(ap, void *);
    return utillib_json_array_create(data, offset, desc);
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
