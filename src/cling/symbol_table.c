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
#include "misc.h"
#include <utillib/json_foreach.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

UTILLIB_ETAB_BEGIN(cling_type)
UTILLIB_ETAB_ELEM_INIT(CL_UNDEF, "undefined type")
UTILLIB_ETAB_ELEM_INIT(CL_INT, "int")
UTILLIB_ETAB_ELEM_INIT(CL_CHAR, "char")
UTILLIB_ETAB_ELEM_INIT(CL_VOID, "void")
UTILLIB_ETAB_ELEM_INIT(CL_CONST, "const")
UTILLIB_ETAB_ELEM_INIT(CL_ARRAY, "array")
UTILLIB_ETAB_ELEM_INIT(CL_FUNC, "function")
UTILLIB_ETAB_END(cling_type);

static void symbol_entry_destroy(struct cling_symbol_entry *self);

static void symbol_table_scope_destroy(struct utillib_hashmap *self) {
  utillib_hashmap_destroy_owning(self, free, symbol_entry_destroy);
}

void cling_symbol_table_init(struct cling_symbol_table *self) {
  self->scope = 0;
  utillib_hashmap_init(&self->global, &cling_string_hash);
}

void cling_symbol_table_destroy(struct cling_symbol_table *self) {
  symbol_table_scope_destroy(&self->global);
}

void cling_symbol_table_enter_scope(struct cling_symbol_table *self) {
  utillib_hashmap_init(&self->local, &cling_string_hash);
  ++self->scope;
}

void cling_symbol_table_leave_scope(struct cling_symbol_table *self) {
  if (self->scope == 0)
    return;
  symbol_table_scope_destroy(&self->local);
  --self->scope;
}

static struct cling_symbol_entry *
symbol_table_lexical_find(struct cling_symbol_table const *self,
                          char const *name) {
  struct cling_symbol_entry *entry;
  if ((entry = utillib_hashmap_at(&self->local, name)))
    return entry;
  return utillib_hashmap_at(&self->global, name);
}

static bool
symbol_table_lexical_exist_name(struct cling_symbol_table const *self,
                                char const *name) {
  if (utillib_hashmap_exist_key(&self->local, name))
    return true;
  return utillib_hashmap_exist_key(&self->global, name);
}

void cling_symbol_table_reserve(struct cling_symbol_table *self,
                                char const *name, int scope_kind) {
  struct utillib_hashmap *scope;

  switch (scope_kind) {
  case CL_LOCAL:
    scope = &self->local;
    break;
  case CL_GLOBAL:
    scope = &self->global;
    break;
  default:
    assert(false);
  }
  utillib_hashmap_insert(scope, strdup(name), NULL);
}

struct cling_symbol_entry *
cling_symbol_table_find(struct cling_symbol_table const *self, char const *name,
                        int scope_kind) {
  if (self->scope == 0 || scope_kind == CL_GLOBAL)
    return utillib_hashmap_at(&self->global, name);
  switch (scope_kind) {
  case CL_LEXICAL:
    return symbol_table_lexical_find(self, name);
  case CL_LOCAL:
    return utillib_hashmap_at(&self->local, name);
  }
}

bool cling_symbol_table_exist_name(struct cling_symbol_table const *self,
                                   char const *name, int scope_kind) {
  if (self->scope == 0 || scope_kind == CL_GLOBAL)
    return utillib_hashmap_exist_key(&self->global, name);
  switch (scope_kind) {
  case CL_LEXICAL:
    return symbol_table_lexical_exist_name(self, name);
  case CL_LOCAL:
    return utillib_hashmap_exist_key(&self->local, name);
  }
}

static void symbol_entry_init(struct cling_symbol_entry *self, int scope,
                              int kind) {
  self->kind = kind;
  self->scope = scope;
}

static void symbol_entry_init_single_var(struct cling_symbol_entry *self,
                                         int scope, int symbol) {
  symbol_entry_init(self, scope, cling_symbol_to_type(symbol));
}

static void symbol_entry_init_const(struct cling_symbol_entry *self, int scope,
                                    int symbol, char const *val) {
  symbol_entry_init(self, scope, CL_CONST);
  self->constant.type = cling_symbol_to_type(symbol);
  self->constant.value = cling_symbol_to_immediate(symbol, val);
}

static void symbol_entry_init_array(struct cling_symbol_entry *self, int scope,
                                    int base_symbol, char const *extend) {
  symbol_entry_init(self, scope, CL_ARRAY);
  self->array.base_type = cling_symbol_to_type(base_symbol);
  sscanf(extend, "%lu", &self->array.extend);
}

static void
symbol_entry_init_function(struct cling_symbol_entry *self, int return_symbol,
                           struct utillib_json_value const *arglist) {
  int *argv_types;
  struct utillib_json_value const *object, *type;
  self->function.argc = utillib_json_array_size(arglist);
  int i = 0;

  symbol_entry_init(self, CL_GLOBAL, CL_FUNC);
  self->function.return_type = cling_symbol_to_type(return_symbol);
  argv_types = malloc(self->function.argc * sizeof argv_types[0]);

  UTILLIB_JSON_ARRAY_FOREACH(object, arglist) {
    type = utillib_json_object_at(object, "type");
    argv_types[i++] = cling_symbol_to_type(type->as_size_t);
  }
  self->function.argv_types = argv_types;
}

static void symbol_entry_destroy(struct cling_symbol_entry *self) {
  if (!self)
    /*
     * Since cling_symbol_table_reserve open the window
     * for NULL entry, we must test it here.
     */
    return;
  switch (self->kind) {
  case CL_FUNC:
    free(self->function.argv_types);
    break;
  }
  free(self);
}

static struct utillib_hashmap *current_scope(struct cling_symbol_table *self) {
  return self->scope ? &self->local : &self->global;
}

static void symbol_table_insert(struct utillib_hashmap *scope, char const *name,
                                struct cling_symbol_entry const *entry) {
  if (!utillib_hashmap_exist_key(scope, name))
    name = strdup(name);
  utillib_hashmap_update(scope, name, entry);
}

/*
 * Table insersion.
 */

void cling_symbol_table_insert_const(
    struct cling_symbol_table *self,
    struct utillib_json_value const *const_decl) {
  struct cling_symbol_entry *entry;
  struct utillib_json_value const *const_defs, *object, *type, *name, *value;
  type = utillib_json_object_at(const_decl, "type");
  const_defs = utillib_json_object_at(const_decl, "const_defs");

  UTILLIB_JSON_ARRAY_FOREACH(object, const_defs) {
    name = utillib_json_object_at(object, "name");
    value = utillib_json_object_at(object, "value");
    entry = malloc(sizeof *entry);
    symbol_entry_init_const(entry, self->scope, type->as_size_t, value->as_ptr);
    symbol_table_insert(current_scope(self), name->as_ptr, entry);
  }
}

void cling_symbol_table_insert_arglist(
    struct cling_symbol_table *self, struct utillib_json_value const *arglist) {
  struct utillib_json_value const *object, *type, *name;
  struct cling_symbol_entry *entry;

  UTILLIB_JSON_ARRAY_FOREACH(object, arglist) {
    type = utillib_json_object_at(object, "type");
    name = utillib_json_object_at(object, "name");
    entry = malloc(sizeof *entry);
    symbol_entry_init_single_var(entry, self->scope, type->as_size_t);
    symbol_table_insert(&self->local, name->as_ptr, entry);
  }
}

void cling_symbol_table_insert_function(
    struct cling_symbol_table *self,
    struct utillib_json_value const *function) {
  struct utillib_json_value const *type, *arglist, *name;
  struct cling_symbol_entry *entry;
  name = utillib_json_object_at(function, "name");
  type = utillib_json_object_at(function, "type");
  arglist = utillib_json_object_at(function, "arglist");
  entry = malloc(sizeof *entry);
  symbol_entry_init_function(entry, type->as_size_t, arglist);
  symbol_table_insert(&self->global, name->as_ptr, entry);
}

void cling_symbol_table_insert_variable(
    struct cling_symbol_table *self,
    struct utillib_json_value const *variable) {
  struct utillib_json_value const *name, *type, *var_defs, *extend, *object;
  struct cling_symbol_entry *entry;
  type = utillib_json_object_at(variable, "type");
  var_defs = utillib_json_object_at(variable, "var_defs");
  UTILLIB_JSON_ARRAY_FOREACH(object, var_defs) {
    entry = malloc(sizeof *entry);
    name = utillib_json_object_at(object, "name");
    extend = utillib_json_object_at(object, "extend");
    if (extend) {
      symbol_entry_init_array(entry, self->scope, type->as_size_t,
                              extend->as_ptr);
    } else { /* Single Variable */
      symbol_entry_init_single_var(entry, self->scope, type->as_size_t);
    }
    symbol_table_insert(current_scope(self), name->as_ptr, entry);
  }
}
