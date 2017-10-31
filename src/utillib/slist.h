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
#include "typedef.h"
#include <stdbool.h>		// for bool
#include <stddef.h>		// for size_t
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
  for (utillib_slist_node *_p = (L)->sl_tail;                                  \
       _p != NULL && (((X) = _p->data) || 1); _p = _p->next)

/**
 * \struct utillib_slist_node
 * A node in the single list. Keeps the next node
 * pointer and `data' pointer.
 */

typedef struct utillib_slist_node {
	struct utillib_slist_node *next;
	utillib_element_t data;
} utillib_slist_node;

/**
 * \struct utillib_slist
 * Keeps the pointer to the node at the front.
 * Also maintains a free list for caching those
 * nodes that were removed previously so that next
 * insertion can use these nodes first.
 */

typedef struct utillib_slist {
	struct utillib_slist_node *sl_tail;
	struct utillib_slist_node *sl_free;
} utillib_slist;

/**
 * \struct utillib_slist_iterator
 * Aquires the front pointer from an slist
 * and follows it down until it becomes NULL.
 */

typedef struct utillib_slist_iterator {
	struct utillib_slist_node *iter_node;
} utillib_slist_iterator;

/** \brief utillib_slist_iterator */
void utillib_slist_iterator_init_null(utillib_slist_iterator *);
void utillib_slist_iterator_init(utillib_slist_iterator *,
				 utillib_slist *);
bool utillib_slist_iterator_has_next(utillib_slist_iterator *);
void utillib_slist_iterator_next(utillib_slist_iterator *);
utillib_element_t utillib_slist_iterator_get(utillib_slist_iterator *);

/** \brief constructor destructor */
void utillib_slist_init(utillib_slist *);
void utillib_slist_destroy(utillib_slist *);
void utillib_slist_destroy_owning(utillib_slist *,
				  utillib_destroy_func_t *);

/** \brief observer */
bool utillib_slist_empty(utillib_slist *);
size_t utillib_slist_size(utillib_slist *);
utillib_element_t utillib_slist_front(utillib_slist *);

/** \brief modifier */
void utillib_slist_push_front(utillib_slist *, utillib_element_t);
void utillib_slist_push_front_node(utillib_slist *, utillib_slist_node *);
void utillib_slist_erase_node(utillib_slist *, utillib_slist_node *);
void utillib_slist_erase(utillib_slist *, utillib_slist_iterator *);
void utillib_slist_pop_front(utillib_slist *);
#endif				// UTILLIB_SLIST_H
