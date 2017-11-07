
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
#ifndef UTILLIB_TEST_LL1_SAMPLE_2_H
#define UTILLIB_TEST_LL1_SAMPLE_2_H
#include <stddef.h> // size_t
#include <utillib/enum.h>
#include <utillib/rule.h>
#include <utillib/symbol.h>

#define LL1_SAMPLE_2_EXTEND 20
#define LL1_SAMPLE_2_TER_SIZE 5
#define LL1_SAMPLE_2_NON_SIZE 5
#define LL1_SAMPLE_2_RULE_SIZE 8

/**
 * Rule
 * E := T E'
 * E' := + E | eps
 * T := F T'
 * T' := T | eps
 * F := P F'
 * F' := * F' | eps
 * P := ( E ) | a | b | ^
 */

UTILLIB_ENUM_BEGIN(ll1_sample_2_symbol_kind)
UTILLIB_ENUM_ELEM_INIT(T2_SYM_E, 1)
UTILLIB_ENUM_ELEM(T2_SYM_EP)
UTILLIB_ENUM_ELEM(T2_SYM_T)
UTILLIB_ENUM_ELEM(T2_SYM_F)
UTILLIB_ENUM_ELEM(T2_SYM_P)
UTILLIB_ENUM_ELEM(T2_SYM_FP)
UTILLIB_ENUM_ELEM(T2_SYM_TP)
UTILLIB_ENUM_ELEM(T2_SYM_LP)
UTILLIB_ENUM_ELEM(T2_SYM_RP)
UTILLIB_ENUM_ELEM(T2_SYM_PLUS)
UTILLIB_ENUM_ELEM(T2_SYM_MUL)
UTILLIB_ENUM_ELEM(T2_SYM_A)
UTILLIB_ENUM_ELEM(T2_SYM_B)
UTILLIB_ENUM_ELEM(T2_SYM_XOR)
UTILLIB_ENUM_END(ll1_sample_2_symbol_kind);

extern const struct utillib_symbol ll1_sample_2_symbols[];
extern const struct utillib_rule_literal ll1_sample_2_rules[];

/* Unimplemented */
extern const size_t ll1_sample_2_first[][LL1_SAMPLE_2_EXTEND];
extern const size_t ll1_sample_2_follow[][LL1_SAMPLE_2_EXTEND];
extern const size_t ll1_sample_2_table[][LL1_SAMPLE_2_EXTEND];

extern const size_t ll1_sample_2_input_1[];
extern const size_t ll1_sample_2_input_2[];

#endif // UTILLIB_TEST_LL1_SAMPLE_2_H
