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
 * This union is **untyped**
 * so all its usage is based
 * on context and it cannot hold
 * memory on heap.
 */
union cling_primary {
  char signed_char;
  char const *string;
  int signed_int;
  unsigned int unsigned_int;
};

/*
 * Makes a bitwise copy of self and put
 * it on the heap.
 */
union cling_primary *cling_primary_copy(union cling_primary const *self);

/*
 * Hashes the signed_int field.
 */
size_t cling_primary_inthash(union cling_primary const *lhs);

/*
 * Compares the signed_int field of 2 cling_primary.
 */
int cling_primary_intcmp(union cling_primary const *lhs,
                         union cling_primary const *rhs);

/*
 * Converts from `rawstr' to appropriate cling_primary
 * according to `type'.
 */
void cling_primary_init(union cling_primary *self, size_t type,
                        char const *rawstr);
/*
 * As long as cling_primary holds integral
 * value, converts it to int according to
 * `type' and stores it back into `self'.
 */
void cling_primary_toint(union cling_primary *self, size_t type);

/*
 * Table insertion.
 */

/*
 * self should be const_decl or parsed by single_const_decl
 */
void cling_ast_insert_const(struct utillib_json_value const *self,
                            struct cling_symbol_table *symbols, int scope_kind);

/*
 * self should be var_decl or parsed by single_var_decl
 */
void cling_ast_insert_variable(struct utillib_json_value const *self,
                               struct cling_symbol_table *symbols,
                               int scope_kind);

void cling_ast_insert_arglist(struct utillib_json_value const *self,
                              struct cling_symbol_table *symbols);

void cling_ast_insert_function(struct utillib_json_value const *self,
                               struct cling_symbol_table *symbol_table);

#endif /* CLING_AST_H */
