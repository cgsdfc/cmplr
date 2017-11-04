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
#ifndef UTILLIB_RULE_H
#define UTILLIB_RULE_H
#include "symbol.h"
#include "vector.h"
#include "json.h"
#define UTILLIB_RULE_MAX_RHS 10
#define UTILLIB_RULE_RHS(RULE) ((RULE)->RHS)
#define UTILLIB_RULE_LHS(RULE) ((RULE)->LHS)
#define UTILLIB_RULE_NULL (&utillib_rule_null)
#define UTILLIB_RULE_ELEM(LHS, ...) { .LHS_LIT=(LHS), .RHS_LIT= {  __VA_ARGS__ , UT_SYM_NULL } },
#define UTILLIB_RULE_BEGIN(NAME) static const struct utillib_rule_literal NAME[]={
#define UTILLIB_RULE_END(NAME) {.LHS_LIT=UT_SYM_NULL}};

struct utillib_rule_literal {
  int LHS_LIT;
  int RHS_LIT[UTILLIB_RULE_MAX_RHS];
};

struct utillib_rule {
  struct utillib_symbol const * LHS;
  utillib_vector RHS;
};

extern struct utillib_rule utillib_rule_null;

/**
 * \struct utillib_rule_index
 * Provides fast access to each rule
 * given the left-hand-side non terminal symbol
 * of the rule. 
 * Each element of the index is a pair of the LHS
 * symbol and its productions in a vector.
 */
struct utillib_rule_index {
  struct utillib_vector rules;
  struct utillib_vector terminals;
  struct utillib_vector non_terminals;
};

void utillib_rule_index_init(struct utillib_rule_index * self, 
    struct utillib_symbol const * symbols,
    struct utillib_rule_literal const * rule_literals);

void utillib_rule_index_destroy(struct utillib_rule_index *self);
#endif // UTILLIB_RULE_H
