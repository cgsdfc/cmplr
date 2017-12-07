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
#include "slist.h"
#include <assert.h>
#include <stdlib.h>

#define slist_check_front(self)                                                \
  do {                                                                         \
    assert((self)->head && "Slist should not be empty");                       \
  } while (0)

void utillib_slist_init(struct utillib_slist *self) { self->head = NULL; }

void utillib_slist_destroy(struct utillib_slist *self) {
  struct utillib_slist_node *node = self->head;
  while (node) {
    struct utillib_slist_node *next = node->next;
    free(node);
    node = next;
  }
}

void utillib_slist_destroy_owning(struct utillib_slist *self,
                                  void (*destroy)(void *elem)) {
  struct utillib_slist_node *node = self->head;
  while (node) {
    struct utillib_slist_node *next = node->next;
    destroy(node->value);
    free(node);
    node = next;
  }
}

bool utillib_slist_empty(struct utillib_slist const *self) {
  return NULL == self->head;
}

size_t utillib_slist_size(struct utillib_slist const *self) {
  size_t size = 0;
  struct utillib_slist_node *node = self->head;
  while (node) {
    struct utillib_slist_node *next = node->next;
    ++size;
    node = next;
  }
  return size;
}

void *utillib_slist_front(struct utillib_slist const *self) {
  slist_check_front(self);
  return self->head->value;
}

void utillib_slist_push_front(struct utillib_slist *self, void const *value) {
  struct utillib_slist **phead = &self->head;
  struct utillib_slist_node *new_node = malloc(sizeof *new_node);
  new_node->value = value;
  new_node->next = *phead;
  *phead = new_node;
}

void *utillib_slist_erase(struct utillib_slist *self, size_t pos) {
  void *old_value;
  struct utillib_slist_node *old_node;
  struct utillib_slist_node *node;

  if (!self->head)
    return NULL;
  if (pos == 0) {
    old_node = self->head;
    old_value = old_node->value;
    self->head = self->head->next;
    free(old_node);
    return old_value;
  }

  node = self->head;
  for (int i = 0; i < pos - 1; ++i) {
    node = node->next;
    if (!node->next)
      return NULL;
  }
  old_node = node->next;
  old_value = old_node->value;
  node->next = old_node->next;
  free(old_node);
  return old_value;
}

void utillib_slist_pop_front(struct utillib_slist *self) {
  slist_check_front(self);
  struct utillib_slist_node *old_node = self->head;
  self->head = self->head->next;
  free(old_node);
}

void utillib_slist_clear(struct utillib_slist *self) {
  utillib_slist_destroy(self);
  self->head = NULL;
}

struct utillib_json_value *
utillib_slist_json_array_create(struct utillib_slist const *self,
                                utillib_json_value_create_func_t create_func) {
  struct utillib_json_value *array = utillib_json_array_create_empty();
  UTILLIB_SLIST_FOREACH(void const *, elem, self) {
    utillib_json_array_push_back(array, create_func(elem));
  }
  return array;
}
