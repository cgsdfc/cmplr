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
/**
 * \file utillib/rule.h
 * Provides supports for defining BNF rule in source code
 * and building index from rules to allow other parser generating
 * algorithm to run upon.
 */
#include "symbol.h"
#include "vector.h"
#include "json.h"
  
/* Currently limit for the maximum size of right hand side */
#define UTILLIB_RULE_MAX_RHS 10
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
 * Collects basic info about the rule defined
 * in source code and provides it to other
 * algorithms.
 */
struct utillib_rule_index {
  struct utillib_vector rules;
  struct utillib_vector terminals;
  struct utillib_vector non_terminals;
  size_t min_terminal;
  size_t min_non_terminal;
  size_t * index;
};

void utillib_rule_index_init(struct utillib_rule_index * self, 
    struct utillib_symbol const * symbols,
    struct utillib_rule_literal const * rule_literals);

void utillib_rule_index_destroy(struct utillib_rule_index *self);

utillib_json_value_t * utillib_rule_json_object_create(void * base, size_t offset);
utillib_json_value_t * utillib_rule_index_json_object_create(void *base, size_t offset);

size_t utillib_rule_index_non_terminal_index(struct utillib_rule_index const*self, size_t value);
size_t utillib_rule_index_terminal_index(struct utillib_rule_index const*self, size_t value);
struct utillib_symbol * utillib_rule_index_top_symbol(struct utillib_rule_index const*self);

#define utillib_rule_rhs(RULE) (&(RULE)->RHS)
#define utillib_rule_lhs(RULE) ((RULE)->LHS)
#define utillib_rule_index_terminals_size(self) \
  (utillib_vector_size(&(self)->terminals))
#define utillib_rule_index_non_terminals_size(self) \
  (utillib_vector_size(&(self)->non_terminals))
#define utillib_rule_index_rules_size(self) \
  (utillib_vector_size(&(self)->rules))
#define utillib_rule_index_rules(self) \
  (&(self)->rules)
#define utillib_rule_index_rule_at(self, rule_id) \
  (utillib_vector_at(&(self)->rules, (rule_id)))


#endif // UTILLIB_RULE_H
