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
#ifndef UTILLIB_TEST_NON_LL1_SAMPLE_2_H
#define UTILLIB_TEST_NON_LL1_SAMPLE_2_H
#include <stddef.h> // size_t
#include <utillib/enum.h>
#include <utillib/ll1_parser.h>
#include <utillib/rule.h>
#include <utillib/symbol.h>

#define NON_LL1_SAMPLE_2_EXTEND 20
#define NON_LL1_SAMPLE_2_TER_SIZE 5
#define NON_LL1_SAMPLE_2_NON_SIZE 5
#define NON_LL1_SAMPLE_2_RULE_SIZE 8

/*
 * Rule:
 * S := A U | B R
 * A := a A U | b
 * B := a B R | b
 * U := c
 * R := d
 */

UTILLIB_ENUM_BEGIN(non_ll1_sample_2_symbol_kind)
UTILLIB_ENUM_ELEM_INIT(SYM_S, 1)
UTILLIB_ENUM_ELEM(SYM_A)
UTILLIB_ENUM_ELEM(SYM_B)
UTILLIB_ENUM_ELEM(SYM_C)
UTILLIB_ENUM_ELEM(SYM_U)
UTILLIB_ENUM_ELEM(SYM_R)

UTILLIB_ENUM_ELEM(SYM_a)
UTILLIB_ENUM_ELEM(SYM_b)
UTILLIB_ENUM_ELEM(SYM_c)
UTILLIB_ENUM_ELEM(SYM_d)
UTILLIB_ENUM_END(non_ll1_sample_2_symbol_kind);

extern const struct utillib_symbol non_ll1_sample_2_symbols[];
extern const struct utillib_rule_literal non_ll1_sample_2_rules[];

extern const size_t non_ll1_sample_2_input_1[];
extern const size_t non_ll1_sample_2_input_2[];

#endif // UTILLIB_TEST_NON_LL1_SAMPLE_2_H
