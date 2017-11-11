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
#ifndef UTILLIB_UNORDERED_MAP_H
#define UTILLIB_UNORDERED_MAP_H
/**
 * \file utillib/unordered_map.h
 * Linked hash map.
 */

#include "enum.h"
#include "pair.h"
#include "slist.h"
#include "unordered_op.h"
#include "vector.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

UTILLIB_ENUM_BEGIN(utillib_unordered_map_retval_t)
/* for mode find_only */
UTILLIB_ENUM_ELEM(KEY_FOUND)
UTILLIB_ENUM_ELEM(KEY_MISSING)
/* for mode force_insert */
UTILLIB_ENUM_ELEM(KEY_EXISTS)
UTILLIB_ENUM_ELEM(KEY_INSERTED)
UTILLIB_ENUM_END(utillib_unordered_map_retval_t)

struct utillib_unordered_map {
  struct utillib_unordered_op *un_op;
  struct utillib_vector un_bucket;
  double un_max_lf;
  size_t un_nbucket;
  size_t un_size;
  /* manage memory of utillib_pair_t */
  struct utillib_pair_t *un_free;
};

struct utillib_unordered_map_iterator {
  struct utillib_vector_iterator iter_slot;
  struct utillib_slist_iterator iter_node;
};

/* utillib_unordered_map_iterator */
void utillib_unordered_map_iterator_init(
    struct utillib_unordered_map_iterator *, struct utillib_unordered_map *);
bool utillib_unordered_map_iterator_has_next(
    struct utillib_unordered_map_iterator *);
void utillib_unordered_map_iterator_next(
    struct utillib_unordered_map_iterator *);
struct utillib_pair_t *
utillib_unordered_map_iterator_get(struct utillib_unordered_map_iterator *);

/* constructor destructor */
void utillib_unordered_map_init_from_array(
    struct utillib_unordered_map *, struct utillib_unordered_op *,
    struct utillib_pair_t const *); /* NULL teminated */
void utillib_unordered_map_init(struct utillib_unordered_map *,
                                struct utillib_unordered_op *);
void utillib_unordered_map_destroy(struct utillib_unordered_map *);
void utillib_unordered_map_destroy_owning(struct utillib_unordered_map *,
                                          void (*destroy)(void*));

/* modifier */
int utillib_unordered_map_emplace(struct utillib_unordered_map *, void const*,
                                  void const*);
int utillib_unordered_map_insert(struct utillib_unordered_map *,
                                 struct utillib_pair_t const *);
int utillib_unordered_map_erase(struct utillib_unordered_map *,void const* );
void utillib_unordered_map_set_max_load_factor(struct utillib_unordered_map *,
                                               double);
void utillib_unordered_map_rehash(struct utillib_unordered_map *, size_t);
void utillib_unordered_map_clear(struct utillib_unordered_map *);

/* observer */
struct utillib_pair_t *
utillib_unordered_map_find(struct utillib_unordered_map *, void const*);
size_t utillib_unordered_map_size(struct utillib_unordered_map *);
size_t utillib_unordered_map_bucket_count(struct utillib_unordered_map *);
bool utillib_unordered_map_empty(struct utillib_unordered_map *);
double utillib_unordered_map_load_factor(struct utillib_unordered_map *);
#endif /* UTILLIB_UNORDERED_MAP_H */
