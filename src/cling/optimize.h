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
#ifndef CLING_OPTIMIZE_H
#define CLING_OPTIMIZE_H
#include <utillib/vector.h>
#include "basic_block.h"
#include "lcse.h"

struct cling_option;
struct cling_ast_function;
/*
 * High level modules that controls
 * optizization.
 */
struct cling_optimizer {
  struct cling_option const *option;
  struct utillib_vector basic_blocks;
  struct cling_lcse_optimizer lcse;
};

void cling_optimizer_init(struct cling_optimizer *self, struct cling_option const *option);
void cling_optimizer_destroy(struct cling_optimizer *self);
void cling_optimizer_optimize(struct cling_optimizer *self,
    struct cling_ast_function *ast_func);

#endif /* CLING_OPTIMIZE_H */
