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
#include "types.h"
#include "vector.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

UTILLIB_ENUM_BEGIN(utillib_unordered_map_retval_t)
// for mode find_only
    UTILLIB_ENUM_ELEM(KEY_FOUND)
    UTILLIB_ENUM_ELEM(KEY_MISSING)
// for mode force_insert
    UTILLIB_ENUM_ELEM(KEY_EXISTS)
    UTILLIB_ENUM_ELEM(KEY_INSERTED)
    UTILLIB_ENUM_END(utillib_unordered_map_retval_t)

typedef struct {
	utillib_hash_func_t *un_hash;
	utillib_equal_func_t *un_equal;
} const *utillib_unordered_map_ft, utillib_unordered_map_ft_impl;

typedef struct utillib_unordered_map {
	utillib_unordered_map_ft un_ft;
	utillib_vector un_bucket;
	double un_max_lf;
	size_t un_nbucket;
	size_t un_size;
	// manage memory of utillib_pair_t
	utillib_pair_t *un_free;
} utillib_unordered_map;

typedef struct utillib_unordered_map_iterator {
	struct utillib_vector_iterator iter_slot;
	struct utillib_slist_iterator iter_node;
} utillib_unordered_map_iterator;

/* utillib_unordered_map_iterator */
void utillib_unordered_map_iterator_init(utillib_unordered_map_iterator *,
					 utillib_unordered_map *);
bool utillib_unordered_map_iterator_has_next(utillib_unordered_map_iterator
					     *);
void utillib_unordered_map_iterator_next(utillib_unordered_map_iterator *);
utillib_pair_t
    *utillib_unordered_map_iterator_get(utillib_unordered_map_iterator *);

/* constructor destructor */
void utillib_unordered_map_init_from_array(utillib_unordered_map *, utillib_unordered_map_ft, utillib_pair_t const *);	// NULL teminated
void utillib_unordered_map_init(utillib_unordered_map *,
				utillib_unordered_map_ft);
void utillib_unordered_map_destroy(utillib_unordered_map *);
void utillib_unordered_map_destroy_owning(utillib_unordered_map *,
					  utillib_destroy_func_t *);

/* modifier */
int utillib_unordered_map_emplace(utillib_unordered_map *, utillib_key_t,
				  utillib_value_t);
int utillib_unordered_map_insert(utillib_unordered_map *,
				 utillib_pair_t const *);
int utillib_unordered_map_erase(utillib_unordered_map *, utillib_key_t);
void utillib_unordered_map_set_max_load_factor(utillib_unordered_map *,
					       double);
void utillib_unordered_map_rehash(utillib_unordered_map *, size_t);
void utillib_unordered_map_clear(utillib_unordered_map *);

/* observer */
utillib_pair_t *utillib_unordered_map_find(utillib_unordered_map *,
					   utillib_key_t);
utillib_unordered_map_ft utillib_unordered_map_const_charp_ft(void);
size_t utillib_unordered_map_size(utillib_unordered_map *);
size_t utillib_unordered_map_bucket_count(utillib_unordered_map *);
bool utillib_unordered_map_empty(utillib_unordered_map *);
double utillib_unordered_map_load_factor(utillib_unordered_map *);
#endif				// UTILLIB_UNORDERED_MAP_H
