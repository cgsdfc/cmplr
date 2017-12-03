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
#include <utillib/slist.h>

/**
 * \struct cling_symbol_table
 * This structure represents symbols in scopes.
 * A scope is just a hashmap holding some entries of symbols.
 * An entry of symbol is a name in the hashmap of the scope
 * and its kind with its value.
 * A series of scopes linked in a list form the lexical scopes
 * for the language, which can be pushed as the parser enters
 * a new scope and popped as it leaves a scope.
 * These hashmaps own the cling_symbol_entry and the name as key.
 */

struct cling_symbol_table {
  struct utillib_hashmap global_table;
  struct utillib_slist scope_table;
  size_t scope;
};

struct cling_symbol_entry {
  int kind;
  struct utillib_json_value *value;
};

/**
 * \enum cling_symbol_entry_kind
 * This enum descripts the properties of an entry
 * in the symbol table. These fields can be OR together
 * to express complex type from basic ones. For example,
 * const int => CL_INT | CL_CONST
 * int foo() => CL_INT | CL_FUNC
 * int a[10] => CL_INT | CL_ARRAY
 * However, some combinations are illegal. For example,
 * CL_INT | CL_ARRAY | CL_FUNC
 *
 */
UTILLIB_ENUM_BEGIN(cling_symbol_entry_kind)
UTILLIB_ENUM_ELEM_INIT(CL_UNDEF, 0)
UTILLIB_ENUM_ELEM_INIT(CL_INT, 1)
UTILLIB_ENUM_ELEM_INIT(CL_CHAR, 2)
UTILLIB_ENUM_ELEM_INIT(CL_CONST, 4)
UTILLIB_ENUM_ELEM_INIT(CL_ARRAY, 8)
UTILLIB_ENUM_ELEM_INIT(CL_FUNC, 16)
UTILLIB_ENUM_END(cling_symbol_entry_kind);

void cling_symbol_table_init(struct cling_symbol_table *self);
void cling_symbol_table_destroy(struct cling_symbol_table *self);

/**
 * \function cling_symbol_table_enter_scope
 * Makes an new scope and increases the scope counter.
 * This means the following insertions will be done in the new scope.
 */
void cling_symbol_table_enter_scope(struct cling_symbol_table *self);

/**
 * \function cling_symbol_table_leave_scope
 * Disposes the current scope if it is not the global scope.
 * All the symbols of it are lost.
 */
void cling_symbol_table_leave_scope(struct cling_symbol_table *self);

/**
 * \function cling_symbol_table_insert
 * Inserts a new entry (name, kind, value) into the current scope
 * and fails if the entry already exists.
 */
int cling_symbol_table_insert(struct cling_symbol_table *self, 
                              char const *name,
                              int kind,
                              struct utillib_json_value *value);

/**
 * \function cling_symbol_table_find
 * Finds the entry for symbol `name' either in the current scope
 * or lexical scopes.
 * \param level If it is 0, only searches the current scope.
 * Otherwise(non-zero), searches from the current scope all the way to
 * the global scope.
 * \return If the symbol is not found, NULL, or else the entry
 * of it.
 */
struct cling_symbol_entry *
cling_symbol_table_find(struct cling_symbol_table const *self, char const *name,
                        size_t level);

/**
 * \function cling_symbol_table_reserve
 * Reserves an entry in the current scope
 * with the `name' as key so that the following
 * lookup shows thats `name' exists.
 * Assumes `name' does not exist before.
 * Notes the `name' will be strdup.
 */
void cling_symbol_table_reserve(struct cling_symbol_table *self,
                                char const *name);

/**
 * \function cling_symbol_table_update
 * Parallel with `utillib_hashmap_insert' but
 * assumes the entry of `name' has been reserved with
 * `cling_symbol_table_reserve' and fills in the `NULL'
 * blank of it.
 * Notes that the name will not be strdup.
 */
void cling_symbol_table_update(struct cling_symbol_table *self,
                               char const *name,
                               int kind,
                               struct utillib_json_value *value);

/**
 * \function cling_symbol_table_exist_name
 * Parallel with `cling_symbol_table_find', but only answers
 * whether the name exists regardless of its entry.
 */
bool cling_symbol_table_exist_name(struct cling_symbol_table const *self,
                                   char const *name, size_t level);

/*
 * JSON
 */
struct utillib_json_value *
cling_symbol_table_json_object_create(struct cling_symbol_table const *self);

#endif /* CLING_SYMBOL_TABLE_H */
