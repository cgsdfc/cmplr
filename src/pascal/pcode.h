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
#ifndef PASCAL_PCODE_H
#define PASCAL_PCODE_H

#include <utillib/enum.h>
#include <utillib/vector.h>

UTILLIB_ENUM_BEGIN(pascal_pcode_kind)
  UTILLIB_ENUM_ELEM(PCODE_LIT)
  UTILLIB_ENUM_ELEM(PCODE_OPR)
  UTILLIB_ENUM_ELEM(PCODE_LOD)
  UTILLIB_ENUM_ELEM(PCODE_STO)
  UTILLIB_ENUM_ELEM(PCODE_CAL)
  UTILLIB_ENUM_ELEM(PCODE_INT)
  UTILLIB_ENUM_ELEM(PCODE_JMP)
  UTILLIB_ENUM_ELEM(PCODE_JPC)
  UTILLIB_ENUM_ELEM(PCODE_RED)
  UTILLIB_ENUM_ELEM(PCODE_WRT)
UTILLIB_ENUM_END(pascal_pcode_kind);

struct pascal_pcode {
  int code[3];
};

struct pascal_interp {
  struct utillib_vector mem_code;
  struct utillib_vector mem_data;
  int pc;


};

void pascal_interp_exec(struct pascal_interp *self);

#endif 
