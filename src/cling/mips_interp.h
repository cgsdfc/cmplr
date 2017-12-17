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
#define MIPS_PAGE_MAX 128
#define MIPS_PAGE_ARRAY_MAX 8
#define MIPS_MEM_MAX (MIPS_PAGE_MAX * MIPS_PAGE_ARRAY_MAX * MIPS_PAGE_ARRAY_MAX)

struct cling_mips_page {
  uint8_t mem[MIPS_PAGE_MAX];
};

struct cling_mips_interp {
  uint32_t regs[CLING_MIPS_REG_MAX];
  uint32_t lo;
  uint32_t pc;
  struct cling_mips_page 
    *page_array[MIPS_PAGE_ARRAY_MAX];
  struct cling_mips_program const *program;
};

void cling_mips_interp_init(struct cling_mips_interp *self, struct cling_mips_program const *program, bool clear);

int cling_mips_interp_exec(struct cling_mips_interp *self);
void cling_mips_interp_destroy(struct cling_mips_interp *self);


#endif /* CLING_MIPS_INTERP */
