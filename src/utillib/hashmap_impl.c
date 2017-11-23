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
#include "hashmap_impl.h"
#include "pair.h"
#include "slist.h"
#include <assert.h>

static void search_result_lookup(
    struct utillib_hashmap_search_result  *self,
    void const *key, int (*compare_handler) (void const* lhs, void const *rhs))
{
  UTILLIB_SLIST_FOREACH(struct utillib_pair*, pair, self->bucket) {
    if (0 == compare_handler(pair->up_first, key)) {
      self->pair=pair;
      return ;
    }
    ++self->itempos;
  }
  self->pair=NULL;
}

void utillib_hashmap_search_result_init(
    struct utillib_hashmap_search_result *self,
    struct utillib_hashmap const *hashmap,
    void const *key)
{
  struct utillib_hashmap_callback const * callback=hashmap->callback;
  assert (callback && "Callback should not be NULL");
  size_t index=hashmap_indexof(key, callback->hash_handler, hashmap->buckets_size);
  hashmap_check_range(index, hashmap->buckets_size);
  self->bucket=&hashmap->buckets[index];
  self->itempos=0;
  search_result_lookup(self, key, callback->compare_handler);
}



