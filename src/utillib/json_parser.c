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

#include "json_parser.h"
#include "json_impl.h" /* json_value_check_kind */
#include "json_parser_impl.h"
#include "json_parser_table.c" /* generated */
#include "json_scanner.h"
#include "pair.h"
#include <assert.h>
#include <stdlib.h>
#ifndef NODEBUG
static const double _PI = 3.1415926;
#endif

/**
 * \function json_parser_values_pop_back
 * Pops off the top of `values' as return value.
 * Assuming `values' is no empty.
 */
static struct utillib_json_value *
json_parser_values_pop_back(struct utillib_vector *values) {
  struct utillib_json_value *val = utillib_vector_back(values);
  utillib_vector_pop_back(values);
  return val;
}

/**
 * \function json_parser_object_addval
 * Pops off a value from `values'
 * Called when `JSON_SYM_RB' was seen.
 */
static void json_parser_object_addval(struct utillib_vector *values) {
  struct utillib_json_value *val = json_parser_values_pop_back(values);
  struct utillib_json_value *object = utillib_vector_back(values);
  struct utillib_pair *pair = utillib_json_object_back(object);
  assert(pair->up_first && "The key of a strval should not be NULL");
  pair->up_second = val;
}

/**
 * \function json_parser_object_addkey
 * Adds the key as `char const*' to json object.
 * Called when `JSON_SYM_COLON' was seen.
 */
static void json_parser_object_addkey(struct utillib_vector *values) {
  struct utillib_json_value *keyval = json_parser_values_pop_back(values);
  json_value_check_kind(keyval,
                        UT_JSON_STRING); /* It should be a json string */
  char const *key = keyval->as_ptr;
  free(keyval); /* It may be cached */
  struct utillib_json_value *object = utillib_vector_back(values);
  utillib_json_object_push_back(object, key, NULL);
}

/**
 * \function json_parser_array_addval
 * Called when `JSON_SYM_RK' was seen.
 */
static void json_parser_array_addval(struct utillib_vector *values) {
  struct utillib_json_value *val = json_parser_values_pop_back(values);
  struct utillib_json_value *array = utillib_vector_back(values);
  utillib_json_array_push_back(array, val);
}

/**
 * \function json_parser_addval
 * Adds value to array or object.
 * Called when `JSON_SYM_COMMA' was seen.
 */
static void json_parser_addval(struct utillib_vector *values) {
  size_t size = utillib_vector_size(values);
  struct utillib_json_value *entity = utillib_vector_at(values, size - 2);
  switch (entity->kind) {
  case UT_JSON_ARRAY:
    json_parser_array_addval(values);
    return;
  case UT_JSON_OBJECT:
    json_parser_object_addval(values);
    return;
  default:
    assert(false && "The entity should be either JSON array or object");
  }
}

static void json_parser_rule_handler(void *_self,
                                     struct utillib_rule const *rule) {

  struct utillib_json_parser *self = _self;
  struct utillib_symbol const *LHS = rule->LHS;
  struct utillib_json_value const *val = NULL;
  switch (LHS->value) {
  case JSON_SYM_ARR:
    val = utillib_json_array_create_empty();
    break;
  case JSON_SYM_OBJ:
    val = utillib_json_object_create_empty();
    break;
  }
  if (val)
    utillib_vector_push_back(&self->values, val);
}

/*
   * This switch is so complicated
   * because empty case should handle
   * correctly. The strategy is as follow:
   * 1. Handles the `UT_SYM_EPS' by pushing
   * `NULL' into `values'.
   * 2. It is known that `JSON_SYM_RB' or
   * `JSON_SYM_RK' must follow `UT_SYM_EPS'
   * so we pop off the `NULL' in these cases.
   * 3. if after (2), the top is `NULL', we
   * know that sth empty here, so we simpily
   * pop off the `NULL'.
   * 4. Otherwise, there is a meaningful value
   * on the top and we add it to the top-1 value
   * which should be an array or object.
   */
static void json_parser_terminal_handler(struct utillib_json_parser *self,
                                         struct utillib_symbol const *symbol,
                                         void const *semantic) {
  struct utillib_vector *values = &self->values;

  switch (symbol->value) {
  case JSON_SYM_TRUE:
  case JSON_SYM_FALSE:
  case JSON_SYM_NULL:
  case JSON_SYM_REAL:
  case JSON_SYM_LONG:
  case JSON_SYM_STR:
    utillib_vector_push_back(values, semantic);
    return;
  default:
    break;
  }
  switch (symbol->value) {
  case JSON_SYM_RB:
    utillib_vector_pop_back(values);
    if (utillib_vector_back(values)) {
      json_parser_object_addval(values);
      return;
    }
    utillib_vector_pop_back(values);
    return;
  case JSON_SYM_RK:
    utillib_vector_pop_back(values);
    if (utillib_vector_back(values)) {
      json_parser_array_addval(values);
      return;
    }
    utillib_vector_pop_back(values);
    return;
  case JSON_SYM_COMMA:
    json_parser_addval(values);
    return;
  case JSON_SYM_COLON:
    json_parser_object_addkey(values);
    return;
  case UT_SYM_EPS:
    utillib_vector_push_back(values, NULL);
    return;
  }
}

static void
json_parser_error_handler(void *_self, void *input,
                          struct utillib_ll1_parser_error const *error) {
  puts("ERROR");
}

static const struct utillib_ll1_parser_callback json_parser_callback = {
    .terminal_handler = json_parser_terminal_handler,
    .rule_handler = json_parser_rule_handler,
    .error_handler = json_parser_error_handler,
};

void utillib_json_parser_factory_init(
    struct utillib_ll1_factory *self) {
  utillib_ll1_factory_build_init(self, utillib_json_symbols, utillib_json_rules);
}

void utillib_json_parser_init(struct utillib_json_parser *self,
                              struct utillib_ll1_factory *factory) {
  utillib_ll1_factory_parser_init(factory, &self->parser, self,
                                  &json_parser_callback);
  utillib_vector_init(&self->values);
}

void utillib_json_parser_destroy(struct utillib_json_parser *self) {
  utillib_vector_destroy(&self->values);
  utillib_ll1_parser_destroy(&self->parser);
}

struct utillib_json_value *
utillib_json_parser_parse(struct utillib_json_parser *self, char const *str) {
  struct utillib_json_scanner scanner;
  utillib_json_scanner_init(&scanner, str);
  if (!utillib_ll1_parser_parse(&self->parser, &scanner,
                                &utillib_json_scanner_op))
    return NULL;
  struct utillib_json_value *val = json_parser_values_pop_back(&self->values);
  return val;
}

#ifndef NDEBUG
bool utillib_json_parser_parse_dbg(struct utillib_json_parser *self,
                                   size_t const *symbols) {
  struct utillib_symbol_scanner scanner;
  utillib_symbol_scanner_init(&scanner, symbols, utillib_json_symbols);
  bool good = utillib_ll1_parser_parse(&self->parser, &scanner,
                                       &utillib_symbol_scanner_op);
  UTILLIB_VECTOR_FOREACH(struct utillib_json_value const *, val,
                         &self->values) {
    utillib_json_pretty_print(val, stderr);
  }
  return good;
}
#endif
