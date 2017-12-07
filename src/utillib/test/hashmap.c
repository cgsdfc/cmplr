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

#include <utillib/hashmap.h>
#include <utillib/json.h>
#include <utillib/strhash.h>
#include <utillib/symbol.h>
#include <utillib/test.h>

static const struct utillib_hashmap_callback string_callback = {
    .compare_handler = strcmp, .hash_handler = lh_strhash,
};

UTILLIB_TEST_SET_UP() {}
UTILLIB_TEST_TEAR_DOWN() { utillib_hashmap_destroy(UT_FIXTURE); }

UTILLIB_TEST(hashmap_insert) {

  char const *const keys[] = {"impressive",   "excellent", "invaluable",
                              "professional", "payload",   NULL};

  char const *const values[] = {"You were very impressive tonight",
                                "Practice makes you excellent",
                                "Friendship is an invaluable treasure",
                                "Please be more professional to your work",
                                "The B2 is fully loaded with payload",
                                NULL};

  utillib_hashmap_init(UT_FIXTURE, &string_callback);
  for (char **key = keys, **value = values; *key != NULL; ++key, ++value) {
    int retv = utillib_hashmap_insert(UT_FIXTURE, *key, *value);
    UTILLIB_TEST_ASSERT_EQ(retv, 0);
    char const *expected_value = utillib_hashmap_at(UT_FIXTURE, *key);
    UTILLIB_TEST_ASSERT_EQ(expected_value, *value);
  }
}

UTILLIB_TEST(hashmap_update) {
  utillib_hashmap_init(UT_FIXTURE, &string_callback);
  char const *old_value = "No I am not";
  char const *new_value = "Yes I am";
  char const *exist_key = "Are you kiding?";
  int retv;
  retv = utillib_hashmap_insert(UT_FIXTURE, exist_key, old_value);
  UTILLIB_TEST_ASSERT_EQ(retv, 0);
  char const *actual_value =
      utillib_hashmap_update(UT_FIXTURE, exist_key, new_value);
  /* The old value should be returned */
  UTILLIB_TEST_ASSERT_EQ(actual_value, old_value);

  /* The new value should be returned */
  actual_value = utillib_hashmap_update(UT_FIXTURE, "I am joking", new_value);
  UTILLIB_TEST_ASSERT_EQ(actual_value, new_value);
}

UTILLIB_TEST(hashmap_discard) {
  utillib_hashmap_init(UT_FIXTURE, &string_callback);
  char const *key = "The key to discard";
  char const *value = "The value to discard";
  utillib_hashmap_insert(UT_FIXTURE, key, value);
  char const *actual_value = utillib_hashmap_discard(UT_FIXTURE, key);
  UTILLIB_TEST_ASSERT_EQ(value, actual_value);
  UTILLIB_TEST_ASSERT_EQ(utillib_hashmap_at(UT_FIXTURE, key), NULL);

  /* Now discard a non-exist_key */
  actual_value = utillib_hashmap_discard(UT_FIXTURE, "Not exist_key");
  UTILLIB_TEST_ASSERT_EQ(NULL, actual_value);
}

UTILLIB_TEST(hashmap_json_object) {

  utillib_hashmap_init(UT_FIXTURE, &string_callback);
  utillib_hashmap_insert(UT_FIXTURE, "symbol1", UTILLIB_SYMBOL_EOF);
  utillib_hashmap_insert(UT_FIXTURE, "symbol2", UTILLIB_SYMBOL_EPS);
  utillib_hashmap_insert(UT_FIXTURE, "symbol3", UTILLIB_SYMBOL_ERR);

  struct utillib_json_value *val = utillib_hashmap_json_object_create(
      UT_FIXTURE, utillib_symbol_json_object_create);
  utillib_json_pretty_print(val);
  utillib_json_value_destroy(val);
}

static struct utillib_json_value *json_string_create_adapted(char const *str) {
  return utillib_json_string_create(&str);
}

UTILLIB_TEST(hashmap_json_array) {
  utillib_hashmap_init(UT_FIXTURE, &string_callback);
  utillib_hashmap_insert(UT_FIXTURE, "symbol1", UTILLIB_SYMBOL_EOF);
  utillib_hashmap_insert(UT_FIXTURE, "symbol2", UTILLIB_SYMBOL_EPS);
  utillib_hashmap_insert(UT_FIXTURE, "symbol3", UTILLIB_SYMBOL_ERR);

  struct utillib_json_value *val =
      utillib_hashmap_json_array_create(UT_FIXTURE, json_string_create_adapted,
                                        utillib_symbol_json_object_create);
  utillib_json_pretty_print(val);
  utillib_json_value_destroy(val);
}

UTILLIB_TEST(hashmap_size) {
  UTILLIB_TEST_ASSERT_EQ(utillib_hashmap_size(UT_FIXTURE), 0);
  char const *input[] = {"@1", "@2", "@3", "@4", NULL};
  for (char const **s = input; *s != NULL; ++s) {
    utillib_hashmap_insert(UT_FIXTURE, *s, *s);
  }
  UTILLIB_TEST_ASSERT_EQ(utillib_hashmap_size(UT_FIXTURE), 4);
}

/*
 * We have to make sure after
 * rehash:
 * 1. The content is the same.
 * 2. No memory leak.
 */
UTILLIB_TEST(hashmap_rehash) {
  char const *input[] = {"@1", "@2", "@3", "@4", "#1", "#2", "#3", "#4", "$1",
                         "$2", "$3", "$4", "&1", "&2", "&3", "&4", NULL};
  utillib_hashmap_init(UT_FIXTURE, &string_callback);
  for (char const **s = input; *s != NULL; ++s) {
    utillib_hashmap_insert(UT_FIXTURE, *s, *s);
  }
  for (char const **s = input; *s != NULL; ++s) {
    char const *value = utillib_hashmap_at(UT_FIXTURE, *s);
    UTILLIB_TEST_ASSERT_NE(NULL, value);
    UTILLIB_TEST_ASSERT_EQ(value, *s);
  }
  utillib_hashmap_print_buckets(UT_FIXTURE);
  utillib_hashmap_rehash(UT_FIXTURE);
  utillib_hashmap_print_buckets(UT_FIXTURE);
  for (char const **s = input; *s != NULL; ++s) {
    char const *value = utillib_hashmap_at(UT_FIXTURE, *s);
    UTILLIB_TEST_ASSERT_NE(NULL, value);
    UTILLIB_TEST_ASSERT_EQ(value, *s);
  }
}

UTILLIB_TEST_DEFINE(Utillib_HashMap) {
  UTILLIB_TEST_BEGIN(Utillib_HashMap)
  UTILLIB_TEST_RUN(hashmap_insert)
  UTILLIB_TEST_RUN(hashmap_update)
  UTILLIB_TEST_RUN(hashmap_discard)
  UTILLIB_TEST_RUN(hashmap_json_object)
  UTILLIB_TEST_RUN(hashmap_json_array)
  UTILLIB_TEST_RUN(hashmap_rehash)
  UTILLIB_TEST_END(Utillib_HashMap)
  UTILLIB_TEST_FIXTURE(struct utillib_hashmap)
  UTILLIB_TEST_RETURN(Utillib_HashMap)
}
