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
#ifndef CLING_MIPS_INTERP
#define CLING_MIPS_INTERP
#include "mips.h"
#include <utillib/hashmap.h>
#include <utillib/vector.h>

#define MIPS_MEMBLK_SIZE 64

#define MIPS_MEM_ARRAY_MAX 8
#define MIPS_MEM_MAX (MIPS_MEMBLK_SIZE * MIPS_MEM_ARRAY_MAX * MIPS_MEM_ARRAY_MAX)

UTILLIB_ENUM_BEGIN(cling_mips_ecode)
UTILLIB_ENUM_ELEM(MIPS_EC_OK)
UTILLIB_ENUM_ELEM(MIPS_EC_EXIT)
UTILLIB_ENUM_ELEM(MIPS_EC_ALIGN)
UTILLIB_ENUM_ELEM(MIPS_EC_NULL)
UTILLIB_ENUM_ELEM(MIPS_EC_NOMEM)
UTILLIB_ENUM_ELEM(MIPS_EC_INDEX)
UTILLIB_ENUM_END(cling_mips_ecode);


struct cling_mips_memblk {
  uint8_t mem[MIPS_MEMBLK_SIZE];
};

struct cling_mips_interp {
  int errno;
  uint32_t regs[CLING_MIPS_REG_MAX];
  uint32_t lo;
  uint32_t pc;
  struct cling_mips_memblk ** memory[MIPS_MEM_ARRAY_MAX];
  struct utillib_vector const *instrs;
  struct utillib_vector strings;
  struct utillib_hashmap labels;
};

void cling_mips_interp_init(struct cling_mips_interp *self);

void cling_mips_interp_load(struct cling_mips_interp *self,
                            struct cling_mips_program const *program);
void cling_mips_interp_unload(struct cling_mips_interp *self);

int cling_mips_interp_exec(struct cling_mips_interp *self);
void cling_mips_interp_destroy(struct cling_mips_interp *self);
void cling_mips_interp_print_error(struct cling_mips_interp const *self);

#endif /* CLING_MIPS_INTERP */
