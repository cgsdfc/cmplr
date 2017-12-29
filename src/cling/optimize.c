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
#include "optimize.h"
#include "ast_ir.h"
#include "basic_block.h"
#include "lcse.h"
#include <assert.h>
#include <stdlib.h>

void cling_lcse_optimize(struct cling_ast_function *ast_func) {
  struct cling_lcse_optimizer optimizer;
  struct utillib_vector instrs;
  struct utillib_vector basic_blocks;
  struct cling_basic_block *block;

  utillib_vector_init(&instrs);
  utillib_vector_init(&basic_blocks);
  cling_basic_block_construct(&basic_blocks, &ast_func->instrs);

  /* ast_ir_vector_print(&ast_func->instrs, stdout); */
  cling_lcse_optimizer_init(&optimizer, ast_func);
  UTILLIB_VECTOR_FOREACH(block, &basic_blocks) {
    cling_lcse_optimizer_emit(&optimizer, block, &instrs);
  }
  ast_ir_fix_address(&instrs, optimizer.address_map);
  /* ast_ir_vector_print(&instrs, stdout); */

  utillib_vector_clear(&ast_func->instrs);
  utillib_vector_append(&ast_func->instrs, &instrs);
  cling_lcse_optimizer_destroy(&optimizer);
  utillib_vector_destroy(&instrs);
  utillib_vector_destroy_owning(&basic_blocks, free);
}

void cling_optimize(struct cling_ast_program *ast_program) {
  struct cling_ast_function *ast_func;
  UTILLIB_VECTOR_FOREACH(ast_func, &ast_program->funcs) {
    cling_lcse_optimize(ast_func);
  }
}
