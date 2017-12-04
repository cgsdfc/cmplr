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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

UTILLIB_ETAB_BEGIN(cling_symbol_entry_kind)
UTILLIB_ETAB_ELEM_INIT(CL_UNDEF, "undefined type")
UTILLIB_ETAB_ELEM_INIT(CL_INT, "int")
UTILLIB_ETAB_ELEM_INIT(CL_CHAR, "char")
UTILLIB_ETAB_ELEM_INIT(CL_VOID, "void")
UTILLIB_ETAB_ELEM_INIT(CL_CONST, "const")
UTILLIB_ETAB_ELEM_INIT(CL_ARRAY, "array")
UTILLIB_ETAB_ELEM_INIT(CL_FUNC, "function")
UTILLIB_ETAB_END(cling_symbol_entry_kind);

static struct cling_symbol_entry *
symbol_entry_create(int kind, struct utillib_json_value *value) {
  struct cling_symbol_entry *new_entry = malloc(sizeof *new_entry);
  new_entry->kind = kind;
  new_entry->value = value;
  return new_entry;
}

static void symbol_entry_destroy(struct cling_symbol_entry *self) {
  utillib_json_value_destroy(self->value);
  free(self);
}

static const struct utillib_hashmap_callback symbol_hash_callback = {
    .hash_handler = mysql_strhash, .compare_handler = strcmp,
};

static struct utillib_hashmap * symbol_table_scope_create(void) {
  struct utillib_hashmap *new_scope = malloc(sizeof *new_scope);
  utillib_hashmap_init(new_scope, &symbol_hash_callback);
  return new_scope;
}

static void symbol_table_scope_destroy(struct utillib_hashmap *self) {
  utillib_hashmap_destroy_owning(self, free, symbol_entry_destroy);
  free(self);
}

void cling_symbol_table_init(struct cling_symbol_table *self) {
  self->scope = 0;
  utillib_hashmap_init(&self->global_table, &symbol_hash_callback);
  utillib_slist_init(&self->scope_table);
}

void cling_symbol_table_destroy(struct cling_symbol_table *self) {
  utillib_hashmap_destroy_owning(&self->global_table, free, symbol_entry_destroy);
  utillib_slist_destroy_owning(&self->scope_table, symbol_table_scope_destroy);
}

void cling_symbol_table_enter_scope(struct cling_symbol_table *self) {
  struct utillib_hashmap *new_scope=symbol_table_scope_create();
  utillib_slist_push_front(&self->scope_table, new_scope);
  ++self->scope;
}

void cling_symbol_table_leave_scope(struct cling_symbol_table *self) {
  assert(self->scope > 0);
  struct utillib_hashmap *old_scope = utillib_slist_front(&self->scope_table);
  utillib_slist_pop_front(&self->scope_table);
  symbol_table_scope_destroy(old_scope);
  --self->scope;
}

static struct cling_symbol_entry *
symbol_table_lexical_find(struct cling_symbol_table const *self, char const *name)
{
  UTILLIB_SLIST_FOREACH(struct utillib_hashmap const *, scope,
                        &self->scope_table) {
  struct cling_symbol_entry * entry;
  if ((entry=utillib_hashmap_at(scope, name)))
    return entry;
  }
  return utillib_hashmap_at(&self->global_table, name);
}

static bool symbol_table_lexical_exist_name(struct cling_symbol_table const *self, char const *name)
{
  UTILLIB_SLIST_FOREACH(struct utillib_hashmap const *, scope,
                        &self->scope_table) {
    if (utillib_hashmap_exist_key(scope, name))
      return true;
  }
  return utillib_hashmap_exist_key(&self->global_table, name);
}

void cling_symbol_table_update(struct cling_symbol_table *self, 
                              char const *name,int kind,
                              struct utillib_json_value *value,
                              int scope_kind) {
  struct utillib_hashmap *scope;
  void * should_be_NULL;

  switch(scope_kind) {
  case CL_GLOBAL:
    scope=&self->global_table;
    break;
  case CL_LOCAL:
    scope=utillib_slist_front(&self->scope_table);
    break;
  default:
    assert(false);
  }
  should_be_NULL=utillib_hashmap_update(scope,
      name, symbol_entry_create(kind, value));
  assert (should_be_NULL == NULL);
}

void cling_symbol_table_reserve(struct cling_symbol_table *self,
                                char const *name,
                                int scope_kind) {
  struct utillib_hashmap *scope;
  int should_be_zero;

  switch(scope_kind) {
  case CL_LOCAL:
    scope=utillib_slist_front(&self->scope_table);
    break;
  case CL_GLOBAL:
    scope=&self->global_table;
    break;
  default:
    assert(false);
  }
  should_be_zero=utillib_hashmap_insert(scope, strdup(name), NULL);
  assert(should_be_zero == 0);
}

struct cling_symbol_entry *
cling_symbol_table_find(struct cling_symbol_table const *self,
    char const *name, int scope_kind) {
  switch (scope_kind) {
  case CL_LEXICAL:
    return symbol_table_lexical_find(self, name);
  case CL_GLOBAL:
    return utillib_hashmap_at(&self->global_table, name);
  case CL_LOCAL:
    return utillib_hashmap_at(utillib_slist_front(&self->scope_table), name);
  }
}

bool cling_symbol_table_exist_name(struct cling_symbol_table const *self,
                                   char const *name, int scope_kind)
{
  switch(scope_kind) {
  case CL_LEXICAL:
    return symbol_table_lexical_exist_name(self, name);
  case CL_GLOBAL:
    return utillib_hashmap_exist_key(&self->global_table, name);
  case CL_LOCAL:
    return utillib_hashmap_exist_key(utillib_slist_front(&self->scope_table), name);
  }
}

/*
 * JSON interface
 */
static struct utillib_json_value *
symbol_entry_json_create(struct cling_symbol_entry *self) {
  return utillib_json_value_copy(self->value);
}

static struct utillib_json_value *
symbol_table_scope_json_object_create(struct utillib_hashmap const *self) {
  return utillib_hashmap_json_object_create(self, symbol_entry_json_create);
}

struct utillib_json_value *
cling_symbol_table_json_object_create(struct cling_symbol_table const *self) {
  struct utillib_json_value *object = utillib_json_object_create_empty();
  utillib_json_object_push_back(
      object, "global_table",
      symbol_table_scope_json_object_create(&self->global_table));
  struct utillib_json_value *array = utillib_slist_json_array_create(
      &self->scope_table, symbol_table_scope_json_object_create);
  utillib_json_object_push_back(object, "scope_table", array);
  return object;
}
