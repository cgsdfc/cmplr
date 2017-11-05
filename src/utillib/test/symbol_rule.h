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
#ifndef UTILLIB_TEST_RULE_H
#define UTILLIB_TEST_RULE_H
#include <utillib/rule.h>
#include <utillib/symbol.h>
#include <utillib/test.h>

UTILLIB_TEST_CONST(terminals_size, 5)
UTILLIB_TEST_CONST(non_terminals_size, 5)
UTILLIB_TEST_CONST(rules_size, 8)

UTILLIB_ENUM_BEGIN(test_symbol_kind)
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
UTILLIB_ENUM_END(test_symbol_kind);

UTILLIB_SYMBOL_BEGIN(test_symbols)
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
UTILLIB_SYMBOL_END(test_symbols);

UTILLIB_RULE_BEGIN(test_rules)
UTILLIB_RULE_ELEM(SYM_E, SYM_T, SYM_EP)
UTILLIB_RULE_ELEM(SYM_EP, SYM_PLUS, SYM_T, SYM_EP)
UTILLIB_RULE_ELEM(SYM_EP, UT_SYM_EPS)
UTILLIB_RULE_ELEM(SYM_T, SYM_F, SYM_TP)
UTILLIB_RULE_ELEM(SYM_TP, SYM_MUL, SYM_F, SYM_TP)
UTILLIB_RULE_ELEM(SYM_TP, UT_SYM_EPS)
UTILLIB_RULE_ELEM(SYM_F, SYM_I)
UTILLIB_RULE_ELEM(SYM_F, SYM_LP, SYM_E, SYM_RP)
UTILLIB_RULE_END(test_rules);

#endif
