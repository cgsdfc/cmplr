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
#ifndef CLING_BASIC_BLOCK
#define CLING_BASIC_BLOCK

#include <utillib/vector.h>

/*
 * Divide each function into a number of basic_blocks
 * for optimizer to run on.
 */

struct cling_basic_block {
  int block_id;
  unsigned int begin;
  unsigned int end;
  struct utillib_vector *instrs;
};

void cling_basic_block_construct(struct utillib_vector *blocks,
                                 struct utillib_vector *instrs);
void basic_block_destroy(struct cling_basic_block *self);
void basic_block_display(struct utillib_vector const *basic_blocks);

#endif /* CLING_BASIC_BLOCK */
