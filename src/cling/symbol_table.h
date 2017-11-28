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
  struct utillib_hashmap const_table;
  struct utillib_hashmap var_table;
  struct utillib_hashmap func_table;
  struct utillib_hashmap scope_table;
};


void cling_symbol_table_init(struct cling_symbol_table *self);
void cling_symbol_table_destroy(struct cling_symbol_table *self);
#endif /* CLING_SYMBOL_TABLE_H */

