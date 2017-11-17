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

/**
 * \file utillib/slist.h
 * A single-linked forward list. Suitable for fast
 * insertion/remove at the front.
 */
#include "config.h" /* for bool */
#include <stddef.h> /* for size_t */
#define UTILLIB_SLIST_HAS_NEXT(N) ((N) != NULL)
#define UTILLIB_SLIST_NODE_DATA(N) ((N)->data)
#define UTILLIB_SLIST_NODE_NEXT(N) ((N)->next)
#define UTILLIB_SLIST_HEAD(L) ((L)->sl_tail)

/**
 * \macro UTILLIB_SLIST_FOREACH
 * Inline triversal for slist.
 */

#define UTILLIB_SLIST_FOREACH(T, X, L)                                         \
  T X;                                                                         \
  for (struct utillib_slist_node *_p = (L)->sl_tail;                           \
       _p != NULL && (((X) = _p->data) || 1); _p = _p->next)

/**
 * \struct utillib_slist_node
 * A node in the single list. Keeps the next node
 * pointer and `data' pointer.
 */

struct utillib_slist_node {
  struct utillib_slist_node *next;
  void const *data;
};

/**
 * \struct utillib_slist
 * Keeps the pointer to the node at the front.
 * Also maintains a free list for caching those
 * nodes that were removed previously so that next
 * insertion can use these nodes first.
 */

struct utillib_slist {
  struct utillib_slist_node *sl_tail;
  struct utillib_slist_node *sl_free;
};

/**
 * \struct utillib_slist_iterator
 * Aquires the front pointer from an slist
 * and follows it down until it becomes NULL.
 */

struct utillib_slist_iterator {
  struct utillib_slist_node *iter_node;
};

/** \brief utillib_slist_iterator */
void utillib_slist_iterator_init_null(struct utillib_slist_iterator *);
void utillib_slist_iterator_init(struct utillib_slist_iterator *,
                                 struct utillib_slist *);
bool utillib_slist_iterator_has_next(struct utillib_slist_iterator *);
void utillib_slist_iterator_next(struct utillib_slist_iterator *);
void *utillib_slist_iterator_get(struct utillib_slist_iterator *);

/** \brief constructor destructor */
void utillib_slist_init(struct utillib_slist *);
void utillib_slist_destroy(struct utillib_slist *);
void utillib_slist_destroy_owning(struct utillib_slist *,
                                  void (*destroy)(void *));

/** \brief observer */
bool utillib_slist_empty(struct utillib_slist *);
size_t utillib_slist_size(struct utillib_slist *);
void *utillib_slist_front(struct utillib_slist *);

/** \brief modifier */
void utillib_slist_push_front(struct utillib_slist *, void const *);
void utillib_slist_push_front_node(struct utillib_slist *,
                                   struct utillib_slist_node *);
void utillib_slist_erase_node(struct utillib_slist *,
                              struct utillib_slist_node *);
void utillib_slist_erase(struct utillib_slist *,
                         struct utillib_slist_iterator *);
void utillib_slist_pop_front(struct utillib_slist *);
#endif /* UTILLIB_SLIST_H */
