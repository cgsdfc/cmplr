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
#include "ll1_sample_1.h"
#include <utillib/rule.h>
#include <utillib/symbol.h>
#include <utillib/test.h>

UTILLIB_SYMBOL_BEGIN(ll1_sample_1_symbols)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_E)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_EP)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_T)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_TP)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_F)
UTILLIB_SYMBOL_TERMINAL(SYM_I)
UTILLIB_SYMBOL_TERMINAL(SYM_LP)
UTILLIB_SYMBOL_TERMINAL(SYM_RP)
UTILLIB_SYMBOL_TERMINAL(SYM_PLUS)
UTILLIB_SYMBOL_TERMINAL(SYM_MUL)
UTILLIB_SYMBOL_END(ll1_sample_1_symbols);

UTILLIB_RULE_BEGIN(ll1_sample_1_rules)
UTILLIB_RULE_ELEM(SYM_E, SYM_T, SYM_EP)
UTILLIB_RULE_ELEM(SYM_EP, SYM_PLUS, SYM_T, SYM_EP)
UTILLIB_RULE_ELEM(SYM_EP, UT_SYM_EPS)
UTILLIB_RULE_ELEM(SYM_T, SYM_F, SYM_TP)
UTILLIB_RULE_ELEM(SYM_TP, SYM_MUL, SYM_F, SYM_TP)
UTILLIB_RULE_ELEM(SYM_TP, UT_SYM_EPS)
UTILLIB_RULE_ELEM(SYM_F, SYM_LP, SYM_E, SYM_RP)
UTILLIB_RULE_ELEM(SYM_F, SYM_I)
UTILLIB_RULE_END(ll1_sample_1_rules);

const size_t ll1_sample_1_input_1[] = {
    SYM_I, SYM_PLUS, SYM_I, SYM_MUL, SYM_I, UT_SYM_EOF, UT_SYM_NULL,
};

void ll1_sample_1_error_handler(void *self,
                                struct utillib_ll1_parser_error const *error) {}

void ll1_sample_1_terminal_handler(void *self,
                                   struct utillib_symbol const *terminal,
                                   void const *semantic) {}

void ll1_sample_1_rule_handler(void *self, struct utillib_rule const *rule) {}

const struct utillib_ll1_parser_callback ll1_sample_1_parser_callback = {
    .terminal_handler = ll1_sample_1_terminal_handler,
    .rule_handler = ll1_sample_1_rule_handler,
    .error_handler = ll1_sample_1_error_handler,
};

const size_t ll1_sample_1_first[][10] = {
        [SYM_E] = {SYM_LP, SYM_I, UT_SYM_NULL},
        [SYM_EP] = {SYM_PLUS, UT_SYM_EPS, UT_SYM_NULL},
        [SYM_T] = {SYM_LP, SYM_I, UT_SYM_NULL},
        [SYM_TP] = {SYM_MUL, UT_SYM_EPS, UT_SYM_NULL},
        [SYM_F] = {SYM_LP, SYM_I, UT_SYM_NULL},
};

const size_t ll1_sample_1_follow[][10] = {
        [SYM_E] = {SYM_RP, UT_SYM_EOF, UT_SYM_NULL},
        [SYM_EP] = {SYM_RP, UT_SYM_EOF, UT_SYM_NULL},
        [SYM_T] = {SYM_PLUS, SYM_RP, UT_SYM_EOF, UT_SYM_NULL},
        [SYM_TP] = {SYM_PLUS, SYM_RP, UT_SYM_EOF, UT_SYM_NULL},
        [SYM_F] = {SYM_PLUS, SYM_RP, UT_SYM_EOF, SYM_MUL, UT_SYM_NULL},
};
const size_t ll1_sample_1_table[][20] = {

        [SYM_E][SYM_I] = UTILLIB_TEST_ZERO_BASED(1),
        [SYM_T][SYM_I] = UTILLIB_TEST_ZERO_BASED(4),
        [SYM_F][SYM_I] = UTILLIB_TEST_ZERO_BASED(8),

        [SYM_EP][SYM_PLUS] = UTILLIB_TEST_ZERO_BASED(2),
        [SYM_TP][SYM_PLUS] = UTILLIB_TEST_ZERO_BASED(6),

        [SYM_TP][SYM_MUL] = UTILLIB_TEST_ZERO_BASED(5),

        [SYM_E][SYM_LP] = UTILLIB_TEST_ZERO_BASED(1),
        [SYM_T][SYM_LP] = UTILLIB_TEST_ZERO_BASED(4),
        [SYM_F][SYM_LP] = UTILLIB_TEST_ZERO_BASED(7),

        [SYM_EP][SYM_RP] = UTILLIB_TEST_ZERO_BASED(3),
        [SYM_TP][SYM_RP] = UTILLIB_TEST_ZERO_BASED(6),

        [SYM_EP][UT_SYM_EOF] = UTILLIB_TEST_ZERO_BASED(3),
        [SYM_TP][UT_SYM_EOF] = UTILLIB_TEST_ZERO_BASED(6),
};
