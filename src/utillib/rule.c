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
#include "rule.h"
#include <assert.h>
#include <limits.h> // ULONG_MAX
#include <stdlib.h> // malloc

/**
 * \variable utillib_rule_eps
 * Represents any rule whose right hand side is
 * merely an `epsilon'.
 * Thus, the left hand side is out of concern or
 * can be deducted from context.
 */
struct utillib_rule utillib_rule_eps = {.id = UT_RULE_EPS};

/*
 * Ensures sanity of LHS
 */
#define rule_check_lhs_non_terminal(symbol)                                    \
  assert(utillib_symbol_kind(symbol) == UT_SYMBOL_NON_TERMINAL &&              \
         "Symbol on the left hand side of a rule must be non terminal")

static struct utillib_rule *
rule_index_rule_create(size_t rule_id, struct utillib_symbol const *LHS) {
  rule_check_lhs_non_terminal(LHS);
  struct utillib_rule *self = malloc(sizeof *self);
  self->id = rule_id;
  self->LHS = LHS;
  utillib_vector_init(&self->RHS);
  return self;
}

/**
 * \function rule_index_rule_create
 * Creates `utillib_rule' from `utillib_rule_literal'.
 * If the rule literal is of form `A := epsilon',
 * \param symbols The symbol table to look up.
 * \param rule_literal The literal under concern.
 */
static struct utillib_rule *rule_index_rule_create_from_literal(
    struct utillib_symbol const *symbols, size_t rule_id,
    struct utillib_rule_literal const *rule_literal) {
  int const *RHS_LIT = rule_literal->RHS_LIT;
  int LHS_LIT = rule_literal->LHS_LIT;
  struct utillib_rule *self =
      rule_index_rule_create(rule_id, &symbols[LHS_LIT]);

  for (; *RHS_LIT != UT_SYM_NULL; ++RHS_LIT) {
    struct utillib_symbol const *symbol;
    if (*RHS_LIT == UT_SYM_EPS)
      symbol = UTILLIB_SYMBOL_EPS;
    else
      symbol = &symbols[*RHS_LIT];
    utillib_vector_push_back(&self->RHS, (void const *)symbol);
  }
  return self;
}

static void rule_index_rule_destroy(struct utillib_rule *self) {
  utillib_vector_destroy(&self->RHS);
  free(self);
}

/**
 * \function utillib_rule_index_init
 * Initilizes self.
 */
void utillib_rule_index_init(struct utillib_rule_index *self,
                             struct utillib_symbol const *symbols,
                             struct utillib_rule_literal const *rule_literals) {
  int value;
  size_t *pvalue;
  struct utillib_vector *pvector;

  utillib_vector_init(&self->rules);
  utillib_vector_init(&self->terminals);
  utillib_vector_init(&self->non_terminals);
  self->min_non_terminal = ULONG_MAX;
  self->min_terminal = ULONG_MAX;
  self->symbols = symbols;
  self->LHS_index = NULL;

  for (struct utillib_rule_literal const *rule_literal = rule_literals;
       rule_literal->LHS_LIT != UT_SYM_NULL; ++rule_literal) {
    /* Rule index starts form ONE */
    size_t rule_id = rule_literal - rule_literals;
    struct utillib_rule *rule =
        rule_index_rule_create_from_literal(symbols, rule_id, rule_literal);
    utillib_vector_push_back(&self->rules, rule);
  }
  /* Since `EOF' always takes the "zero" place, we start from "1" */
  for (struct utillib_symbol const *symbol = symbols + 1;
       utillib_symbol_name(symbol) != NULL; ++symbol) {
    value = utillib_symbol_value(symbol);
    if (utillib_symbol_kind(symbol) == UT_SYMBOL_TERMINAL) {
      pvalue = &self->min_terminal;
      pvector = &self->terminals;
    } else {
      pvalue = &self->min_non_terminal;
      pvector = &self->non_terminals;
    }
    if (value < *pvalue)
      *pvalue = value;
    utillib_vector_push_back(pvector, symbol);
  }
  /* counts EOF */
  self->non_terminals_size = utillib_vector_size(&self->non_terminals);
  self->terminals_size = utillib_vector_size(&self->terminals) + 1;
  self->symbols_size = self->non_terminals_size + self->terminals_size;
}

static void rule_index_LHS_index_destroy(struct utillib_rule_index *self) {
  if (!self->LHS_index)
    return;
  for (int i = 0; i < self->non_terminals_size; ++i) {
    utillib_vector_destroy(&self->LHS_index[i]);
  }
  free(self->LHS_index);
}

/**
 * \function utillib_rule_index_destroy
 */
void utillib_rule_index_destroy(struct utillib_rule_index *self) {
  utillib_vector_destroy(&self->terminals);
  utillib_vector_destroy(&self->non_terminals);
  utillib_vector_destroy_owning(&self->rules, (void *)rule_index_rule_destroy);
  rule_index_LHS_index_destroy(self);
}

/**
 * \function utillib_rule_index_symbol_index
 * Converts the value in `symbol' to an unsigned
 * integer that is suitable for indexing an array
 * from 0 to number-of-symbol, exclusively.
 * \param symbol From which the index is computed.
 * its value should be non negative.
 * Notes that it preserves the zero index for special
 * symbol `eof'.
 */
#define rule_index_check_symbol_value(symbol)                                  \
  do {                                                                         \
    assert(utillib_symbol_value(symbol) >= 0 &&                                \
           "Value of symbol should be non negative");                          \
  } while (0)

size_t utillib_rule_index_symbol_index(struct utillib_rule_index const *self,
                                       struct utillib_symbol const *symbol) {
  rule_index_check_symbol_value(symbol);
  size_t value = utillib_symbol_value(symbol);
  if (value == UT_SYM_EOF)
    return UT_SYM_EOF;
  /* Checked */
  switch (utillib_symbol_kind(symbol)) {
  case UT_SYMBOL_TERMINAL:
    /* EOF takes zero */
    return value - self->min_terminal + 1;
  case UT_SYMBOL_NON_TERMINAL:
    return value - self->min_non_terminal;
  }
}

struct utillib_symbol const *
utillib_rule_index_symbol_at(struct utillib_rule_index const *self,
                             size_t symbol_id) {
  assert(symbol_id < self->symbols_size && "Symbol index out of range");
  return &self->symbols[symbol_id];
}

/**
 * \function utillib_rule_index_top_symbol
 * For any symbol defined in the `begin-elem-end' way,
 * identifies the first non terminal symbol as the
 * TOP symbol of the input grammar.
 */
#define rule_index_check_non_terminal_non_empty(self)                          \
  do {                                                                         \
    assert(self->non_terminals_size > 0 &&                                     \
           "Number of non terminal symbols should be at least 1");             \
  } while (0)

struct utillib_symbol *
utillib_rule_index_top_symbol(struct utillib_rule_index const *self) {
  rule_index_check_non_terminal_non_empty(self);
  return utillib_vector_front(&self->non_terminals);
}

size_t utillib_rule_index_rules_size(struct utillib_rule_index const *self) {
  return utillib_vector_size(&self->rules);
}

/**
 * \function utillib_rule_index_build_LHS_index
 * Builds an index for all the non terminal symbols which
 * allows traversal of the rules with the same LHS faster.
 */
void utillib_rule_index_build_LHS_index(struct utillib_rule_index *self) {
  if (self->LHS_index) {
    return;
  }
  size_t non_terminals_size = self->non_terminals_size;
  self->LHS_index = calloc(sizeof self->LHS_index[0], non_terminals_size);
  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, rule, &self->rules) {
    struct utillib_symbol const *LHS = rule->LHS;
    size_t pos = utillib_rule_index_symbol_index(self, LHS);
    assert(pos < non_terminals_size &&
           "Non terminal symbol index out of range");
    struct utillib_vector *same_LHS_rule = &self->LHS_index[pos];
    utillib_vector_push_back(same_LHS_rule, rule);
  }
}

void utillib_rule_index_load_table(struct utillib_rule_index const *self,
                                   struct utillib_vector2 *table,
                                   int const *array) {
  size_t non_terminals_size = self->non_terminals_size;
  size_t terminals_size = self->terminals_size;
  utillib_vector2_init(table, non_terminals_size, terminals_size);

  for (int i = 0; i < non_terminals_size; ++i) {
    for (int j = 0; j < terminals_size; ++j) {
      int rule_id = array[i * terminals_size + j];
      struct utillib_rule const *rule;
      if (rule_id == UT_RULE_NULL) {
        rule = NULL;
      } else if (rule_id == UT_RULE_EPS) {
        rule = UTILLIB_RULE_EPS;
      } else {
        rule = utillib_vector_at(&self->rules, rule_id);
      }
      utillib_vector2_set(table, i, j, rule);
    }
  }
}

/**
 * Implements JSON format interface.
 */

struct utillib_json_value *
utillib_rule_json_object_create(struct utillib_rule const *self) {
  if (self == UTILLIB_RULE_EPS) {
    static const char * null_str="A := epsilon";
    return utillib_json_string_create(&null_str);
  }
  struct utillib_json_value *object = utillib_json_object_create_empty();
  struct utillib_json_value *array = utillib_json_array_create_empty();

  utillib_json_object_push_back(object, "LHS",
                                utillib_symbol_json_string_create(self->LHS));
  UTILLIB_VECTOR_FOREACH(struct utillib_symbol const *, symbol, &self->RHS) {
    utillib_json_array_push_back(array,
                                 utillib_symbol_json_string_create(symbol));
  }
  utillib_json_object_push_back(object, "RHS", array);
  return object;
}

struct utillib_json_value *
utillib_rule_index_json_object_create(struct utillib_rule_index const *self) {
  struct utillib_json_value * object=utillib_json_object_create_empty();
  utillib_json_object_push_back(object, "min-termianls-value",
      utillib_json_size_t_create(&self->min_terminal));
  utillib_json_object_push_back(object, "min-non-termianls-value",
      utillib_json_size_t_create(&self->min_non_terminal));
  utillib_json_object_push_back(object, "termianl-symbols",
      utillib_vector_json_array_create(&self->terminals,
        utillib_symbol_json_object_create));
  utillib_json_object_push_back(object, "non-terminal-symbols",
      utillib_vector_json_array_create(&self->non_terminals,
        utillib_symbol_json_object_create));
  utillib_json_object_push_back(object, "rules",
      utillib_vector_json_array_create(&self->rules,
        utillib_rule_json_object_create));
  return object;
}

/**
 * \function utillib_rule_json_pretty_print
 * Convenient helper to print a rule to stderr
 * Mainly for ease of debug.
 */
void utillib_rule_json_pretty_print(struct utillib_rule const *self) {
  struct utillib_json_value *val =
      utillib_rule_json_object_create(self);
  utillib_json_pretty_print(val, stderr);
  utillib_json_value_destroy(val);
}
