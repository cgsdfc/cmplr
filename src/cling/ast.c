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
#include "symbols.h"
#include "rd_parser.h"
#include "symbol_table.h"
#include "error.h"

#include <utillib/json_foreach.h>

size_t cling_ast_get_type(struct utillib_json_value *self)
{
  struct utillib_json_value * type=utillib_json_object_at(self, "type");
  assert(type);
  return type->as_size_t;
}

char const * cling_ast_get_string(struct utillib_json_value *self, char const *key)
{
  struct utillib_json_value *name = utillib_json_object_at(self, key);
  assert(name);
  return name->as_ptr;
}

static size_t ast_gettype(struct utillib_json_value *self) {
  switch(self->as_size_t) {
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

static char const* ast_getname(struct utillib_json_value *self) {
  assert (self->kind == UT_JSON_STRING);
  return self->as_ptr;
}

void cling_ast_literal(struct utillib_json_value const*self,
    union cling_semantic *value)
{
  size_t type=cling_ast_get_type(self);
  char const *rawstr=cling_ast_get_string(self, "rawstr");
  switch (type) {
  case SYM_UINT:
    sscanf(rawstr, "%lu", &value->unsigned_int);
    return;
  case SYM_INTEGER:
    sscanf(rawstr, "%ld", &value->signed_int);
    return;
  case SYM_STRING:
    value->string=strdup(rawstr);
    return;
  case SYM_CHAR:
    sscanf(rawstr, "%c", &value->signed_char);
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

void cling_ast_insert_const(struct utillib_json_value *self,
                            struct cling_symbol_table *symbol_table,
                            int scope_kind) {
  struct utillib_json_value *type = utillib_json_object_at(self, "type");
  struct utillib_json_value *const_defs =
      utillib_json_object_at(self, "const_defs");
  int kind = CL_CONST | ast_gettype(type);

  UTILLIB_JSON_ARRAY_FOREACH(object, const_defs) {
    struct utillib_json_value *name =
        utillib_json_object_at(object, "name");
    struct utillib_json_value *new_object=utillib_json_value_copy(object);
    cling_symbol_table_update(symbol_table, ast_getname(name), kind, new_object, scope_kind);
  }
}

void cling_ast_insert_variable(struct utillib_json_value *self,
                               struct cling_symbol_table *symbol_table,
                               int scope_kind) {
  struct utillib_json_value *type = utillib_json_object_at(self, "type");
  struct utillib_json_value *var_defs =
      utillib_json_object_at(self, "var_defs");
  const int base_kind = ast_gettype(type);

  UTILLIB_JSON_ARRAY_FOREACH(object, var_defs) {
    int kind=base_kind;
    struct utillib_json_value *name = utillib_json_object_at(object, "name");
    if (utillib_json_object_at(object, "extend")) {
      kind  |= CL_ARRAY;
    }
    struct utillib_json_value * new_object=utillib_json_value_copy(object);
    cling_symbol_table_update(symbol_table, ast_getname(name), kind, new_object, scope_kind);
  }
}

/*
 * Inserts arglist into the current scope of the function.
 */
void cling_ast_insert_arglist(struct utillib_json_value *self,
    struct cling_symbol_table * symbol_table) {
  UTILLIB_JSON_ARRAY_FOREACH(object, self) {
    struct utillib_json_value * type=utillib_json_object_at(object, "type");
    int kind=ast_gettype(type);
    struct utillib_json_value * name=utillib_json_object_at(object, "name");
    struct utillib_json_value * new_object=utillib_json_value_copy(object);
    cling_symbol_table_update(symbol_table, ast_getname(name), kind, new_object, CL_LOCAL);
  }
}

/*
 * Extracts prototype of a function
 * and return a deep copy of it.
 */
static struct utillib_json_value *
ast_extract_prototype(struct utillib_json_value *self, 
    struct utillib_json_value const *type,
    struct utillib_json_value const *name)
{
  struct utillib_json_value * object=utillib_json_object_create_empty();
  struct utillib_json_value * arglist=utillib_json_object_at(self, "arglist");
  utillib_json_object_push_back(object, "type", utillib_json_value_copy(type));
  utillib_json_object_push_back(object, "name", utillib_json_value_copy(name));
  utillib_json_object_push_back(object, "arglist", utillib_json_value_copy(arglist));
  return object;
}

void cling_ast_insert_function(struct utillib_json_value *self,
    struct cling_symbol_table * symbol_table) {
  int kind=CL_FUNC;
  struct utillib_json_value * type=utillib_json_object_at(self, "type");
  struct utillib_json_value * name=utillib_json_object_at(self, "name");
  struct utillib_json_value * new_object=ast_extract_prototype(self, type, name);
  kind |= ast_gettype(type);
  cling_symbol_table_update(symbol_table, ast_getname(name), kind, new_object, CL_GLOBAL);
}

void cling_ast_set_op(struct utillib_json_value *self, size_t op) {
  utillib_json_object_push_back(self, "op", utillib_json_size_t_create(&op));
}

void cling_ast_set_opstr(struct utillib_json_value *self, size_t op) {
  char const * opstr=cling_symbol_kind_tostring(op);
  utillib_json_object_push_back(self, "opstr", utillib_json_string_create(&opstr));
}

void cling_ast_set_lhs(struct utillib_json_value *self,
                       struct utillib_json_value *lhs) {
  utillib_json_object_push_back(self, "lhs", lhs);
}

void cling_ast_set_rhs(struct utillib_json_value *self,
                       struct utillib_json_value *rhs) {
  utillib_json_object_push_back(self, "rhs", rhs);
}

void cling_ast_set_extend(struct utillib_json_value *self,
    struct utillib_json_value *extend) {
  utillib_json_object_push_back(self, "extend", extend);
}


struct utillib_json_value *cling_ast_constant(size_t code,
    char const *rawstr) {
  struct utillib_json_value * object=utillib_json_object_create_empty();
  cling_ast_set_type(object, code);
  utillib_json_object_push_back(object, "rawstr",
      utillib_json_string_create(&rawstr));
  return object;
}

struct utillib_json_value *cling_ast_statement(size_t type) {
  struct utillib_json_value *object = utillib_json_object_create_empty();
  utillib_json_object_push_back(object, "type",
                                utillib_json_size_t_create(&type));
  return object;
}

void cling_ast_set_name(struct utillib_json_value *self, char const *name) {
  utillib_json_object_push_back(self, "name",
                                utillib_json_string_create(&name));
}

struct utillib_json_value *
cling_ast_string(char const *rawstr) {
  struct utillib_json_value * strobj = utillib_json_object_create_empty();
  utillib_json_object_push_back(strobj, "is_str", &utillib_json_true);
  utillib_json_object_push_back(strobj, "value",
      utillib_json_string_create(&rawstr));
  return strobj;
}

void cling_ast_set_type(struct utillib_json_value *self, size_t type) {
  utillib_json_object_push_back(self, "type",
                                utillib_json_size_t_create(&type));
}

int cling_ast_check_assignable(char const *name, 
    struct cling_symbol_table const * symbol_table) {
  struct cling_symbol_entry * entry;
  int kind;

  entry=cling_symbol_table_find(symbol_table, name, CL_LEXICAL);
  if (!entry)
    return CL_EUNDEFINED;
  kind=entry->kind;
  if (kind & CL_FUNC || kind & CL_ARRAY || 
      kind & CL_CONST)
    return CL_ENOTLVALUE;
  return 0;
}
