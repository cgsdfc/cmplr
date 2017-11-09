
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
#include "non_ll1_sample_2.h"

UTILLIB_SYMBOL_BEGIN(non_ll1_sample_2_symbols)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_S)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_A)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_B)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_C)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_U)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_R)
UTILLIB_SYMBOL_TERMINAL(SYM_a)
UTILLIB_SYMBOL_TERMINAL(SYM_b)
UTILLIB_SYMBOL_TERMINAL(SYM_c)
UTILLIB_SYMBOL_TERMINAL(SYM_d)
UTILLIB_SYMBOL_END(non_ll1_sample_2_symbols);

UTILLIB_RULE_BEGIN(non_ll1_sample_2_rules)
  UTILLIB_RULE_ELEM(SYM_S, SYM_A, SYM_U)
  UTILLIB_RULE_ELEM(SYM_S, SYM_B, SYM_R)
  UTILLIB_RULE_ELEM(SYM_A, SYM_a, SYM_A, SYM_U)
  UTILLIB_RULE_ELEM(SYM_A, SYM_b)
  UTILLIB_RULE_ELEM(SYM_B, SYM_a, SYM_B, SYM_R)
  UTILLIB_RULE_ELEM(SYM_B, SYM_b)
  UTILLIB_RULE_ELEM(SYM_U, SYM_c)
  UTILLIB_RULE_ELEM(SYM_R, SYM_d)
UTILLIB_RULE_END(non_ll1_sample_2_rules);
