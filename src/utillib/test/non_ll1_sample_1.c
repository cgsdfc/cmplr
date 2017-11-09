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
#include "non_ll1_sample_1.h"

UTILLIB_SYMBOL_BEGIN(non_ll1_sample_1_symbols)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_S)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_A)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_B)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_C)
UTILLIB_SYMBOL_TERMINAL(SYM_a)
UTILLIB_SYMBOL_TERMINAL(SYM_b)
UTILLIB_SYMBOL_TERMINAL(SYM_c)

UTILLIB_SYMBOL_END(non_ll1_sample_1_symbols);

UTILLIB_RULE_BEGIN(non_ll1_sample_1_rules)
  UTILLIB_RULE_ELEM(SYM_S, SYM_a, SYM_b, SYM_B)
  UTILLIB_RULE_ELEM(SYM_A, SYM_S, SYM_C)
  UTILLIB_RULE_ELEM(SYM_A, SYM_B, SYM_A, SYM_A)
  UTILLIB_RULE_ELEM(SYM_A, UT_SYM_EPS)
  UTILLIB_RULE_ELEM(SYM_B, SYM_A, SYM_b, SYM_A)
  UTILLIB_RULE_ELEM(SYM_C, SYM_B)
  UTILLIB_RULE_ELEM(SYM_C, SYM_c)
UTILLIB_RULE_END(non_ll1_sample_1_rules);

const size_t non_ll1_sample_1_first[][NON_LL1_SAMPLE_1_EXTEND]={
  [SYM_S]={ SYM_a, UT_SYM_NULL },
  [SYM_A]={SYM_a, UT_SYM_EPS, SYM_b , UT_SYM_NULL },
  [SYM_B]={SYM_a, SYM_b , UT_SYM_NULL },
  [SYM_C]={SYM_a, SYM_b, SYM_c, UT_SYM_NULL },
};

const size_t non_ll1_sample_1_follow[][NON_LL1_SAMPLE_1_EXTEND]={
  [SYM_S]={ SYM_c , UT_SYM_NULL },
  [SYM_A]={SYM_b, UT_SYM_NULL },
};

  /* [SYM_B]={S */
/* [SYM_C]={S */

