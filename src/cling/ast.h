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
#ifndef CLING_AST_H
#define CLING_AST_H
#include "symbol_table.h"
#include <utillib/json.h>

/*
 * Every operation backed up by
 * AST is held here including
 * some symbol_table lookup and
 * insertion. The reason is that
 * I want to keep the symbol_table
 * language-neutual.
 */

union cling_semantic {
  char signed_char;
  char const *string;
  long signed_int;
  size_t unsigned_int;
};

/*
 * Table insertion.
 */

void cling_ast_insert_const(struct utillib_json_value *self,
                            struct cling_symbol_table *symbols, int scope_kind);

void cling_ast_insert_variable(struct utillib_json_value *self,
                               struct cling_symbol_table *symbols,
                               int scope_kind);

void cling_ast_insert_arglist(struct utillib_json_value *self,
                              struct cling_symbol_table *symbols);

void cling_ast_insert_function(struct utillib_json_value *self,
                               struct cling_symbol_table *symbol_table);

#endif /* CLING_AST_H */
