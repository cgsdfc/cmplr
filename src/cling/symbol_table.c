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

#include "symbol_table.h"
#include <utillib/strhash.h>

#include <stdlib.h>
#include <assert.h>
#include <string.h>

static const struct utillib_hashmap_callback symbol_hash_callback={
  .hash_handler=mysql_strhash,
  .compare_handler=strcmp,
};

static void symbol_table_scope_destroy(struct utillib_hashmap *self)
{
  utillib_hashmap_destroy_owning(self, NULL, free);
  free(self);
}

void cling_symbol_table_init(struct cling_symbol_table *self)
{
  self->scope=0;
  utillib_hashmap_init(&self->global_table ,
      &symbol_hash_callback);
  utillib_slist_init(&self->scope_table);
}

void cling_symbol_table_destroy(struct cling_symbol_table *self)
{
  utillib_hashmap_destroy(&self->global_table);
  utillib_slist_destroy_owning(&self->scope_table, symbol_table_scope_destroy);

}

void cling_symbol_table_enter_scope(struct cling_symbol_table *self)
{
  struct utillib_hashmap * new_scope=malloc(sizeof *new_scope);
  utillib_hashmap_init(new_scope, &symbol_hash_callback);
  utillib_slist_push_front(&self->scope_table, new_scope);
  ++self->scope;
}

void cling_symbol_table_exit_scope(struct cling_symbol_table *self)
{
  assert (self->scope > 0);
  struct utillib_hashmap * old_scope=utillib_slist_front(&self->scope_table);
  utillib_slist_pop_front(&self->scope_table);
  utillib_hashmap_destroy(old_scope);
  free(old_scope);
  --self->scope;
}

static struct utillib_hashmap *
symbol_table_get_scope(struct cling_symbol_table *self)
{
  if (self->scope == 0)
    return &self->global_table;
  return utillib_slist_front(&self->scope_table);
}

int cling_symbol_table_insert(struct cling_symbol_table *self, 
    int kind, char const *name,
    struct utillib_json_value * value)
{
  struct utillib_hashmap * scope=symbol_table_get_scope(self);
  if (utillib_hashmap_at(scope, name))
    return 1;
  struct cling_symbol_entry * new_entry=malloc(sizeof *new_entry);
  new_entry->kind=kind;
  new_entry->value=value;
  utillib_hashmap_insert(scope, name, new_entry);
  return 0;
}

struct utillib_json_value *
cling_symbol_table_at(struct cling_symbol_table *self, char const * name,
    int * kind)
{
  struct utillib_hashmap * scope = symbol_table_get_scope(self);
  struct cling_symbol_entry * entry=utillib_hashmap_at(scope, name);
  if (!entry)
    return NULL;
  * kind=entry->kind;
  return entry->value;
}



