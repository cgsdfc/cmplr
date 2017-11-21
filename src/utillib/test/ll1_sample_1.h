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
#ifndef UTILLIB_TEST_LL1_SAMPLE_1_H
#define UTILLIB_TEST_LL1_SAMPLE_1_H
#include <stddef.h> // size_t
#include <utillib/enum.h>
#include <utillib/ll1_parser.h>
#include <utillib/rule.h>
#include <utillib/symbol.h>

#define LL1_SAMPLE_1_TER_SIZE 6
#define LL1_SAMPLE_1_NON_SIZE 5
#define LL1_SAMPLE_1_RULE_SIZE 8

/*
 * Rule
 * E := T E'
 * E' := + T E'
 * E' := eps
 * T := F T'
 * T' := * F T'
 * T' := eps
 * F := ( E ) | i
 */

UTILLIB_ENUM_BEGIN(ll1_sample_1_symbol_kind)
UTILLIB_ENUM_ELEM_INIT(SYM_E, 1)
UTILLIB_ENUM_ELEM(SYM_EP)
UTILLIB_ENUM_ELEM(SYM_T)
UTILLIB_ENUM_ELEM(SYM_TP)
UTILLIB_ENUM_ELEM(SYM_F)
UTILLIB_ENUM_ELEM(SYM_I)
UTILLIB_ENUM_ELEM(SYM_LP)
UTILLIB_ENUM_ELEM(SYM_RP)
UTILLIB_ENUM_ELEM(SYM_PLUS)
UTILLIB_ENUM_ELEM(SYM_MUL)
UTILLIB_ENUM_END(ll1_sample_1_symbol_kind);

struct ll1_sample_1_semantic {
  struct utillib_vector stack;
};

extern const struct utillib_symbol ll1_sample_1_symbols[];
extern const struct utillib_rule_literal ll1_sample_1_rules[];
extern const struct utillib_ll1_parser_op ll1_sample_1_parser_op;

extern const size_t ll1_sample_1_first[][10];
extern const size_t ll1_sample_1_follow[][10];
extern const size_t ll1_sample_1_table[][20];

extern const size_t ll1_sample_1_input_1[];
extern const size_t ll1_sample_1_input_2[];

void ll1_sample_1_rule_handler(void *self, struct utillib_rule const *rule);

void ll1_sample_1_error_handler(void *self,
                                struct utillib_ll1_parser_error const *error);

void ll1_sample_1_terminal_handler(void *self,
                                   struct utillib_symbol const *terminal,
                                   void const *semantic);
#endif // UTILLIB_TEST_LL1_SAMPLE_1_H
