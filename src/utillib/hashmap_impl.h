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
#ifndef UTILLIB_HASHMAP_IMPL_H
#define UTILLIB_HASHMAP_IMPL_H
#include "hashmap.h"
#define hashmap_indexof(key, hash_handler, buckets_size)                       \
  (hash_handler(key) & (buckets_size - 1))
#define hashmap_check_range(index, buckets_size)                               \
  do {                                                                         \
    assert((index) < (buckets_size) && "Index out of range");                  \
  } while (0)

/**
 * \struct utillib_hashmap_search_result
 * Helper to keep the result of one search
 */
struct utillib_hashmap_search_result {
  size_t itempos;
  struct utillib_slist *bucket;
  struct utillib_pair *pair;
};

void utillib_hashmap_search_result_init(
    struct utillib_hashmap_search_result *self,
    struct utillib_hashmap const *hashmap, void const *key);

#endif /* UTILLIB_HASHMAP_IMPL_H */
