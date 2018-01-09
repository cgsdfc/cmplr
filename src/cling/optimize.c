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
#include "option.h"
#include <assert.h>
#include <stdlib.h>


void cling_optimizer_init(struct cling_optimizer *self, struct cling_option const *option,
                struct cling_ast_function const *ast_func)
{
        self->option=option;
        utillib_vector_init(&self->basic_blocks);
        cling_lcse_optimizer_init(&self->lcse, ast_func);
}

void cling_optimizer_destroy(struct cling_optimizer *self)
{
        utillib_vector_destroy_owning(&self->basic_blocks, basic_block_destroy);
        cling_lcse_optimizer_destroy(&self->lcse);
}

void cling_optimizer_optimize(struct cling_optimizer *self, struct cling_ast_function *ast_func)
{
        cling_basic_block_construct(&self->basic_blocks, &ast_func->instrs);
        if (self->option->optimize_lcse)
                cling_lcse_optimizer_emit(&self->lcse, &self->basic_blocks, ast_func);
}
