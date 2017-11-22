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
#include <utillib/test.h>

UTILLIB_TEST_SET_UP() {
  utillib_rule_index_init(UT_FIXTURE, ll1_sample_1_symbols, ll1_sample_1_rules);
}

UTILLIB_TEST_TEAR_DOWN() { utillib_rule_index_destroy(UT_FIXTURE); }

UTILLIB_TEST(rule_index_init) {
  struct utillib_rule_index *index = UT_FIXTURE;
  UTILLIB_TEST_ASSERT_EQ(utillib_rule_index_terminals_size(index),
                         LL1_SAMPLE_1_TER_SIZE);
  UTILLIB_TEST_ASSERT_EQ(utillib_rule_index_non_terminals_size(index),
                         LL1_SAMPLE_1_NON_SIZE);
  UTILLIB_TEST_ASSERT_EQ(utillib_rule_index_rules_size(index),
                         LL1_SAMPLE_1_RULE_SIZE);
}

UTILLIB_TEST(rule_index_json) {
  struct utillib_json_value *val =
      utillib_rule_index_json_object_create(UT_FIXTURE);
  utillib_json_pretty_print(val, stderr);
  utillib_json_value_destroy(val);
}

/**
 * \test rule_index_index_not_overflow
 * Proves that terminal or non-terminal values are mapped
 * to [0, N) where N is the number of terminals (non-terminals).
 *
 * We do not test the uniqueness of those indecies
 * since the values are garuanteed to be unique (enum) and
 * provided that the mapping is one to one onto function.
 * For the same sake, we do not test against the overlap
 * of terminal and non-terminal indices.
 *
 * This test is important since the table indecies should
 * not overflow its extends.
 */

UTILLIB_TEST_AUX(rule_index_index_not_overflow_helper,
                 struct utillib_vector const *vector, size_t symbols_size) {
  UTILLIB_VECTOR_FOREACH(struct utillib_symbol const *, symbol, vector) {
    size_t index = utillib_rule_index_symbol_index(UT_FIXTURE, symbol);
    UTILLIB_TEST_EXPECT_GE(index, 0);
    UTILLIB_TEST_EXPECT_LT(index, symbols_size);
  }
}

UTILLIB_TEST(rule_index_index_not_overflow) {
  struct utillib_rule_index *self = UT_FIXTURE;
  struct utillib_vector const *terminal_vector = &self->terminals;
  struct utillib_vector const *non_terminal_vector = &self->non_terminals;

  UTILLIB_TEST_AUX_INVOKE(rule_index_index_not_overflow_helper, terminal_vector,
                          self->terminals_size);
  UTILLIB_TEST_AUX_INVOKE(rule_index_index_not_overflow_helper,
                          non_terminal_vector, self->non_terminals_size);
}

/**
 * \test rule_index_rule_id
 * Ensures that each rule in `self->rules' has the same
 * rule_id as its index.
 */
UTILLIB_TEST(rule_index_rule_id) {
  struct utillib_rule_index const *self = UT_FIXTURE;
  struct utillib_vector const *RULES = utillib_rule_index_rules(self);
  for (int i = 0; i < LL1_SAMPLE_1_RULE_SIZE; ++i) {
    struct utillib_rule const *rule = utillib_vector_at(RULES, i);
    UTILLIB_TEST_EXPECT_EQ(rule->id, i);
  }
}

UTILLIB_TEST(rule_index_build_LHS_index) {}

UTILLIB_TEST_DEFINE(Utillib_Rule) {
  UTILLIB_TEST_BEGIN(Utillib_Rule)
  UTILLIB_TEST_RUN(rule_index_init)
  UTILLIB_TEST_RUN(rule_index_index_not_overflow)
  UTILLIB_TEST_RUN(rule_index_rule_id)
  /* Verbose output skipped */
  UTILLIB_TEST_RUN(rule_index_json)
  UTILLIB_TEST_END(Utillib_Rule)
  UTILLIB_TEST_FIXTURE(struct utillib_rule_index)
  UTILLIB_TEST_RETURN(Utillib_Rule)
}
