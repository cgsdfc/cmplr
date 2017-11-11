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
utillib_ll1_parser_rule_handler const * ll1_sample_1_rule_handlers[UTILLIB_RULE_SIZE(ll1_sample_1_rules)]={ };

