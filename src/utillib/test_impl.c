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
#include "test_impl.h"
#include "test.h"
#include "enum.h"
#include "json.h"

/*
 * JSON interface
 */

/**
 * \function json_test_entry_create
 * Creates JSON value from `struct utillib_test_entry'.
 */
static struct utillib_json_value *
test_entry_json_object_create(struct utillib_test_entry const *self) {
  char const *kind_str = utillib_test_status_kind_tostring(self->status);
  struct utillib_json_value *object = utillib_json_object_create_empty();
  utillib_json_object_push_back(object, "name",
                                utillib_json_string_create(&self->func_name));
  utillib_json_object_push_back(object, "status",
                                utillib_json_string_create(&kind_str));
  utillib_json_object_push_back(object, "result",
                                utillib_json_bool_create(&self->succeeded));
  return object;
}

/**
 * \function json_test_entry_array_pointer_create
 * Creates the JSON array of `struct utillib_test_entry'
 */

static struct utillib_json_value *
test_entry_json_array_create(struct utillib_test_env const *self) {
  struct utillib_json_value *array = utillib_json_array_create_empty();
  for (struct utillib_test_entry const *item = self->cases;
       item->func != NULL; ++item) {
    utillib_json_array_push_back(array, test_entry_json_object_create(item));
  }
  return array;
}

/**
 * \function json_test_env_create
 * Wraps `TestEnv_Fields'.
 */
static struct utillib_json_value *
test_env_json_object_create(struct utillib_test_env const *self) {
  struct utillib_json_value *object = utillib_json_object_create_empty();
  utillib_json_object_push_back(object, "filename",
                                utillib_json_string_create(&self->filename));
  utillib_json_object_push_back(object, "name",
                                utillib_json_string_create(&self->case_name));
  utillib_json_object_push_back(object, "ntests",
                                utillib_json_size_t_create(&self->ntests));
  utillib_json_object_push_back(object, "nskipped",
                                utillib_json_size_t_create(&self->nskipped));
  utillib_json_object_push_back(object, "nrun",
                                utillib_json_size_t_create(&self->nrun));
  utillib_json_object_push_back(object, "npassed",
                                utillib_json_size_t_create(&self->nsuccess));
  utillib_json_object_push_back(object, "nfailed",
                                utillib_json_size_t_create(&self->nfailure));
  utillib_json_object_push_back(object, "details",
                                test_entry_json_array_create(self));
  return object;
}

struct utillib_json_value const *
utillib_test_suite_json_object_create(struct utillib_test_suite const *self) {
  struct utillib_json_value *object = utillib_json_object_create_empty();
  utillib_json_object_push_back(object, "filename",
                                utillib_json_string_create(&self->filename));
  utillib_json_object_push_back(
      object, "tests", utillib_vector_json_array_create(
                           &self->tests, test_entry_json_object_create));
  return object;
}
