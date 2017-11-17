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
#include "enum.h"
#include "rule.h"
#include "unordered_map.h"
#include "vector.h"
#include <inttypes.h>

UTILLIB_ENUM_BEGIN(utillib_lr_entry_kind)
UTILLIB_ENUM_ELEM(UT_LR_REDUCE)
UTILLIB_ENUM_ELEM(UT_LR_SHIFT)
UTILLIB_ENUM_ELEM(UT_LR_ACCEPT)
UTILLIB_ENUM_ELEM(UT_LR_REJECT)
UTILLIB_ENUM_END(utillib_lr_entry_kind);

struct utillib_lr_entry {
  int kind;
  size_t state_id;
  size_t rule_id;
};

struct utillib_lr_item {
  size_t id;
  size_t pos;
  struct utillib_rule const *rule;
};

struct utillib_lr_itemset {
  size_t id;
  struct utillib_vector itemset;
};

struct utillib_lr_builder {
  struct utillib_rule_index *rule_index;
  struct utillib_unordered_map item_index;
  struct utillib_unordered_map itemset_index;
  struct utillib_lr_entry *state_table;
  struct utillib_vector errors;
};

void utillib_lr_item_init(struct utillib_lr_item *self,
                          struct utillib_rule const *rule);
void utillib_lr_builder_init(struct utillib_lr_builder *self,
                             struct utillib_rule_index *rule_index);
void utillib_lr_builder_build_table(struct utillib_lr_builder *self);
int utillib_lr_builder_check(struct utillib_lr_builder *self);
void utillib_lr_builder_destroy(struct utillib_lr_builder *self);

#endif /* UTILLIB_LR_BUILDER_H */
