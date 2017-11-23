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

#ifndef UTILLIB_SLIST_H
#define UTILLIB_SLIST_H

#include "config.h" /* for bool */
#include <stddef.h> /* for size_t */

#define UTILLIB_SLIST_FOREACH(T, X, L)                                         \
  T X;                                                                         \
  for (struct utillib_slist_node *_p = (L)->head;                           \
       _p != NULL && (((X) = _p->value) || 1); _p = _p->next)


struct utillib_slist_node {
  struct utillib_slist_node *next;
  void *value;
};


struct utillib_slist {
  struct utillib_slist_node *head;
};

void utillib_slist_init(struct utillib_slist *self);
void utillib_slist_destroy(struct utillib_slist *self);
void utillib_slist_destroy_owning(struct utillib_slist *self,
                                  void (*destroy)(void *elem));

bool utillib_slist_empty(struct utillib_slist const *self);
/**
 * \function utillib_slist_size
 * Counts the elements linearly to size.
 */
size_t utillib_slist_size(struct utillib_slist const *self);
void *utillib_slist_front(struct utillib_slist const *self);

void utillib_slist_push_front(struct utillib_slist *self, void const *value);

/**
 * \function utillib_slist_erase
 * Removes the node in the `pos' of the list 
 * and returns the value held by this node.
 * If there is no `pos' in the list, `NULL'
 * is returned.
 */
void * utillib_slist_erase(struct utillib_slist *self, size_t pos);
void utillib_slist_pop_front(struct utillib_slist *self);
void utillib_slist_clear(struct utillib_slist *self);

#endif /* UTILLIB_SLIST_H */
