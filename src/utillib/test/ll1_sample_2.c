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
#include "ll1_sample_2.h"

UTILLIB_SYMBOL_BEGIN(ll1_sample_2_symbols)
UTILLIB_SYMBOL_NON_TERMINAL(T2_SYM_E)
UTILLIB_SYMBOL_NON_TERMINAL(T2_SYM_EP)
UTILLIB_SYMBOL_NON_TERMINAL(T2_SYM_T)
UTILLIB_SYMBOL_NON_TERMINAL(T2_SYM_F)
UTILLIB_SYMBOL_NON_TERMINAL(T2_SYM_P)
UTILLIB_SYMBOL_NON_TERMINAL(T2_SYM_FP)
UTILLIB_SYMBOL_NON_TERMINAL(T2_SYM_TP)

UTILLIB_SYMBOL_TERMINAL(T2_SYM_LP)
UTILLIB_SYMBOL_TERMINAL(T2_SYM_RP)
UTILLIB_SYMBOL_TERMINAL(T2_SYM_MUL)
UTILLIB_SYMBOL_TERMINAL(T2_SYM_PLUS)
UTILLIB_SYMBOL_TERMINAL(T2_SYM_A)
UTILLIB_SYMBOL_TERMINAL(T2_SYM_B)
UTILLIB_SYMBOL_TERMINAL(T2_SYM_XOR)
UTILLIB_SYMBOL_END(ll1_sample_2_symbols);

UTILLIB_RULE_BEGIN(ll1_sample_2_rules)
UTILLIB_RULE_ELEM(T2_SYM_E, T2_SYM_T, T2_SYM_EP)
UTILLIB_RULE_ELEM(T2_SYM_EP, T2_SYM_PLUS, T2_SYM_E)
UTILLIB_RULE_ELEM(T2_SYM_EP, UT_SYM_EPS)
UTILLIB_RULE_ELEM(T2_SYM_T, T2_SYM_F, T2_SYM_TP)
UTILLIB_RULE_ELEM(T2_SYM_TP, T2_SYM_T)
UTILLIB_RULE_ELEM(T2_SYM_TP, UT_SYM_EPS)
UTILLIB_RULE_ELEM(T2_SYM_F, T2_SYM_P, T2_SYM_FP)
UTILLIB_RULE_ELEM(T2_SYM_FP, T2_SYM_MUL, T2_SYM_FP)
UTILLIB_RULE_ELEM(T2_SYM_TP, UT_SYM_EPS)
UTILLIB_RULE_ELEM(T2_SYM_P, T2_SYM_LP, T2_SYM_E, T2_SYM_RP)
UTILLIB_RULE_ELEM(T2_SYM_P, T2_SYM_A)
UTILLIB_RULE_ELEM(T2_SYM_P, T2_SYM_B)
UTILLIB_RULE_ELEM(T2_SYM_P, T2_SYM_XOR)
UTILLIB_RULE_END(ll1_sample_2_rules);
