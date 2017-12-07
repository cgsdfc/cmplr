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
#include "json_impl.h"
#include "pair.h"
#include "print.h" // for utillib_static_sprintf
#include "string.h"
#include <stdarg.h> // for va_list
#include <stdlib.h> // for free
#include <string.h>
#include <strings.h>

/**
 * \file utillib/json.c
 * Provides mapping between C structure and JSON
 * data format.
 */

const struct utillib_json_value utillib_json_true = {.kind = UT_JSON_BOOL};
const struct utillib_json_value utillib_json_false = {.kind = UT_JSON_BOOL};
const struct utillib_json_value utillib_json_null = {.kind = UT_JSON_NULL};

static void json_array_init(struct utillib_json_array *self) {
  utillib_vector_init(&self->elements);
}

static void json_object_init(struct utillib_json_object *self) {
  utillib_vector_init(&self->members);
}
/**
 * \function json_value_create_ptr
 * Used by utillib_json_array_create
 * and utillib_json_object_create
 */
static struct utillib_json_value *json_value_create_ptr(int kind, void *data) {
  struct utillib_json_value *self = malloc(sizeof *self);
  self->as_ptr = data;
  self->kind = kind;
  return self;
}

/**
 * \function json_object_register
 * \param base Pointer to a struct.
 * \param fields Description of the fields of this struct
 * Notes the key was `strdup'.
 */
static void
json_object_register(struct utillib_json_object *self, char const *base,
                     struct utillib_json_object_field const *fields) {
  for (struct utillib_json_object_field const *field = fields;
       field->create_func != NULL; ++field) {
    struct utillib_json_value const *value =
        field->create_func(base + field->offset);
    const char *key = strdup(field->key);
    utillib_vector_push_back(&self->members, utillib_make_pair(key, value));
  }
}

/**
 * \function json_array_register
 * \param base Pointer to the begin of the array.
 * \param desc Description of this array.
 */
static void json_array_register(struct utillib_json_array *self,
                                char const *base,
                                struct utillib_json_array_desc const *desc) {
  for (int i = 0; i < desc->size; ++i, base += desc->elemsz) {
    utillib_vector_push_back(&self->elements, desc->create_func(base));
  }
}

/**
 * \function json_object_destroy
 * Destructs a single member of a larger object.
 * Recursively desctructs the value of the member.
 */
static void json_object_member_destroy(void *_self) {
  struct utillib_pair *self = _self;
  utillib_json_value_destroy(self->up_second);
  free(self->up_first);
  free(self);
}

/**
 * \function utillib_json_object_destroy
 * Destructs a JSON object.
 */
static void json_object_destroy(struct utillib_json_object *self) {
  utillib_vector_destroy_owning(&self->members, json_object_member_destroy);
  free(self);
}

/**
 * \function json_array_destroy
 * Destructs a JSON array.
 * 1. Destructs each element of it as a `utillib_json_value'.
 * 2. And the vector holding them.
 */
static void json_array_destroy(struct utillib_json_array *self) {
  utillib_vector_destroy_owning(&self->elements, utillib_json_value_destroy);
  free(self);
}

void utillib_json_array_desc_init(
    struct utillib_json_array_desc *self, size_t elemsz, size_t size,
    utillib_json_value_create_func_t create_func) {
  self->elemsz = elemsz;
  self->size = size;
  self->create_func = create_func;
}

/**
 * \function utillib_json_value_destroy
 * Destructs a JSON value according to its type.
 */
void utillib_json_value_destroy(struct utillib_json_value *self) {
  switch (self->kind) {
  case UT_JSON_ARRAY:
    json_array_destroy(self->as_ptr);
    free(self);
    return;
  case UT_JSON_OBJECT:
    json_object_destroy(self->as_ptr);
    free(self);
    return;
  case UT_JSON_STRING:
    free(self->as_ptr);
    free(self);
    return;
  case UT_JSON_NULL:
  case UT_JSON_BOOL:
    /* pass */
    return;
  default:
    free(self);
    return;
  }
}

struct utillib_json_value *utillib_json_bool_create(bool const *base) {
  return *base ? &utillib_json_true : &utillib_json_false;
}

struct utillib_json_value *utillib_json_string_create(char const *const *str) {
  struct utillib_json_value *self = malloc(sizeof *self);
  self->kind = UT_JSON_STRING;
  self->as_ptr = strdup(*str);
  return self;
}

#define JSON_PRIMARY_CREATE_FUNC_DEFINE(NAME, FIELD, TYPE, VALUE)              \
  struct utillib_json_value *NAME(TYPE const *base) {                          \
    struct utillib_json_value *self = malloc(sizeof *self);                    \
    self->FIELD = *base;                                                       \
    self->kind = (VALUE);                                                      \
    return self;                                                               \
  }

JSON_PRIMARY_CREATE_FUNC_DEFINE(utillib_json_real_create, as_double, double,
                                UT_JSON_REAL)
JSON_PRIMARY_CREATE_FUNC_DEFINE(utillib_json_size_t_create, as_size_t, size_t,
                                UT_JSON_SIZE_T)
JSON_PRIMARY_CREATE_FUNC_DEFINE(utillib_json_int_create, as_int, int,
                                UT_JSON_INT)
JSON_PRIMARY_CREATE_FUNC_DEFINE(utillib_json_long_create, as_long, long,
                                UT_JSON_LONG)
JSON_PRIMARY_CREATE_FUNC_DEFINE(utillib_json_float_create, as_float, float,
                                UT_JSON_FLOAT)

/**
 * \function utillib_json_null_array_create
 * Map anything to a JSON array of `null'.
 * \param size The number of `null' in the array to create.
 * Notes that the `size' parameter here has different meaning
 * from the more general `utillib_json_array_create'.
 * As if sizeof null == 1.
 */
struct utillib_json_value *utillib_json_null_array_create(size_t size) {
  struct utillib_json_value *array = utillib_json_array_create_empty();
  for (size_t i = 0; i < size; ++i) {
    utillib_json_array_push_back(array, &utillib_json_null);
  }
  return array;
}

struct utillib_json_value *
utillib_json_object_create(void const *data,
                           const struct utillib_json_object_field *fields) {
  struct utillib_json_object *self = malloc(sizeof *self);
  json_object_init(self);
  json_object_register(self, data, fields);
  return json_value_create_ptr(UT_JSON_OBJECT, self);
}

struct utillib_json_value *
utillib_json_array_create(void const *data,
                          const struct utillib_json_array_desc *desc) {
  struct utillib_json_array *self = malloc(sizeof *self);
  json_array_init(self);
  json_array_register(self, data, desc);
  return json_value_create_ptr(UT_JSON_ARRAY, self);
}

/*
 * forward declaration.
 */

static void json_value_tostring(struct utillib_json_value const *,
                                struct utillib_string *);
/**
 * \function json_array_tostring
 */

static void json_array_tostring(struct utillib_json_array const *self,
                                struct utillib_string *string) {
  struct utillib_json_value *elem;

  utillib_string_append(string, "[");
  if (utillib_vector_empty(&self->elements)) {
    utillib_string_append(string, "]");
    return;
  }
  UTILLIB_VECTOR_FOREACH(elem, &self->elements) {
    json_value_tostring(elem, string);
    utillib_string_append(string, ",");
  }
  utillib_string_replace_last(string, ']');
}

/**
 * \function json_object_tostring
 */
static void json_object_tostring(struct utillib_json_object const *self,
                                 struct utillib_string *string) {
  struct utillib_pair *mem;

  utillib_string_append(string, "{");
  if (utillib_vector_empty(&self->members)) {
    utillib_string_append(string, "}");
    return;
  }
  UTILLIB_VECTOR_FOREACH(mem, &self->members) {
    utillib_string_append(string, "\"");
    utillib_string_append(string, mem->up_first);
    utillib_string_append(string, "\":");
    json_value_tostring(mem->up_second, string);
    utillib_string_append(string, ",");
  }
  utillib_string_replace_last(string, '}');
}

/**
 * \function json_value_tostring
 */
static void json_value_tostring(struct utillib_json_value const *self,
                                struct utillib_string *string) {
  char const *str;
  switch (self->kind) {
  case UT_JSON_INT:
    str = utillib_static_sprintf("%d", self->as_int);
    utillib_string_append(string, str);
    return;
  case UT_JSON_SIZE_T:
    str = utillib_static_sprintf("%lu", self->as_size_t);
    utillib_string_append(string, str);
    return;
  case UT_JSON_FLOAT:
    str = utillib_static_sprintf("%f", self->as_double);
    utillib_string_append(string, str);
    return;
  case UT_JSON_REAL:
    str = utillib_static_sprintf("%lf", self->as_double);
    utillib_string_append(string, str);
    return;
  case UT_JSON_LONG:
    str = utillib_static_sprintf("%ld", self->as_long);
    utillib_string_append(string, str);
    return;
  case UT_JSON_BOOL:
    utillib_string_append(string,
                          self == &utillib_json_true ? "true" : "false");
    return;
  case UT_JSON_NULL:
    utillib_string_append(string, "null");
    return;
  case UT_JSON_STRING:
    str = utillib_static_sprintf("\"%s\"", self->as_ptr);
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

/**
 * \funtion utillib_json_tostring
 * Serializes the JSON value to a string.
 * Before that, it initializes the string, so **do not**
 * initialize the string yourself.
 */

void utillib_json_tostring(struct utillib_json_value const *self,
                           struct utillib_string *str) {
  utillib_string_init(str);
  json_value_tostring(self, str);
}

/**
 * The following functions create empty JSON array or object
 * that client can fill in with push_back functions.
 * Notes the returned json value is meant to be modified
 * so no const qualifier.
 */
struct utillib_json_value *utillib_json_array_create_empty(void) {
  struct utillib_json_array *self = malloc(sizeof *self);
  json_array_init(self);
  return json_value_create_ptr(UT_JSON_ARRAY, self);
}

struct utillib_json_value *utillib_json_object_create_empty(void) {
  struct utillib_json_object *self = malloc(sizeof *self);
  json_object_init(self);
  return json_value_create_ptr(UT_JSON_OBJECT, self);
}

/**
 * Provides a way to allow client to create
 * JSON arrays and objects by manually adding
 * elements and members, which may helps them to
 * interprete the C data differently (not as the
 * contents of C struct or C array).
 * \param self Should be returned by either
 * `utillib_json_array_create_empty' or
 * `utillib_json_object_create_empty'.
 */

void utillib_json_array_push_back(struct utillib_json_value *self,
                                  struct utillib_json_value const *value) {
  json_value_check_kind(self, UT_JSON_ARRAY);
  struct utillib_json_array *array = self->as_ptr;
  utillib_vector_push_back(&array->elements, value);
}

void utillib_json_object_push_back(struct utillib_json_value *self,
                                   char const *key,
                                   struct utillib_json_value const *value) {
  json_value_check_kind(self, UT_JSON_OBJECT);
  struct utillib_json_object *object = self->as_ptr;
  utillib_vector_push_back(&object->members,
                           utillib_make_pair(strdup(key), value));
}

struct utillib_pair *utillib_json_object_back(struct utillib_json_value *self) {
  json_value_check_kind(self, UT_JSON_OBJECT);
  struct utillib_json_object *object = self->as_ptr;
  return utillib_vector_back(&object->members);
}

struct utillib_json_value *
utillib_json_array_back(struct utillib_json_value *self) {
  json_value_check_kind(self, UT_JSON_ARRAY);
  struct utillib_json_array *array = self->as_ptr;
  return utillib_vector_back(&array->elements);
}

struct utillib_json_value *utillib_json_null_create(void) {
  return &utillib_json_null;
}

/**
 * \function utillib_json_pretty_print
 * Convenient function combining `utillib_json_tostring'
 * and `utillib_printer_print_json'.
 */
void utillib_json_pretty_fprint(struct utillib_json_value const *self,
                                FILE *file) {
  struct utillib_string json;
  struct utillib_printer_t print;
  utillib_printer_init(&print, file, 4);
  utillib_json_tostring(self, &json);
  utillib_printer_print_json(&print, utillib_string_c_str(&json));
  utillib_string_destroy(&json);
}

struct utillib_json_value *
utillib_json_object_at(struct utillib_json_value const *self, char const *key) {
  json_value_check_kind(self, UT_JSON_OBJECT);
  struct utillib_pair const *pair;

  struct utillib_json_object *object = self->as_ptr;
  UTILLIB_VECTOR_FOREACH(pair, &object->members) {
    if (0 == strcmp(key, pair->up_first)) {
      return pair->up_second;
    }
  }
  return NULL;
}

struct utillib_json_value *
utillib_json_array_at(struct utillib_json_value const *self, size_t index) {
  json_value_check_kind(self, UT_JSON_ARRAY);
  struct utillib_json_array *array = self->as_ptr;
  return utillib_vector_at(&array->elements, index);
}

struct utillib_json_value *utillib_json_string_create_adaptor(char const *str) {
  return utillib_json_string_create(&str);
}

static struct utillib_json_value *
json_array_copy(struct utillib_json_array *self) {
  struct utillib_json_value *array = utillib_json_array_create_empty();
  struct utillib_json_value const *val;

  UTILLIB_VECTOR_FOREACH(val, &self->elements) {
    utillib_json_array_push_back(array, utillib_json_value_copy(val));
  }
  return array;
}

static struct utillib_json_value *
json_object_copy(struct utillib_json_object *self) {
  struct utillib_json_value *object = utillib_json_object_create_empty();
  struct utillib_pair const *pair;

  UTILLIB_VECTOR_FOREACH(pair, &self->members) {
    utillib_json_object_push_back(object, pair->up_first,
                                  utillib_json_value_copy(pair->up_second));
  }
  return object;
}

struct utillib_json_value *
utillib_json_value_copy(struct utillib_json_value const *self) {
  switch (self->kind) {
  case UT_JSON_INT:
    return utillib_json_int_create(&self->as_int);
  case UT_JSON_SIZE_T:
    return utillib_json_size_t_create(&self->as_size_t);
  case UT_JSON_FLOAT:
    return utillib_json_float_create(&self->as_double);
  case UT_JSON_REAL:
    return utillib_json_real_create(&self->as_double);
  case UT_JSON_LONG:
    return utillib_json_long_create(&self->as_long);
  case UT_JSON_NULL:
  case UT_JSON_BOOL:
    return self;
  case UT_JSON_STRING:
    return utillib_json_string_create(&self->as_ptr);
  case UT_JSON_ARRAY:
    return json_array_copy(self->as_ptr);
  case UT_JSON_OBJECT:
    return json_object_copy(self->as_ptr);
  }
}

size_t utillib_json_array_size(struct utillib_json_value const *self) {
  json_value_check_kind(self, UT_JSON_ARRAY);
  return utillib_vector_size(_JSON_ARRAY(self));
}

void utillib_json_pretty_print(struct utillib_json_value const *self) {
  return utillib_json_pretty_fprint(self, stdout);
}
