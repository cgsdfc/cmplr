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

#ifndef CLING_SYMBOL_TABLE_H
#define CLING_SYMBOL_TABLE_H
#include <utillib/enum.h>
#include <utillib/hashmap.h>

struct cling_symbol_table {
  struct utillib_hashmap global;
  struct utillib_hashmap local;
  unsigned int scope;
};

struct cling_symbol_entry {
  int kind;
  int scope;
  union {
    struct {
      int base_type;
      size_t extend;
    } array;
    struct {
      unsigned int argc;
      int *argv_types;
      int return_type;
    } function;
    struct {
      int type;
      int value;
    } constant;
  };
};

/**
 * Determine the scope in which searches
 * or insertions are done.
 * Object like function should be inserted into global scope.
 * When searching for names, we use lexical scope.
 * When checking declaring a name, we declare it in the current scope.
 */

UTILLIB_ENUM_BEGIN(cling_symbol_table_scope_kind)
UTILLIB_ENUM_ELEM(CL_GLOBAL)
UTILLIB_ENUM_ELEM(CL_LOCAL)
UTILLIB_ENUM_ELEM(CL_LEXICAL)
UTILLIB_ENUM_END(cling_symbol_table_scope_kind);

UTILLIB_ENUM_BEGIN(cling_type)
UTILLIB_ENUM_ELEM(CL_UNDEF)
UTILLIB_ENUM_ELEM(CL_INT)
UTILLIB_ENUM_ELEM(CL_CHAR)
UTILLIB_ENUM_ELEM(CL_VOID)
UTILLIB_ENUM_ELEM(CL_CONST)
UTILLIB_ENUM_ELEM(CL_ARRAY)
UTILLIB_ENUM_ELEM(CL_FUNC)
UTILLIB_ENUM_END(cling_type);

void cling_symbol_table_init(struct cling_symbol_table *self);
void cling_symbol_table_destroy(struct cling_symbol_table *self);

/**
 * Init the local scope and increases the scope counter.
 * This means the following insertions will be done in the local scope.
 */
void cling_symbol_table_enter_scope(struct cling_symbol_table *self);

/**
 * Clear the local scope.
 * All the symbols of it are lost.
 */
void cling_symbol_table_leave_scope(struct cling_symbol_table *self);

/**
 * Finds the entry for symbol `name' in the scope
 * specified by `scope_kind'.
 * If the symbol is not found, NULL, or else the entry
 * of it.
 */
struct cling_symbol_entry *
cling_symbol_table_find(struct cling_symbol_table const *self, char const *name,
                        int scope_kind);

/**
 * Reserves an entry in scope specified by `scope_kind'
 * with the `name' as key so that the following
 * lookup shows thats `name' exists.
 * Notes the `name' will be strdup.
 * Notes `scope_kind' cannot be CL_LEXICAL.
 */
void cling_symbol_table_reserve(struct cling_symbol_table *self,
                                char const *name, int scope_kind);

/**
 * Parallel with `cling_symbol_table_find', but only answers
 * whether the name exists regardless of its entry.
 */
bool cling_symbol_table_exist_name(struct cling_symbol_table const *self,
                                   char const *name, int scope_kind);

void cling_symbol_table_insert_const(
    struct cling_symbol_table *self,
    struct utillib_json_value const *const_decl);

void cling_symbol_table_insert_arglist(
    struct cling_symbol_table *self, struct utillib_json_value const *arglist);

void cling_symbol_table_insert_function(
    struct cling_symbol_table *self, struct utillib_json_value const *function);

void cling_symbol_table_insert_variable(
    struct cling_symbol_table *self, struct utillib_json_value const *variable);

#endif /* CLING_SYMBOL_TABLE_H */
