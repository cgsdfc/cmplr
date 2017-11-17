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
#include "pcode.h"

UTILLIB_ETAB_BEGIN(pascal_pcode_kind)
UTILLIB_ETAB_ELEM(PCODE_LIT)
UTILLIB_ETAB_ELEM(PCODE_OPR)
UTILLIB_ETAB_ELEM(PCODE_LOD)
UTILLIB_ETAB_ELEM(PCODE_STO)
UTILLIB_ETAB_ELEM(PCODE_CAL)
UTILLIB_ETAB_ELEM(PCODE_INT)
UTILLIB_ETAB_ELEM(PCODE_JMP)
UTILLIB_ETAB_ELEM(PCODE_JPC)
UTILLIB_ETAB_ELEM(PCODE_RED)
UTILLIB_ETAB_ELEM(PCODE_WRT)
UTILLIB_ETAB_END(pascal_pcode_kind);

void pascal_interp_exec(struct pascal_interp *self) {
  struct pascal_pcode *code = utillib_vector_at(&self->mem_code, self->pc);
  switch (code->code[0]) {
    int uint;
  case PCODE_LIT:
    uint = code->code[2];
    utillib_vector_push_back(&self->mem_data, uint);
    break;
  case PCODE_OPR:
    interp_calculate(self, code->code[2]);
    break;
  case PCODE_LOD:
  }
