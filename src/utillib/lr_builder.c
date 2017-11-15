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

#include "lr_builder.h"
/*
 * Private interface
 */

/*
 * utillib_lr_item
 */

static size_t lr_item_hash(void const *item)
{
  struct utillib_lr_item const *self=item;
  size_t hash=self->pos;
  uintptr_t rule_addr=self->rule;
  return rule_addr ^ hash;
}

static bool lr_item_equal(void const *lhs, void const *rhs)
{
  struct utillib_lr_item const * lhs_item=lhs;
  struct utillib_lr_item const * rhs_item=rhs;
  return lhs_item->pos == rhs_item->pos && lhs_item->rule == rhs_item->rule;
}

static const struct utillib_unordered_op 
lr_item_unordered_op={
  .hash=lr_item_hash,
  .equal=lr_item_equal,
};

static void lr_item_init(struct utillib_lr_item *self,
    struct utillib_rule const *rule)
{
  self->pos=0;
  self->rule=rule;
}

/*
 * utillib_lr_itemset
 */

static size_t lr_itemset_hash(void const * itemset)
{
  struct utillib_lr_itemset const *self=itemset;
  size_t hash=0;
  UTILLIB_VECTOR_FOREACH(struct utillib_lr_item const*, item, &self->itemset) {
    uintptr_t item_addr=item;
    hash+=item_addr;
  }
  return hash;
}

/*
 * Public interface
 */


void utillib_lr_item_init(struct utillib_lr_item *self, struct utillib_rule const * rule)
{
  self->rule_index=rule_index;


}

void utillib_lr_builder_init(struct utillib_lr_builder *self, struct utillib_rule_index * rule_index);
void utillib_lr_builder_build_table(struct utillib_lr_builder *self);
int utillib_lr_builder_check(struct utillib_lr_builder *self);
void utillib_lr_builder_destroy(struct utillib_lr_builder *self);

