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
#include "unordered_map.h"
#include "equal.h"
#include "error.h"
#include "hash.h"
#include "slist.h"
#include <assert.h>
#include <float.h>

UTILLIB_ENUM_BEGIN(find_mode_t)
UTILLIB_ENUM_ELEM(FIND_ONLY)
UTILLIB_ENUM_ELEM(FORCE_INSERT)
UTILLIB_ENUM_END(find_mode_t)

/**
 * \function do_hash
 * Do hash using fast modulo.
 */
static size_t do_hash(struct utillib_unordered_map *self, utillib_key_t key) {
  // use fast modulo i.e. x % power_of_2 == x & (power_of_2-1)
  return self->un_op->hash(key) & (self->un_nbucket - 1);
}

/**
   \function do_equal
   Calls the equal function.
*/
static bool do_equal(struct utillib_unordered_map *self, utillib_key_t lhs,
                     utillib_key_t rhs) {
  return self->un_op->equal(lhs, rhs);
}

/**
 * \function make_slist
 */
static struct utillib_slist *make_slist(void) {
  // create an empty slist on the heap
  struct utillib_slist *list = malloc(sizeof *list);
  if (list) {
    utillib_slist_init(list);
    return list;
  }
  utillib_die("NOMEM in make_slist");
}

static void push_free(struct utillib_unordered_map *self,
                      struct utillib_pair_t *pair) {
  struct utillib_pair_t **head = &self->un_free;
  UTILLIB_PAIR_SECOND(pair) = *head;
  *head = pair;
}

void utillib_unordered_map_clear(struct utillib_unordered_map *self) {
  UTILLIB_VECTOR_FOREACH(struct utillib_slist *, list, &self->un_bucket) {
    while (!utillib_slist_empty(list)) {
      struct utillib_pair_t *pair = utillib_slist_front(list);
      utillib_slist_pop_front(list);
      push_free(self, pair);
    }
  }
}

struct utillib_pair_t *pop_free(struct utillib_unordered_map *self) {
  struct utillib_pair_t *pair = self->un_free;
  self->un_free = UTILLIB_PAIR_SECOND(pair);
  return self->un_free;
}

static struct utillib_pair_t *make_pair(struct utillib_unordered_map *self,
                                        utillib_key_t key,
                                        utillib_value_t value) {
  struct utillib_pair_t *pair;
  if (self->un_free) {
    pair = pop_free(self);
    UTILLIB_PAIR_FIRST(pair) = key;
    UTILLIB_PAIR_SECOND(pair) = value;
    return pair;
  }
  return utillib_make_pair(key, value);
}

static void push_back_bucket(struct utillib_unordered_map *self,
                             size_t nbucket) {
  for (int i = 0; i < nbucket; ++i) {
    struct utillib_slist *l = make_slist();
    utillib_vector_push_back(&(self->un_bucket), l);
  }
}

static void rehash_impl(struct utillib_unordered_map *self, size_t nbucket) {
  utillib_vector holder;
  utillib_vector_init(&holder);
  utillib_vector_reserve(&holder, self->un_size);
  size_t cur_nb = utillib_vector_size(&(self->un_bucket));
  UTILLIB_VECTOR_FOREACH(struct utillib_slist *, l, &(self->un_bucket)) {
    while (!utillib_slist_empty(l)) {
      utillib_vector_push_back(&holder, utillib_slist_front(l));
      utillib_slist_pop_front(l);
    }
  }
  for (int i = cur_nb; i < nbucket; ++i) {
    utillib_vector_push_back(&(self->un_bucket), make_slist());
  }
  UTILLIB_VECTOR_FOREACH(struct utillib_pair_t *, pair, &holder) {
    size_t hashv = do_hash(self, UTILLIB_PAIR_FIRST(pair));
    struct utillib_slist *l = utillib_vector_at(&(self->un_bucket), hashv);
    utillib_slist_push_front(l, pair);
  }
  self->un_nbucket = nbucket;
  utillib_vector_destroy(&holder);
}

void utillib_unordered_map_init(struct utillib_unordered_map *self,
                                struct utillib_unordered_op *op) {
  static const size_t init_nbucket = 8; // use fast modulo
  static const double init_max_lf = 0.8;
  self->un_nbucket = init_nbucket;
  self->un_max_lf = init_max_lf;
  self->un_op = op;
  self->un_size = 0;
  self->un_free = NULL;
  utillib_vector_init(&(self->un_bucket));
  push_back_bucket(self, init_nbucket);
}

void utillib_unordered_map_init_from_array(struct utillib_unordered_map *self,
                                           struct utillib_unordered_op *op,
                                           struct utillib_pair_t const *p) {
  utillib_unordered_map_init(self, op);
  for (; UTILLIB_PAIR_FIRST(p) != NULL; ++p) {
    utillib_unordered_map_insert(self, p);
  }
}

static void destroy_free_owning(struct utillib_unordered_map *self,
                                utillib_destroy_func_t *destroy) {
  while (self->un_free) {
    struct utillib_pair_t *p = self->un_free;
    self->un_free = UTILLIB_PAIR_SECOND(p);
    destroy(p);
    free(p);
  }
}

static void destroy_free(struct utillib_unordered_map *self) {
  while (self->un_free) {
    struct utillib_pair_t *p = self->un_free;
    self->un_free = UTILLIB_PAIR_SECOND(p);
    free(p);
  }
}

static void destroy_bucket(struct utillib_unordered_map *self) {
  UTILLIB_VECTOR_FOREACH(struct utillib_slist *, list, &self->un_bucket) {
    utillib_slist_destroy(list);
    free(list);
  }
  utillib_vector_destroy(&self->un_bucket);
}

void utillib_unordered_map_destroy_owning(struct utillib_unordered_map *self,
                                          utillib_destroy_func_t *destroy) {
  utillib_unordered_map_clear(self);
  destroy_free_owning(self, destroy);
  destroy_bucket(self);
}

void utillib_unordered_map_destroy(struct utillib_unordered_map *self) {
  utillib_unordered_map_clear(self);
  destroy_free(self);
  destroy_bucket(self);
}

static struct utillib_pair_t *
linear_search_list(struct utillib_unordered_map *self,
                   struct utillib_slist *list, utillib_key_t key,
                   struct utillib_slist_node **pos) {
  struct utillib_slist_node *node = UTILLIB_SLIST_HEAD(list);
  for (; node != NULL; node = UTILLIB_SLIST_NODE_NEXT(node)) {
    struct utillib_pair_t const *pair = UTILLIB_SLIST_NODE_DATA(node);
    if (do_equal(self, key, UTILLIB_PAIR_FIRST(pair))) {
      if (pos) {
        *pos = node;
      }
      return (void *)pair;
    }
  }
  return NULL;
}

static int find_impl(struct utillib_unordered_map *self, utillib_key_t key,
                     utillib_value_t value, int mode,
                     struct utillib_slist **plist,
                     struct utillib_slist_node **pos,
                     struct utillib_pair_t **retv) {
  size_t hashv = do_hash(self, key);
  struct utillib_slist *list = utillib_vector_at(&(self->un_bucket), hashv);
  struct utillib_pair_t *pair = linear_search_list(self, list, key, pos);
  if (retv) {
    *retv = pair;
  }
  if (plist) {
    *plist = list;
  }
  switch (mode) {
  case FIND_ONLY:
    return pair == NULL ? KEY_FOUND : KEY_MISSING;
  case FORCE_INSERT:
    if (pair) {
      return KEY_EXISTS;
    }
    pair = make_pair(self, key, value);
    utillib_slist_push_front(list, pair);
    ++(self->un_size);
    return KEY_INSERTED;
  default:
    assert(false);
  }
}

static int insert_impl(struct utillib_unordered_map *self, utillib_key_t key,
                       utillib_value_t value) {
  if (self->un_max_lf - utillib_unordered_map_load_factor(self) < DBL_EPSILON) {
    rehash_impl(self, self->un_nbucket << 1); // use fast modulo
  }
  return find_impl(self, key, value, FORCE_INSERT, NULL, NULL, NULL);
}

int utillib_unordered_map_insert(struct utillib_unordered_map *self,
                                 struct utillib_pair_t const *pair) {
  return insert_impl(self, UTILLIB_PAIR_FIRST(pair), UTILLIB_PAIR_SECOND(pair));
}

int utillib_unordered_map_emplace(struct utillib_unordered_map *self,
                                  utillib_key_t key, utillib_value_t value) {
  return insert_impl(self, key, value);
}

struct utillib_pair_t *
utillib_unordered_map_find(struct utillib_unordered_map *self,
                           utillib_key_t key) {
  struct utillib_pair_t *p;
  find_impl(self, key, NULL, FIND_ONLY, NULL, NULL, &p);
  return p;
}

int utillib_unordered_map_erase(struct utillib_unordered_map *self,
                                utillib_key_t key) {
  struct utillib_slist *l;
  struct utillib_slist_node *n;
  switch (find_impl(self, key, NULL, FIND_ONLY, &l, &n, NULL)) {
  default:
    assert(false);
  case KEY_MISSING:
    return KEY_MISSING;
  case KEY_FOUND:
    utillib_slist_erase_node(l, n);
    --(self->un_size);
    return KEY_FOUND;
  }
}

size_t utillib_unordered_map_size(struct utillib_unordered_map *self) {
  return self->un_size;
}

bool utillib_unordered_map_empty(struct utillib_unordered_map *self) {
  return utillib_unordered_map_size(self) == 0;
}

double utillib_unordered_map_load_factor(struct utillib_unordered_map *self) {
  return (double)utillib_unordered_map_size(self) /
         utillib_unordered_map_bucket_count(self);
}

size_t utillib_unordered_map_bucket_count(struct utillib_unordered_map *self) {
  return self->un_nbucket;
}

size_t utillib_unordered_map_bucket_size(struct utillib_unordered_map *self,
                                         size_t n) {
  assert(n < self->un_nbucket);
  struct utillib_slist *l = utillib_vector_at(&(self->un_bucket), n);
  return utillib_slist_size(l);
}

double
utillib_unordered_map_max_load_factor(struct utillib_unordered_map *self) {
  return self->un_max_lf;
}

void utillib_unordered_map_set_max_load_factor(
    struct utillib_unordered_map *self, double max_lf) {
  self->un_max_lf = max_lf;
}

static void iter_skip_empty_slot(struct utillib_unordered_map_iterator *self) {
  for (; utillib_vector_iterator_has_next(&self->iter_slot);
       utillib_vector_iterator_next(&self->iter_slot)) {
    struct utillib_slist *list = utillib_vector_iterator_get(&self->iter_slot);
    if (!utillib_slist_empty(list)) {
      return;
    }
  }
}

void utillib_unordered_map_iterator_init(
    struct utillib_unordered_map_iterator *self,
    struct utillib_unordered_map *cont) {
  utillib_vector_iterator_init(&self->iter_slot, &cont->un_bucket);
  utillib_slist_iterator_init(&self->iter_node,
                              utillib_vector_front(&cont->un_bucket));
}

bool utillib_unordered_map_iterator_has_next(
    struct utillib_unordered_map_iterator *self) {
  iter_skip_empty_slot(self);
  return utillib_vector_iterator_has_next(&self->iter_slot);
}

void utillib_unordered_map_iterator_next(
    struct utillib_unordered_map_iterator *self) {
  if (utillib_slist_iterator_has_next(&self->iter_node)) {
    utillib_slist_iterator_next(&self->iter_node);
    return;
  }
  utillib_vector_iterator_next(&self->iter_slot);
  utillib_slist_iterator_init(&self->iter_node,
                              utillib_vector_iterator_get(&self->iter_slot));
}

struct utillib_pair_t *utillib_unordered_map_iterator_get(
    struct utillib_unordered_map_iterator *self) {
  return utillib_slist_iterator_get(&self->iter_node);
}
