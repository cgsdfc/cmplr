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
#include "misc.h"
#include "symbols.h"
#include "symbol_table.h"
#include "ir.h"

#include <assert.h>

int cling_symbol_to_type(int symbol) {
  switch (symbol) {
  case SYM_INTEGER:
  case SYM_UINT:
  case SYM_KW_INT:
    return CL_INT;
  case SYM_CHAR:
  case SYM_KW_CHAR:
    return CL_CHAR;
  case SYM_KW_VOID:
    return CL_VOID;
  default:
    assert(false);
  }
}

int cling_type_to_wide(int type) {
  switch (type) {
  case CL_INT:
    return CL_WORD;
  case CL_CHAR:
    return CL_BYTE;
  default:
    return CL_NULL;
  }
}

