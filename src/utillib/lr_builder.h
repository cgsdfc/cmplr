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

#ifndef UTILLIB_LR_BUILDER_H
#define UTILLIB_LR_BUILDER_H
#include "bitset.h"
#include "graph.h"
#include "hashmap.h"
#include "list.h"
#include "rule.h"

/**
 * \struct utillib_lr_item
 *
 *
 */
struct utillib_lr_item {
  size_t pos;
  size_t value;
  struct utillib_rule const *rule;
  struct utillib_symbol const *prefixed;
};

/**
 * \struct utillib_lr_itemset
 * A set of LR(0) items. Reprensented
 * in tree structure and have a bitset
 * as its hash value on the global itemset_index.
 * The gotosets uses the `access' field
 * as hash value.
 */

struct utillib_lr_itemset {
  struct utillib_symbol const *access;
  struct utillib_bitset bitset;
  struct utillib_vector items;
  size_t value;
};

struct utillib_lr_builder {
  struct utillib_rule_index *rule_index;
  struct utillib_hashmap itemset_index;
  struct utillib_hashmap item_index;
  struct utillib_vector gotosets;
  size_t items_count;
  struct utillib_list queue;
  bool *closure_added;
  size_t non_terminals_size;
  size_t max_items;
};

#endif /* UTILLIB_LR_BUILDER_H */
