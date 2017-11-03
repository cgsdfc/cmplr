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
#include "error.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void destroy_node(utillib_slist_node * node)
{
	free(node);
}

static void destory_slist(utillib_slist_node * node)
{
	utillib_slist_node *x;
	while (node) {
		x = node;
		node = x->next;
		destroy_node(x);
	}
}

static void push_front_aux(utillib_slist_node ** tail,
			   utillib_slist_node * node)
{
	node->next = (*tail);
	*tail = node;
}

static void pop_front_aux(utillib_slist_node ** tail,
			  utillib_slist_node ** node)
{
	*node = *tail;
	*tail = (*tail)->next;
}

static utillib_slist_node *make_node(struct utillib_slist * self,
				     utillib_element_t data)
{
	utillib_slist_node *node;
	if (self->sl_free) {
		pop_front_aux(&(self->sl_free), &node);
		node->data = data;
		node->next = NULL;
		return node;
	}
	node = calloc(sizeof *node, 1);
	if (node) {
		node->data = data;
		return node;
	}
	utillib_die("ENOMEM in make_node");
}

void utillib_slist_push_front_node(struct utillib_slist * self,
				   utillib_slist_node * node)
{
	assert(node);
	push_front_aux(&(self->sl_tail), node);
}

/**
 * \function utillib_slist_init
 * Initializes an empty single list.
 */

void utillib_slist_init(struct utillib_slist * self)
{
	self->sl_tail = self->sl_free = NULL;
}

/**
 * \function utillib_slist_push_front
 * Pushes an element to its front in O(1) time.
 */

void utillib_slist_push_front(struct utillib_slist * self, utillib_element_t data)
{
	utillib_slist_node *to_push = make_node(self, data);
	push_front_aux(&(self->sl_tail), to_push);
}

/**
 * \function utillib_slist_erase
 * Deletes the element pointed to by `iter' in O(N) time.
 * \param iter The iterator over `self'.
 */
void utillib_slist_erase(struct utillib_slist * self,
			 struct utillib_slist_iterator * iter)
{
	utillib_slist_erase_node(self, iter->iter_node);
}

void utillib_slist_erase_node(struct utillib_slist * self,
			      utillib_slist_node * node)
{
	utillib_slist_node **prev = &(self->sl_tail);
	for (; *prev && (*prev)->next != node; *prev = (*prev)->next);
	*prev = node->next;
	push_front_aux(&(self->sl_free), *prev);
}

utillib_element_t utillib_slist_front(struct utillib_slist * self)
{
	assert(self->sl_tail);
	return self->sl_tail->data;
}

void utillib_slist_pop_front(struct utillib_slist * self)
{
	assert(self->sl_tail);
	utillib_slist_node *x;
	pop_front_aux(&(self->sl_tail), &x);
	push_front_aux(&(self->sl_free), x);
}

void utillib_slist_destroy(struct utillib_slist * self)
{
	destory_slist(self->sl_tail);
	destory_slist(self->sl_free);
}

bool utillib_slist_empty(struct utillib_slist * self)
{
	return self->sl_tail == NULL;
}

/**
 * \function utillib_slist_size
 * Counts the elements linearly.
 * \return number of elements.
 */

size_t utillib_slist_size(struct utillib_slist * self)
{
	size_t i = 0;
	for (utillib_slist_node * tail = self->sl_tail; tail != NULL;
	     tail = tail->next) {
		i++;
	}
	return i;
}

/**
 * \function utillib_slist_iterator_init_null
 * Initializes self pointing to no slist.
 */

void utillib_slist_iterator_init_null(struct utillib_slist_iterator * self)
{
	self->iter_node = NULL;
}

/**
 * \function utillib_slist_iterator_init
 * Initializes self over `cont'.
 */

void utillib_slist_iterator_init(struct utillib_slist_iterator * self,
				 struct utillib_slist * cont)
{
	self->iter_node = cont->sl_tail;
}

bool utillib_slist_iterator_has_next(struct utillib_slist_iterator * self)
{
	return NULL != self->iter_node;
}

void utillib_slist_iterator_next(struct utillib_slist_iterator * self)
{
	self->iter_node = UTILLIB_SLIST_NODE_NEXT(self->iter_node);
}

utillib_element_t utillib_slist_iterator_get(struct utillib_slist_iterator * self)
{
	return UTILLIB_SLIST_NODE_DATA(self->iter_node);
}
