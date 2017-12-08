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
#ifndef CLING_POLISH_IR_H
#define CLING_POLISH_IR_H
#include "ir.h"
#include <utillib/vector.h>

struct cling_polish_ir {
  /*
   * Stack holding ast nodes
   */
  struct utillib_vector stack;
  /*
   * Stack holding operands when evaluating.
   */
  struct utillib_vector opstack;
  /*
   * From which temps counter starts
   */
  struct cling_ast_ir_global *global;
};

void cling_polish_ir_init(struct cling_polish_ir *self,
    struct utillib_json_value const* root,
    struct cling_ast_ir_global * global);

void cling_polish_ir_destroy(struct cling_polish_ir *self);

/*
 * Emits ast_ir to instrs.
 */
void cling_polish_ir_emit(struct cling_polish_ir *self,
    struct utillib_vector *instrs);

/*
 * Put the final result of polish_ir into
 * operands[index].
 */
void cling_polish_ir_result(struct cling_polish_ir const *self,
    struct cling_ast_ir *ir, int index);

struct utillib_json_value *
cling_polish_ir_json_array_create(struct cling_polish_ir const *self);
#endif /* CLING_POLISH_IR_H */
