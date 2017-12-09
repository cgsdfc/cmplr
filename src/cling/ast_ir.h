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
#ifndef CLING_AST_IR_H
#define CLING_AST_IR_H
#include "ir.h"
/*
 * Well, we are gonna generate
 * quarternary form of our code.
 * OMG, such a weird name!
 */

/*
 * Holder of its own instrs.
 * Basic unit of code.
 */
struct cling_ast_function {
  char *name;
  unsigned int temps;
  struct utillib_vector instrs;
};

/*
 * Holder of all the functions
 * and some global init code.
 */
struct cling_ast_program {
  struct utillib_vector init_code;
  struct utillib_vector funcs;
};

void cling_ast_program_init(struct cling_ast_program *self);

void cling_ast_program_destroy(struct cling_ast_program *self);

void cling_ast_program_print(struct cling_ast_program const* self);

void cling_ast_ir_emit_program(
    struct utillib_json_value const *self,
    struct cling_symbol_table const *symbol_table,
    struct cling_ast_program *program);

#endif /* CLING_AST_IR_H */
