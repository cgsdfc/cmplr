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
#ifndef UTILLIB_HASHMAP_H
#define UTILLIB_HASHMAP_H

/**
 * \file utillib/hashmap.h
 * Introduction
 * HashMap can be used to implement important
 * structures that based on fast lookup.
 * It speeds up lookup by deriving index from
 * the content of its elements and properly
 * maintaining the elements with the same index.
 * Practically usages can include:
 * 1. Caches a subset of elements for frequent
 * access.
 * 2. Checks the existence of certain elements
 * and ensures they are not discovered twice.
 * Notices that HashMap serves as a aid of lookup
 * rather than container of a large chunk of elements.
 * The key can be part of a struct that serves as
 * the value in a HashMap and a struct can use
 * different fields as keys each of which comprises
 * a HashMap.
 * 
 * Rehashing policy
 * When the elements of a HashMap increase, most likely
 * the speed of lookup will decrease. A HashMap is said
 * to be auto-rehashing if it can detect the load factor
 * itself and adjust the size of buckets accordingly.
 * However, since this implementation aims at small
 * size of struct and simple logic, it does not rehash
 * automatically, but rather requires the client to 
 * decide when should do the rehashing. This means the
 * implementation does provide way to count elements
 * and way to do rehashing. All the client needs to do
 * is analyzing the statistic and rehash.
 * 
 */

#include "config.h"
#include "json.h"
#include <stddef.h>

struct utillib_hashmap_callback {
  size_t (*hash_handler )(void const *self);
  int (*compare_handler )(void const *lhs, void const *rhs);
};

struct utillib_hashmap {
  struct utillib_hashmap_callback const *callback;
  struct utillib_slist * buckets;
  size_t buckets_size;
};

/**
 * \function utillib_hashmap_init
 * Initializes an empty hashmap with a default
 * buckets_size and a bucket array of that size.
 * The `callback' determines how the keys will be 
 * hashed and compared from then on.
 */
void utillib_hashmap_init(struct utillib_hashmap *self,
                                struct utillib_hashmap_callback const *callback);
/**
 * \function utillib_hashmap_init_string
 * Same as `utillib_hashmap_init' but binds the `callback'
 * to the suite of `const char*' which is frequently used.
 * If initialized in this way, the client can use 
 * `utillib_hashmap_json_object_create' to get a JSON view
 * resembling an object that has named fields.
 */
void utillib_hashmap_init_string(struct utillib_hashmap *self);

/**
 * \function utillib_hashmap_destroy
 * Destructs `self'.
 */
void utillib_hashmap_destroy(struct utillib_hashmap *self);

/**
 * \function utillib_hashmap_destroy_owning
 * Destructs `self' and passes each key to 
 * `key_destroy', each value to `value_destroy'.
 * Notices that one of `key_destroy' and `value_destroy'
 * can be `NULL', which will not be called. If both are
 * `NULL', please use `utillib_hashmap_destroy' which is
 * more efficient.
 */
void utillib_hashmap_destroy_owning(struct utillib_hashmap *self,
                                          void (*key_destroy)(void *key),
                                          void (*value_destroy)(void *value));

/**
 * \function utillib_hashmap_insert
 * Inserts the key-value pair to self. If 
 * the key already existed, the insertion failed.
 * Returns zero if insertion success.
 * Returns non-zero if insertion failed.
 */
int utillib_hashmap_insert(struct utillib_hashmap *self, void const *key, void const *value);

/**
 * \function utillib_hashmap_update
 * Updates the value at `key'. If the key did not exist
 * before, `value' is returned. Otherwise, the old value
 * at `key' is returned.
 * This function alway modifies `self'.
 */
void * utillib_hashmap_update(struct utillib_hashmap *self, void const *key, void const *value);

/**
 * \function utillib_hashmap_discard
 * Discards the value at `key'. If no such value at `key',
 * `NULL' is returned. Otherwise, the discarded value is 
 * returned.
 */
void *utillib_hashmap_discard(struct utillib_hashmap *self, void const *key);

/**
 * \function utillib_hashmap_clear
 * Discards all the keys and values.
 * Notices if `self' is owning these keys and values, memory
 * leak may happen because no clean up of them will be performed.
 */
void utillib_hashmap_clear(struct utillib_hashmap *self);

/**
 * \function utillib_hashmap_at
 * Searches for value corresponding to this `key'
 * and return the address of the value if it was found.
 * Returns `NULL' if not.
 */
void * utillib_hashmap_at(struct utillib_hashmap const* self, void const *key);
/**
 * \function utillib_hashmap_size
 * Counts the elements and return the number of them.
 * Run linearly of the size of `self'.
 */
size_t utillib_hashmap_size(struct utillib_hashmap const *self);
/**
 * \function utillib_hashmap_empty
 * Decides whether `self' is empty.
 * Runs linearly of the size of the internal buckets.
 */
bool utillib_hashmap_empty(struct utillib_hashmap const *self);

/**
 * \function utillib_hashmap_json_object_create
 * Assumes that the type of key is `char const*' and
 * returns a JSON object representation of self like
 * { "key": value }.
 * The value part is created using the `create_func'
 * argument.
 * if the type of key is not `char const*', that is,
 * self is not initialized by `utillib_hashmap_init_string'.
 * it returns `NULL' to indicate that error.
 */
struct utillib_json_value*
utillib_hashmap_json_object_create(struct utillib_hashmap *self,
    utillib_json_value_create_func_t create_func);

/**
 * \function utillib_hashmap_json_array_create
 * Does not assume anything about the key and value.
 * Creates a JSON array of JSON objects each of which
 * has the form { "key": key_value, "value": value_value }.
 */
struct utillib_json_value *
utillib_hashmap_json_array_create(struct utillib_hashmap *self,
    utillib_json_value_create_func_t key_create,
    utillib_json_value_create_func_t value_create);

#endif /* UTILLIB_HASHMAP_H */
