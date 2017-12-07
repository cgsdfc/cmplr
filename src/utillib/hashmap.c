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
#include "hashmap.h"
#include "hashmap_impl.h"
#include "json_impl.h"
#include "pair.h"
#include "slist.h"
#include <assert.h>
#include <stdlib.h>

#define UTILLIB_HASHMAP_DEFAULT_BUCKETS_SIZE 16

void utillib_hashmap_init(struct utillib_hashmap *self,
                          struct utillib_hashmap_callback const *callback) {
  self->callback = callback;
  self->buckets_size = UTILLIB_HASHMAP_DEFAULT_BUCKETS_SIZE;
  self->buckets = calloc(sizeof self->buckets[0], self->buckets_size);
}

/* We are not owning elements */
void utillib_hashmap_destroy(struct utillib_hashmap *self) {
  for (int i = 0; i < self->buckets_size; ++i) {
    /* Frees those pairs */
    utillib_slist_destroy_owning(&self->buckets[i], free);
  }
  free(self->buckets);
  self->buckets = NULL;
}

void utillib_hashmap_destroy_owning(struct utillib_hashmap *self,
                                    void (*key_destroy)(void *key),
                                    void (*value_destroy)(void *value)) {
  for (int i = 0; i < self->buckets_size; ++i) {
    struct utillib_slist *bucket = &self->buckets[i];
    /* Have to traversal the list */
    UTILLIB_SLIST_FOREACH(struct utillib_pair *, pair, bucket) {
      if (key_destroy)
        key_destroy(pair->up_first);
      if (value_destroy)
        value_destroy(pair->up_second);
    }
    utillib_slist_destroy_owning(bucket, free);
  }
  free(self->buckets);
  self->buckets = NULL;
}

int utillib_hashmap_insert(struct utillib_hashmap *self, void const *key,
                           void const *value) {
  struct utillib_hashmap_search_result result;
  utillib_hashmap_search_result_init(&result, self, key);
  if (result.pair)
    return 1;

  struct utillib_pair *new_pair = utillib_make_pair(key, value);
  utillib_slist_push_front(result.bucket, new_pair);
  return 0;
}

void *utillib_hashmap_update(struct utillib_hashmap *self, void const *key,
                             void const *value) {
  struct utillib_hashmap_search_result result;
  utillib_hashmap_search_result_init(&result, self, key);
  if (result.pair) {
    void *old_value = result.pair->up_second;
    result.pair->up_second = value;
    return old_value;
  }
  struct utillib_pair *new_pair = utillib_make_pair(key, value);
  utillib_slist_push_front(result.bucket, new_pair);
  return value;
}

void *utillib_hashmap_discard(struct utillib_hashmap *self, void const *key) {
  struct utillib_hashmap_search_result result;
  struct utillib_pair *old_pair;
  void *old_value;

  utillib_hashmap_search_result_init(&result, self, key);
  if (!result.pair)
    return NULL;
  old_pair = utillib_slist_erase(result.bucket, result.itempos);
  assert(old_pair == result.pair && "should remove the same pair");
  old_value = old_pair->up_second;
  free(old_pair);
  return old_value;
}

void utillib_hashmap_rehash(struct utillib_hashmap *self) {
  struct utillib_slist *new_buckets;
  size_t new_buckets_size = self->buckets_size << 1;
  struct utillib_hashmap_callback const *callback = self->callback;

  new_buckets = calloc(sizeof new_buckets[0], new_buckets_size);
  for (int i = 0; i < self->buckets_size; ++i) {
    struct utillib_slist *old_bucket = &self->buckets[i];
    UTILLIB_SLIST_FOREACH(struct utillib_pair *, pair, old_bucket) {
      size_t index = hashmap_indexof(pair->up_first, callback->hash_handler,
                                     new_buckets_size);
      hashmap_check_range(index, new_buckets_size);
      utillib_slist_push_front(&new_buckets[index], pair);
    }
    utillib_slist_destroy(old_bucket);
  }
  free(self->buckets);
  self->buckets = new_buckets;
  self->buckets_size = new_buckets_size;
}

void *utillib_hashmap_at(struct utillib_hashmap const *self, void const *key) {
  struct utillib_hashmap_search_result result;
  utillib_hashmap_search_result_init(&result, self, key);
  if (result.pair)
    return result.pair->up_second;
  return NULL;
}

bool utillib_hashmap_exist_key(struct utillib_hashmap const *self,
                               void const *key) {
  struct utillib_hashmap_search_result result;
  utillib_hashmap_search_result_init(&result, self, key);
  return result.pair != NULL;
}

size_t utillib_hashmap_size(struct utillib_hashmap const *self) {
  size_t size = 0;
  for (int i = 0; i < self->buckets_size; ++i) {
    size += utillib_slist_size(&self->buckets[i]);
  }
  return size;
}

bool utillib_hashmap_empty(struct utillib_hashmap const *self) {
  for (int i = 0; i < self->buckets_size; ++i)
    if (!utillib_slist_empty(&self->buckets[i]))
      return false;
  return true;
}

struct utillib_json_value *utillib_hashmap_json_object_create(
    struct utillib_hashmap const *self,
    utillib_json_value_create_func_t create_func) {
  json_check_create_func(create_func);
  struct utillib_json_value *object = utillib_json_object_create_empty();
  for (int i = 0; i < self->buckets_size; ++i) {
    struct utillib_slist *bucket = &self->buckets[i];
    UTILLIB_SLIST_FOREACH(struct utillib_pair *, pair, bucket) {
      struct utillib_json_value *value = pair->up_second
                                             ? create_func(pair->up_second)
                                             : utillib_json_null_create();
      utillib_json_object_push_back(object, pair->up_first, value);
    }
  }
  return object;
}

struct utillib_json_value *utillib_hashmap_json_array_create(
    struct utillib_hashmap const *self,
    utillib_json_value_create_func_t key_create,
    utillib_json_value_create_func_t value_create) {
  json_check_create_func(key_create);
  json_check_create_func(value_create);

  struct utillib_json_value *array = utillib_json_array_create_empty();
  for (int i = 0; i < self->buckets_size; ++i) {
    struct utillib_slist *bucket = &self->buckets[i];
    UTILLIB_SLIST_FOREACH(struct utillib_pair *, pair, bucket) {
      struct utillib_json_value *object = utillib_json_object_create_empty();
      utillib_json_object_push_back(
          object, "key", pair->up_first ? key_create(pair->up_first)
                                        : utillib_json_null_create());
      utillib_json_object_push_back(
          object, "value", pair->up_second ? value_create(pair->up_second)
                                           : utillib_json_null_create());
      utillib_json_array_push_back(array, object);
    }
  }
  return array;
}

size_t utillib_hashmap_buckets_size(struct utillib_hashmap const *self) {
  return self->buckets_size;
}

#ifndef NDEBUG
void utillib_hashmap_print_buckets(struct utillib_hashmap const *self) {
  for (int i = 0; i < self->buckets_size; ++i) {
    printf("#%-4d ", i);
    struct utillib_slist *bucket = &self->buckets[i];
    UTILLIB_SLIST_FOREACH(struct utillib_pair *, pair, bucket) {
      fputs("# ", stdout);
    }
    puts("");
  }
  size_t size = utillib_hashmap_size(self);
  printf("buckets_size is %lu\n", self->buckets_size);
  printf("size is %lu\n", size);
  printf("load factor is %f\n", (float)size / self->buckets_size);
}

#endif
