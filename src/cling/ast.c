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
#include "ast.h"
#include "symbol_table.h"
#include "symbols.h"

#include <utillib/json_foreach.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

size_t cling_ast_get_type(struct utillib_json_value const *self) {
  struct utillib_json_value *type = utillib_json_object_at(self, "type");
  assert(type);
  return type->as_size_t;
}

char const *cling_ast_get_string(struct utillib_json_value const *self,
                                 char const *key) {
  struct utillib_json_value *string = utillib_json_object_at(self, key);
  assert(string);
  return string->as_ptr;
}

static size_t ast_type2kind(size_t type) {
  switch (type) {
  case SYM_KW_INT:
    return CL_INT;
  case SYM_KW_CHAR:
    return CL_CHAR;
  case SYM_KW_VOID:
    return CL_VOID;
  default:
    assert(false);
  }
}

union cling_primary *cling_primary_copy(union cling_primary const *self) {
  union cling_primary *other = malloc(sizeof *other);
  memcpy(other, self, sizeof *other);
  return other;
}

inline size_t cling_primary_inthash(union cling_primary const *lhs) {
  return lhs->signed_int;
}

inline int cling_primary_intcmp(union cling_primary const *lhs,
                                union cling_primary const *rhs) {
  return lhs->signed_int - rhs->signed_int;
}

void cling_primary_toint(union cling_primary *self, size_t type) {
  switch (type) {
  case SYM_UINT:
    self->signed_int = (int)self->unsigned_int;
    break;
  case SYM_CHAR:
    self->signed_int = (char)self->signed_char;
    break;
  case SYM_INTEGER:
    break;
  }
}

void cling_primary_init(union cling_primary *self, size_t type,
                        char const *rawstr) {
  switch (type) {
  case SYM_UINT:
    sscanf(rawstr, "%u", &self->unsigned_int);
    return;
  case SYM_INTEGER:
    sscanf(rawstr, "%d", &self->signed_int);
    return;
  case SYM_STRING:
    self->string = rawstr;
    return;
  case SYM_CHAR:
    sscanf(rawstr, "%c", &self->signed_char);
    return;
  default:
    assert(false);
  }
}

/**
 * \function cling_ast_insert_const
 * Inserts a single_const_decl into the symbol_table.
 * Assumes self is not null.
 * Assumes all the symbol in `self' are not redefined.
 * Inserts into the current scope.
 */

void cling_ast_insert_const(struct utillib_json_value const *self,
                            struct cling_symbol_table *symbol_table,
                            int scope_kind) {
  size_t type = cling_ast_get_type(self);
  struct utillib_json_value *const_defs =
      utillib_json_object_at(self, "const_defs");
  int kind = CL_CONST | ast_type2kind(type);

  struct utillib_json_value *object;
  UTILLIB_JSON_ARRAY_FOREACH(object, const_defs) {
    char const *name = cling_ast_get_string(object, "name");
    struct utillib_json_value *new_object = utillib_json_value_copy(object);
    cling_symbol_table_update(symbol_table, name, kind, new_object, scope_kind);
  }
}

void cling_ast_insert_variable(struct utillib_json_value const *self,
                               struct cling_symbol_table *symbol_table,
                               int scope_kind) {
  size_t type = cling_ast_get_type(self);
  struct utillib_json_value *var_defs =
      utillib_json_object_at(self, "var_defs");
  const int base_kind = ast_type2kind(type);

  struct utillib_json_value *object;
  UTILLIB_JSON_ARRAY_FOREACH(object, var_defs) {
    int kind = base_kind;
    char const *name = cling_ast_get_string(object, "name");
    if (utillib_json_object_at(object, "extend")) {
      kind |= CL_ARRAY;
    }
    struct utillib_json_value *new_object = utillib_json_value_copy(object);
    cling_symbol_table_update(symbol_table, name, kind, new_object, scope_kind);
  }
}

/*
 * Inserts arglist into the local scope of the function.
 */
void cling_ast_insert_arglist(struct utillib_json_value const *self,
                              struct cling_symbol_table *symbol_table) {
  struct utillib_json_value *object;
  UTILLIB_JSON_ARRAY_FOREACH(object, self) {
    size_t type = cling_ast_get_type(object);
    int kind = ast_type2kind(type);
    char const *name = cling_ast_get_string(object, "name");
    struct utillib_json_value *new_object = utillib_json_value_copy(object);
    cling_symbol_table_update(symbol_table, name, kind, new_object, CL_LOCAL);
  }
}

/*
 * Extracts prototype of a function
 * and return a deep copy of it.
 */
static struct utillib_json_value *
ast_extract_prototype(struct utillib_json_value const *self,
                      struct utillib_json_value const *type,
                      struct utillib_json_value const *name) {
  struct utillib_json_value *object = utillib_json_object_create_empty();
  struct utillib_json_value *arglist = utillib_json_object_at(self, "arglist");
  size_t argc=utillib_json_array_size(arglist);
  utillib_json_object_push_back(object, "type", utillib_json_value_copy(type));
  utillib_json_object_push_back(object, "name", utillib_json_value_copy(name));
  utillib_json_object_push_back(object, "arglist",
                                utillib_json_value_copy(arglist));
  utillib_json_object_push_back(object, "argc", utillib_json_size_t_create(&argc));
  return object;
}

void cling_ast_insert_function(struct utillib_json_value const *self,
                               struct cling_symbol_table *symbol_table) {
  int kind = CL_FUNC;
  struct utillib_json_value *type = utillib_json_object_at(self, "type");
  struct utillib_json_value *name = utillib_json_object_at(self, "name");
  struct utillib_json_value *new_object =
      ast_extract_prototype(self, type, name);
  kind |= ast_type2kind(type->as_size_t);
  cling_symbol_table_update(symbol_table, name->as_ptr, kind, new_object,
                            CL_GLOBAL);
}
