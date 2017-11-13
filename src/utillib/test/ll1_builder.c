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
#include "symbol_rule.h"
#include <utillib/ll1_builder.h>
#include <utillib/test.h>

/* Must have private copy */
static struct utillib_rule_index rule_index;
static struct utillib_vector2 ll1_table;

UTILLIB_TEST_SET_UP() {
  utillib_rule_index_init(&rule_index, test_symbols, test_rules);
  utillib_ll1_builder_init(UT_FIXTURE, &rule_index);
  utillib_ll1_builder_build_table(UT_FIXTURE, &ll1_table);
}

UTILLIB_TEST_TEAR_DOWN() {
  utillib_ll1_builder_destroy(UT_FIXTURE);
  utillib_rule_index_destroy(&rule_index);
  utillib_vector2_destroy(&ll1_table);
}

/**
 * \test build_expected_set
 * Due to the different formats, here we
 * convert the array format to bitset
 * so that a comparasion can be performed.
 * \param set Outputs built bitset.
 * \param symbols_size Needed to initilize bitset.
 * \param expected_set Built-from set.
 *
 */
UTILLIB_TEST_AUX(build_expected_set, struct utillib_ll1_set *set,
                 size_t symbols_size, size_t const expected_set[]) {
  utillib_ll1_set_init(set, symbols_size);
  for (size_t j = 0; expected_set[j] != UT_SYM_NULL; ++j) {
    size_t sym = expected_set[j];
    if (sym == UT_SYM_EPS || sym == UT_SYM_EOF)
      set->flag = true;
    else
      utillib_ll1_set_insert(set, sym);
  }
}

/**
 * \test ll1_builder_set_correct
 * Tests that actual_sets have at many elements
 * as expected_sets.
 * \param actual_sets The set from `utillib_ll1_builder'.
 * \param expected_sets The correct data.
 *
 * Note the differences between 2 sets:
 * Built one: has a flag as whether `epsilon' is inside &&
 * has bit set for the symbols inside.
 * Hard-coded one: has symbol value as content including
 * `epsilon'.
 */

UTILLIB_TEST_AUX(ll1_builder_set_correct,
                 struct utillib_ll1_set const *actual_sets,
                 size_t const expected_sets[][10]) {
  struct utillib_ll1_builder const *self = UT_FIXTURE;
  struct utillib_rule_index const *rule_index = self->rule_index;
  size_t symbols_size = utillib_rule_index_symbols_size(rule_index);
  UTILLIB_VECTOR_FOREACH(struct utillib_symbol const *, symbol,
                         utillib_rule_index_non_terminals(rule_index)) {
    size_t index = utillib_rule_index_symbol_index(rule_index, symbol);
    struct utillib_ll1_set const *actual_set = &actual_sets[index];
    struct utillib_ll1_set expected_set;
    UTILLIB_TEST_AUX_INVOKE(build_expected_set, &expected_set, symbols_size,
                            expected_sets[utillib_symbol_value(symbol)]);
    UTILLIB_TEST_EXPECT(utillib_ll1_set_equal(actual_set, &expected_set));
    utillib_ll1_set_destroy(&expected_set);
  }
}

UTILLIB_TEST(ll1_builder_FIRST_correct) {
  struct utillib_ll1_builder const *self = UT_FIXTURE;
  UTILLIB_TEST_AUX_INVOKE(ll1_builder_set_correct, self->FIRST,
                          test_rules_FIRST);
}

UTILLIB_TEST(ll1_builder_FOLLOW_correct) {
  struct utillib_ll1_builder const *self = UT_FIXTURE;
  UTILLIB_TEST_AUX_INVOKE(ll1_builder_set_correct, self->FOLLOW,
                          test_rules_FOLLOW);
}

/**
 * \test ll1_builder_locate_rule
 * Locates the Rule Index in `table'.
 * \param table An expected table whose index is symbol value.
 * \param row The index of terminal symbol.
 * \param col The index of the non terminal symbol.
 * \prule_id Outputs the found rule id.
 * The tricky part is special symbol `eof' has both value and index
 * of 0 which prevents us from treating it as a normal terminal.
 * Thus, if `eof' comes in, the `row' argument is unchanged.
 *
 */
UTILLIB_TEST_AUX(ll1_builder_locate_rule, size_t row, size_t col,
                 size_t *prule_id) {
  /* `eof' tricky part */
  col += col ? rule_index.min_terminal - 1 : 0;
  row += rule_index.min_non_terminal;
  *prule_id = test_rules_TABLE[row][col];
}

UTILLIB_TEST(ll1_builder_build_table_correct) {
  size_t Expected_RuleID;
  size_t Actual_RuleID;
  for (int i = 0; i < ll1_table.nrow; ++i) {
    for (int j = 0; j < ll1_table.ncol; ++j) {
      struct utillib_rule const *Actual_Rule =
          utillib_vector2_at(&ll1_table, i, j);
      UTILLIB_TEST_AUX_INVOKE(ll1_builder_locate_rule, i, j, &Expected_RuleID);
      if (Actual_Rule) {
        if (Actual_Rule == UTILLIB_RULE_EPS) {
          struct utillib_rule const *Expected_Rule =
              utillib_rule_index_rule_at(&rule_index, Expected_RuleID);
          struct utillib_symbol const *x =
              utillib_vector_front(&Expected_Rule->RHS);
          UTILLIB_TEST_EXPECT_EQ(x, UTILLIB_SYMBOL_EPS);
        } else {
          Actual_RuleID = Actual_Rule->id;
          UTILLIB_TEST_EXPECT_EQ(Actual_RuleID, Expected_RuleID);
        }
      } else {
        UTILLIB_TEST_EXPECT_EQ(Expected_RuleID, 0);
      }
    }
  }
}

/**
 * \test ll1_builder_build_table_json
 * Outputs JSON of the built LL(1) table.
 */
UTILLIB_TEST(ll1_builder_build_table_json) {
  struct utillib_json_value_t *val = utillib_vector2_json_array_create(
      &ll1_table, utillib_rule_json_object_create);
  utillib_json_pretty_print(val, stderr);
  utillib_json_value_destroy(val);
}

/**
 * \test ll1_builder_build_table_json
 * Outputs JSON of the builder itself.
 * Mainly its SETs.
 */
UTILLIB_TEST(ll1_builder_json) {
  struct utillib_json_value_t *val =
      utillib_ll1_builder_json_object_create(UT_FIXTURE);
  utillib_json_pretty_print(val, stderr);
  utillib_json_value_destroy(val);
}

UTILLIB_TEST_DEFINE(Utillib_LL1Builder) {
  UTILLIB_TEST_BEGIN(Utillib_LL1Builder)
  UTILLIB_TEST_RUN(ll1_builder_json)
  UTILLIB_TEST_RUN(ll1_builder_FIRST_correct)
  UTILLIB_TEST_RUN(ll1_builder_FOLLOW_correct)
  UTILLIB_TEST_RUN(ll1_builder_build_table_json)
  UTILLIB_TEST_RUN(ll1_builder_build_table_correct)
  UTILLIB_TEST_END(Utillib_LL1Builder)
  UTILLIB_TEST_FIXTURE(struct utillib_ll1_builder);
  UTILLIB_TEST_RETURN(Utillib_LL1Builder)
}
