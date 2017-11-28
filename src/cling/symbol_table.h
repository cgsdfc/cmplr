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
#include <utillib/hashmap.h>
#include <utillib/slist.h>
#include <utillib/enum.h>

/**
 * \struct cling_symbol_table
 * This symbol table is intended to be 
 * light and share data with the AST.
 * Thus, it holds shared `utillib_json_value'.
 * Its purpose is assist the semantic checking
 * and codegen.
 * const_table holds the global constants (type and value).
 * var_table holds the global variables (type and value).
 * func_table holds functions (signature, return value and
 * normal arglist)
 * scope_table handles scopes formed by composite_stmt.
 * When a composite_stmt is seen, its local constants and
 * variables are first collected into the ast and then
 * entered the scope_table. When the scope leaves, these
 * entities are popped.
 * In case a name in the deeper scope shallows the outter
 * one, 
 * 
 */

struct cling_symbol_table {
  struct utillib_hashmap global_table;
  struct utillib_slist scope_table;
  size_t scope;
};

struct cling_symbol_entry {
  int kind;
  struct utillib_json_value * value;
};

/**
 * \enum cling_symbol_entry_kind
 * This enum descripts the properties of an entry
 * in the symbol table. These fields can be OR together
 * to express complex type from basic ones. For example,
 * const int => CL_INT | CL_CONST
 * int foo() => CL_INT | CL_FUNC
 * int a[10] => CL_INT | CL_ARRAY
 * However, some combinations are illegal.
 */
UTILLIB_ENUM_BEGIN(cling_symbol_entry_kind)
  UTILLIB_ENUM_ELEM_INIT(CL_INT, 1)
  UTILLIB_ENUM_ELEM_INIT(CL_CHAR, 2)
  UTILLIB_ENUM_ELEM_INIT(CL_CONST, 4)
  UTILLIB_ENUM_ELEM_INIT(CL_ARRAY, 8)
  UTILLIB_ENUM_ELEM_INIT(CL_FUNC, 16)
UTILLIB_ENUM_END(cling_symbol_entry_kind);

void cling_symbol_table_init(struct cling_symbol_table *self);
void cling_symbol_table_destroy(struct cling_symbol_table *self);

void cling_symbol_table_enter_scope(struct cling_symbol_table *self);
void cling_symbol_table_exit_scope(struct cling_symbol_table *self);
int cling_symbol_table_insert(struct cling_symbol_table *self, 
    int kind, char const *name,
    struct utillib_json_value * value);
struct utillib_json_value *
cling_symbol_table_at(struct cling_symbol_table *self, char const * name,
    int * kind);
struct utillib_json_value *
cling_symbol_table_json_object_create(struct cling_symbol_table *self);

#endif /* CLING_SYMBOL_TABLE_H */

