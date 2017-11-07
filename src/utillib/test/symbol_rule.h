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

enum {
  UT_LL1_INPUT0,
  UT_LL1_INPUT1,
};

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
UTILLIB_TEST_CONST(test_terminals_size, 5)
UTILLIB_TEST_CONST(test_non_terminals_size, 5)
UTILLIB_TEST_CONST(test_rules_size, 8)

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

static UTILLIB_SYMBOL_BEGIN(test_symbols) UTILLIB_SYMBOL_NON_TERMINAL(SYM_E)
    UTILLIB_SYMBOL_NON_TERMINAL(SYM_EP) UTILLIB_SYMBOL_NON_TERMINAL(SYM_T)
        UTILLIB_SYMBOL_NON_TERMINAL(SYM_TP) UTILLIB_SYMBOL_NON_TERMINAL(SYM_F)
            UTILLIB_SYMBOL_TERMINAL(SYM_I) UTILLIB_SYMBOL_TERMINAL(SYM_LP)
                UTILLIB_SYMBOL_TERMINAL(SYM_RP)
                    UTILLIB_SYMBOL_TERMINAL(SYM_PLUS)
                        UTILLIB_SYMBOL_TERMINAL(SYM_MUL)
                            UTILLIB_SYMBOL_END(test_symbols);

static UTILLIB_RULE_BEGIN(test_rules) UTILLIB_RULE_ELEM(SYM_E, SYM_T, SYM_EP)
    UTILLIB_RULE_ELEM(SYM_EP, SYM_PLUS, SYM_T, SYM_EP)
        UTILLIB_RULE_ELEM(SYM_EP, UT_SYM_EPS)
            UTILLIB_RULE_ELEM(SYM_T, SYM_F, SYM_TP)
                UTILLIB_RULE_ELEM(SYM_TP, SYM_MUL, SYM_F, SYM_TP)
                    UTILLIB_RULE_ELEM(SYM_TP, UT_SYM_EPS)
                        UTILLIB_RULE_ELEM(SYM_F, SYM_LP, SYM_E, SYM_RP)
                            UTILLIB_RULE_ELEM(SYM_F, SYM_I)
                                UTILLIB_RULE_END(test_rules);

static const size_t test_rules_FIRST[][10] = {
        [SYM_E] = {SYM_LP, SYM_I, UT_SYM_NULL},
        [SYM_EP] = {SYM_PLUS, UT_SYM_EPS, UT_SYM_NULL},
        [SYM_T] = {SYM_LP, SYM_I, UT_SYM_NULL},
        [SYM_TP] = {SYM_MUL, UT_SYM_EPS, UT_SYM_NULL},
        [SYM_F] = {SYM_LP, SYM_I, UT_SYM_NULL},
};

static const size_t test_rules_FOLLOW[][10] = {
        [SYM_E] = {SYM_RP, UT_SYM_EOF, UT_SYM_NULL},
        [SYM_EP] = {SYM_RP, UT_SYM_EOF, UT_SYM_NULL},
        [SYM_T] = {SYM_PLUS, SYM_RP, UT_SYM_EOF, UT_SYM_NULL},
        [SYM_TP] = {SYM_PLUS, SYM_RP, UT_SYM_EOF, UT_SYM_NULL},
        [SYM_F] = {SYM_PLUS, SYM_RP, UT_SYM_EOF, SYM_MUL, UT_SYM_NULL},
};

static const size_t test_rules_TABLE[][20] = {

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

static const size_t test_symbols_input[] = {
    SYM_I, SYM_PLUS, SYM_I, SYM_MUL, SYM_I, UT_SYM_EOF, UT_SYM_NULL,
};

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
UTILLIB_ENUM_BEGIN(test2_symbol_kind)
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
UTILLIB_ENUM_END(test2_symbol_kind);

static UTILLIB_SYMBOL_BEGIN(test2_symbols) UTILLIB_SYMBOL_NON_TERMINAL(T2_SYM_E)
    UTILLIB_SYMBOL_NON_TERMINAL(T2_SYM_EP) UTILLIB_SYMBOL_NON_TERMINAL(T2_SYM_T)
        UTILLIB_SYMBOL_NON_TERMINAL(T2_SYM_F) UTILLIB_SYMBOL_NON_TERMINAL(
            T2_SYM_P) UTILLIB_SYMBOL_NON_TERMINAL(T2_SYM_FP)
            UTILLIB_SYMBOL_NON_TERMINAL(T2_SYM_TP)

                UTILLIB_SYMBOL_TERMINAL(T2_SYM_LP)
                    UTILLIB_SYMBOL_TERMINAL(T2_SYM_RP)
                        UTILLIB_SYMBOL_TERMINAL(T2_SYM_MUL)
                            UTILLIB_SYMBOL_TERMINAL(T2_SYM_PLUS)
                                UTILLIB_SYMBOL_TERMINAL(T2_SYM_A)
                                    UTILLIB_SYMBOL_TERMINAL(T2_SYM_B)
                                        UTILLIB_SYMBOL_TERMINAL(T2_SYM_XOR)
                                            UTILLIB_SYMBOL_END(test2_symbols);

static UTILLIB_RULE_BEGIN(test2_rules) UTILLIB_RULE_ELEM(T2_SYM_E, T2_SYM_T,
                                                         T2_SYM_EP)
    UTILLIB_RULE_ELEM(T2_SYM_EP, T2_SYM_PLUS,
                      T2_SYM_E) UTILLIB_RULE_ELEM(T2_SYM_EP, UT_SYM_EPS)
        UTILLIB_RULE_ELEM(T2_SYM_T, T2_SYM_F, T2_SYM_TP)
            UTILLIB_RULE_ELEM(T2_SYM_TP, T2_SYM_T) UTILLIB_RULE_ELEM(T2_SYM_TP,
                                                                     UT_SYM_EPS)
                UTILLIB_RULE_ELEM(T2_SYM_F, T2_SYM_P, T2_SYM_FP)
                    UTILLIB_RULE_ELEM(T2_SYM_FP, T2_SYM_MUL, T2_SYM_FP)
                        UTILLIB_RULE_ELEM(T2_SYM_TP, UT_SYM_EPS)
                            UTILLIB_RULE_ELEM(T2_SYM_P, T2_SYM_LP, T2_SYM_E,
                                              T2_SYM_RP)
                                UTILLIB_RULE_ELEM(T2_SYM_P, T2_SYM_A)
                                    UTILLIB_RULE_ELEM(T2_SYM_P, T2_SYM_B)
                                        UTILLIB_RULE_ELEM(T2_SYM_P, T2_SYM_XOR)
                                            UTILLIB_RULE_END(test2_rules);

#endif
