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
#include "list.h"
#include <assert.h>
#include <stdlib.h>

void utillib_list_init(struct utillib_list *self)
{
  self->head=NULL;
  self->tail=NULL;
}

void utillib_list_destroy(struct utillib_list *self)
{
  while (self->head) {
    struct utillib_list_node * old_node=self->head;
    self->head=old_node->next;
    free(old_node);
  }
}

void * utillib_list_front(struct utillib_list *self)
{
  assert (self->head && "should not be empty");
  return (void*) self->head->value;
}

void * utillib_list_back(struct utillib_list *self)
{
  assert (self->tail && "should not be empty");
  return (void*) self->tail->value;
}

size_t utillib_list_size(struct utillib_list *self)
{
  size_t size=0;
  for (struct utillib_list_node  *node=self->head;
      node != NULL ; node=node->next)
    ++size;
  return size;
}

bool utillib_list_empty(struct utillib_list *self)
{
  return self->head == NULL;
}

void utillib_list_push_front(struct utillib_list *self, void  const *value)
{
  struct utillib_list_node * new_node;
  new_node=malloc(sizeof *new_node);
  new_node->value=value;
  new_node->next=self->head;
  new_node->prev=NULL;
  self->head->prev=new_node;
  self->head=new_node;
  if (!self->tail)
    self->tail=new_node;
}

void utillib_list_push_back(struct utillib_list *self, void  const *value)
{
  struct utillib_list_node * new_node;
  new_node=malloc(sizeof *new_node);
  new_node->value=value;
  new_node->next=NULL;
  new_node->prev=self->tail;
  self->tail->next=new_node;
  self->tail=new_node;
  if (!self->head)
    self->head=new_node;
}

void utillib_list_pop_front(struct utillib_list *self)
{
  assert (self->head && "should not be empty");
  struct utillib_list_node * old_node;
  old_node=self->head;
  self->head=old_node->next;
  self->head->prev=NULL;
  free(old_node);
}

void utillib_list_pop_back(struct utillib_list *self)
{
  assert (self->tail && "should not be empty");
  struct utillib_list_node * old_node;
  old_node=self->tail;
  self->tail=old_node->prev;
  self->tail->next=NULL;
  free(old_node);
}








