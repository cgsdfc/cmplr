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
#include <stdlib.h>
#include <assert.h>
#include <string.h>

static size_t 
lr_builder_max_items(struct utillib_rule_index const *rule_index)
{
  size_t size=0;
  UTILLIB_VECTOR_FOREACH(struct utillib_rule *, rule, &rule_index->rules) {
    size += utillib_vector_size(&rule->RHS) + 1;
  }
  return size + 1;
}

/**
 * \function utillib_lr_itemset_init
 * Initilizes a itemset with its accessing symbol.
 */
struct utillib_lr_itemset *
utillib_lr_itemset_create(struct utillib_symbol const *access,
    size_t max_items)
{
  struct utillib_lr_itemset *self=malloc(sizeof *self);
  self->access=access;
  utillib_bitset_init(&self->bitset, max_items);
  utillib_vector_init(&self->items);
  return self;
}

void utillib_lr_itemset_destroy(struct utillib_lr_itemset *self)
{
  utillib_bitset_destroy(&self->bitset);
  utillib_vector_destroy(&self->items);
  free(self);
}

void utillib_lr_item_init(struct utillib_lr_item *self, size_t pos,
    struct utillib_rule const *rule)
{
  self->pos=pos;
  self->rule=rule;
  if (pos == utillib_vector_size(&rule->RHS))
    self->prefixed=NULL;
  else
    self->prefixed=utillib_vector_at(&rule->RHS, pos);
}

static size_t 
lr_item_hash(struct utillib_lr_item const *self)
{
  return self->pos + self->rule->id;
}

static int
lr_item_compare(struct utillib_lr_item const *lhs,
    struct utillib_lr_item const *rhs)
{
  if (lhs->pos == rhs->pos && lhs->rule->id == rhs->rule->id)
    return 0;
  return 1;
}

static const struct utillib_hashmap_callback lr_item_callback= {
  .hash_handler=lr_item_hash,
  .compare_handler=lr_item_compare,
};

static size_t
lr_itemset_hash(struct utillib_lr_itemset const *self)
{
  return utillib_bitset_hash(&self->bitset);
}

static int 
lr_itemset_compare(struct utillib_lr_itemset const *lhs,
    struct utillib_lr_itemset const *rhs)
{
  if (utillib_bitset_equal(&lhs->bitset, &rhs->bitset))
    return 0;
  return 1;
}

static const struct utillib_hashmap_callback lr_itemset_callback = {
  .hash_handler=lr_itemset_hash,
  .compare_handler=lr_itemset_compare,
};

struct utillib_lr_item *
utillib_lr_builder_item_lookup(struct utillib_lr_builder *self,
    struct utillib_lr_item const * item)
{
  struct utillib_lr_item * exist=utillib_hashmap_at(&self->item_index, item);
  if (exist)
    return exist;
  exist=malloc(sizeof *exist);
  utillib_lr_item_init(exist, item->pos, item->rule);
  exist->value=self->items_count++;
  int inserted=utillib_hashmap_insert(&self->item_index, exist, exist);
  assert (inserted==0 && "should insert new item successfully");
}

static void lr_builder_closure_added_clear(struct utillib_lr_builder *self)
{
  memset(self->closure_added, 0, sizeof (bool) * self->non_terminals_size);
}

/**
 * \function utillib_lr_itemset_additem
 * Adds an item to itemset. 
 * \param item The item to add which should be unique.
 * \return Whether the item was added.
 */
void utillib_lr_itemset_additem(struct utillib_lr_itemset *self,
    struct utillib_lr_item const *item)
{
  bool updated=utillib_bitset_insert_updated(&self->bitset, item->value);
  assert (updated && "should always update the itemset");
  utillib_vector_push_back(&self->items, item);
}

/**
 * \function utillib_lr_itemset_closure
 * Does closure on this itemset by looping
 * over all the items and trying to bring new
 * guys into this itemset until it cannot accommodate
 * any more.
 */
void utillib_lr_itemset_closure(
    struct utillib_lr_builder *self,
    struct utillib_lr_itemset *itemset)
{
  struct utillib_rule_index * rule_index=self->rule_index;
  struct utillib_vector * items=&itemset->items;
  struct utillib_vector * LHS_index=rule_index->LHS_index;
  struct utillib_lr_item new_item;

  lr_builder_closure_added_clear(self);

  for (int i=0; i<utillib_vector_size(items); ++i) {
    struct utillib_lr_item * item=utillib_vector_at(items, i);
    struct utillib_symbol const * prefixed=item->prefixed;
    if (!prefixed || prefixed->kind == UT_SYMBOL_TERMINAL)
      continue;
    size_t prefixed_index=utillib_rule_index_symbol_index(rule_index, prefixed);
    assert (prefixed_index < self->non_terminals_size && "out of range");
    if (self->closure_added[prefixed_index])
      continue;
    self->closure_added[prefixed_index]=true;
    struct utillib_vector * prefixed_rules=&LHS_index[prefixed_index];
    UTILLIB_VECTOR_FOREACH(struct utillib_rule *, rule, prefixed_rules) {
      utillib_lr_item_init(&new_item, 0, rule);
      utillib_lr_itemset_additem(itemset,
          utillib_lr_builder_item_lookup(self, &new_item));
    }
  }
}

/**
 * \function utillib_lr_itemset_build_goto
 * From one itemset, tries to move the indicator one symbol leftwards
 * for each of its containing item and collects the itemsets with
 * the same symbol move past into one vector.
 * The result is a hashmap of other itemset that is reachable from
 * the original one.
 * Notes that these reachable itemsets were completed by closure.
 */

void lr_builder_build_goto(struct utillib_lr_builder * self, 
    struct utillib_lr_itemset * input_itemset)
{
  struct utillib_lr_item new_item;
  struct utillib_vector * gotosets=&self->gotosets;
  assert (utillib_vector_empty(gotosets) && "gotosets should be empty");

  UTILLIB_VECTOR_FOREACH(struct utillib_lr_item *, item, &input_itemset->items) 
  {
    if (!item->prefixed)
      continue;
    utillib_lr_item_init(&new_item, item->pos+1, item->rule);
    struct utillib_lr_itemset * maybe_exist=NULL;

    UTILLIB_VECTOR_FOREACH(struct utillib_lr_itemset*, itemset, gotosets) {
      if (itemset->access == item->prefixed) {
        maybe_exist=itemset;
        break;
      }
    }

    if (!maybe_exist) {
      struct utillib_lr_itemset * new_itemset=
      utillib_lr_itemset_create(item->prefixed, self->max_items);
      utillib_vector_push_back(gotosets, new_itemset);
      maybe_exist=new_itemset;
    }
    utillib_lr_itemset_additem(maybe_exist,
        utillib_lr_builder_item_lookup(self, &new_item));
  }
}

/**
 * \function lr_builder_core_bfs
 * Computes all the LR(0) itemsets, or the core
 * using breadth first search of the underlying
 * digraph.
 * First, it pushes the first itemset into a queue.
 * Then, it continuously polls the queue until it becomes
 * empty. For each front of the queue, it expands edges from
 * this itemset with possible symbols. For those reachable 
 * itemsets from the front one, they will be lookuped in
 * a hashmap before added to the back of the queue.
 * If it was seen, it will not be added.
 * The output will be a `struct utillib_edge_list' that
 * contains all the relationships among all these itemsets.
 */

static void lr_builder_core_bfs(struct utillib_lr_builder *self, 
    struct utillib_edge_list * core)
{
  struct utillib_list * queue=&self->queue;
  struct utillib_hashmap * itemset_index=&self->itemset_index;
  struct utillib_vector * gotosets=&self->gotosets;

  while (!utillib_list_empty(queue)) {
    struct utillib_lr_itemset * topset=utillib_list_front(queue);
    utillib_list_pop_front(queue);
    lr_builder_build_goto(self, topset);
    UTILLIB_VECTOR_FOREACH(struct utillib_lr_itemset*, reachable, gotosets) {
      struct utillib_lr_itemset * maybe_exist;
      maybe_exist=utillib_hashmap_at(itemset_index, reachable);
      if (maybe_exist) {
        utillib_edge_list_add_edge(core, 
            topset->value, maybe_exist->value, maybe_exist->access);
        utillib_lr_itemset_destroy(reachable);
        continue;
      }
      reachable->value=utillib_edge_list_add_node(core, reachable);
      utillib_edge_list_add_edge(core,
          topset->value, reachable->value, reachable->access);
      utillib_hashmap_insert(itemset_index, reachable, reachable);
      utillib_list_push_back(queue, reachable);
    }
    utillib_vector_clear(gotosets);
  }
}

void utillib_lr_builder_init(struct utillib_lr_builder *self,
    struct utillib_rule_index *rule_index)
{
  self->items_count=0;
  self->max_items=lr_builder_max_items(rule_index);
  self->rule_index=rule_index;
  self->non_terminals_size=rule_index->non_terminals_size;
  self->closure_added=calloc(sizeof (bool), self->non_terminals_size);
  utillib_list_init(&self->queue);
  utillib_vector_init(&self->gotosets);
  utillib_hashmap_init(&self->itemset_index, &lr_itemset_callback);
  utillib_hashmap_init(&self->item_index, &lr_item_callback);
  utillib_rule_index_build_LHS_index(rule_index);
}

void utillib_lr_builder_build_core(struct utillib_lr_builder *self,
    struct utillib_edge_list * core)
{
  utillib_edge_list_init(core);
  struct utillib_lr_itemset * first_itemset;
  first_itemset=utillib_lr_itemset_create(NULL, self->max_items);
  utillib_lr_itemset_closure(self, first_itemset);
  first_itemset->value=utillib_edge_list_add_node(core, first_itemset);
  utillib_hashmap_insert(&self->itemset_index, first_itemset, first_itemset);
  utillib_list_push_back(&self->queue, first_itemset);
  lr_builder_core_bfs(self, core);
}

void utillib_lr_builder_destroy(struct utillib_lr_builder *self)
{
  utillib_list_destroy(&self->queue);
  free(self->closure_added);
  utillib_hashmap_destroy_owning(&self->item_index, free, NULL);
  utillib_hashmap_destroy(&self->itemset_index);
  utillib_vector_destroy(&self->gotosets);
}

