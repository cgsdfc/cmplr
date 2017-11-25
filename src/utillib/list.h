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
#ifndef UTILLIB_LIST_H
#define UTILLIB_LIST_H
#include "config.h" /* for bool */
#include <stddef.h> /* for size_t */

struct utillib_list_node {
  struct utillib_list_node *prev;
  struct utillib_list_node *next;
  void const * value;
};

/**
 * \struct utillib_list
 * A doublely linked with fast insersion
 * and removal at the front and back.
 * It can be used as a queue.
 */

struct utillib_list {
  struct utillib_list_node * head;
  struct utillib_list_node * tail;
};

void utillib_list_init(struct utillib_list *self);
void utillib_list_destroy(struct utillib_list *self);

void * utillib_list_front(struct utillib_list *self);
void * utillib_list_back(struct utillib_list *self);
size_t utillib_list_size(struct utillib_list *self);
bool utillib_list_empty(struct utillib_list *self);

void utillib_list_push_front(struct utillib_list *self, void const *value);
void utillib_list_push_back(struct utillib_list *self, void const *value);
void utillib_list_pop_front(struct utillib_list *self);
void utillib_list_pop_back(struct utillib_list *self);


#endif /* UTILLIB_LIST_H */

