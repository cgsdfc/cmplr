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
#include <limits.h> // ULONG_MAX
#include <stdlib.h> // malloc
#include <assert.h>

/**
 * \variable utillib_rule_null
 * Represents any rule whose right hand side is
 * merely an `epsilon'.
 * Thus, the left hand side is out of concern or
 * can be deducted from context.
 */
struct utillib_rule utillib_rule_null;

/**
 * \function rule_index_rule_create
 * Creates `utillib_rule' from `utillib_rule_literal'.
 * If the rule literal is of form `A := epsilon', 
 * \param symbols The symbol table to look up.
 * \param rule_literal The literal under concern.
 */
static struct utillib_rule *
rule_index_rule_create(struct utillib_symbol const *symbols,
    size_t rule_id,
    struct utillib_rule_literal const *rule_literal) 
{
  int  const* RHS_LIT=rule_literal->RHS_LIT;
  struct utillib_rule *self = malloc(sizeof *self);
  utillib_vector_init(&self->RHS);
  int LHS_LIT = rule_literal->LHS_LIT;
  self->LHS = &symbols[LHS_LIT];
  self->id=rule_id;
  for (; *RHS_LIT != UT_SYM_NULL; ++RHS_LIT) {
    struct utillib_symbol const *symbol;
    if (*RHS_LIT == UT_SYM_EPS)
      symbol = UTILLIB_SYMBOL_EPS;
    else
      symbol = &symbols[*RHS_LIT];
    utillib_vector_push_back(&self->RHS, (utillib_element_t) symbol);
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

  for (struct utillib_rule_literal const *rule_literal = rule_literals;
       rule_literal->LHS_LIT != UT_SYM_NULL; ++rule_literal) {
    /* Rule index starts form ONE */
    size_t rule_id=1+rule_literal-rule_literals;
    struct utillib_rule *rule = rule_index_rule_create(symbols, rule_id, rule_literal);
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
    utillib_vector_push_back(pvector, (utillib_element_t)symbol);
  }
  self->non_terminals_size=utillib_vector_size(&self->non_terminals);
  self->terminals_size=utillib_vector_size(&self->terminals);
  self->rules_size=utillib_vector_size(&self->rules);
  self->symbols_size=self->non_terminals_size+self->terminals_size+1;
}

/**
 * \function utillib_rule_index_destroy
 */
void utillib_rule_index_destroy(struct utillib_rule_index *self) {
  utillib_vector_destroy(&self->terminals);
  utillib_vector_destroy(&self->non_terminals);
  utillib_vector_destroy_owning(
      &self->rules, (utillib_destroy_func_t *)rule_index_rule_destroy);
}

/**
 * \function utillib_rule_index_symbol_index
 * Converts the value in `symbol' to an unsigned
 * integer that is suitable for indexing an array
 * from 0 to number-of-symbol, exclusively.
 * \param symbol From which the index is computed.
 * its value should be non negative.
 * 
 */
#define rule_index_check_symbol_value(symbol) do {\
  assert (utillib_symbol_value(symbol) > 0 && "Value of symbol should be non negative");\
} while(0)

size_t utillib_rule_index_symbol_index(struct utillib_rule_index const *self,
    struct utillib_symbol const *symbol)
{
  rule_index_check_symbol_value(symbol);
  /* Checked */
  size_t value=utillib_symbol_value(symbol);
  switch (utillib_symbol_kind(symbol)) {
  case UT_SYMBOL_TERMINAL:
    return value - self->min_terminal;
  case UT_SYMBOL_NON_TERMINAL:
    return value - self->min_non_terminal;
  }
}

/**
 * \function utillib_rule_index_top_symbol
 * For any symbol defined in the `begin-elem-end' way,
 * identifies the first non terminal symbol as the 
 * TOP symbol of the input grammar.
 */
#define rule_index_check_non_terminal_non_empty(self) do {\
  assert (self->non_terminals_size > 0 && "Number of non terminal symbols should be at least 1");\
} while (0)

struct utillib_symbol *
utillib_rule_index_top_symbol(struct utillib_rule_index const *self) {
  rule_index_check_non_terminal_non_empty(self);
  return utillib_vector_front(&self->non_terminals);
}

/**
 * Implements JSON format interface.
 */
static utillib_json_value_t *
rule_RHS_json_array_create_from_vector(void *base, size_t offset) {
  return utillib_json_array_create_from_vector(
      base,  utillib_symbol_json_object_create);
}

UTILLIB_JSON_OBJECT_FIELD_BEGIN(Rule_Fields)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_rule, "left-hand-side", LHS,
                               utillib_symbol_json_object_pointer_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_rule, "right-hand-side", RHS,
                               rule_RHS_json_array_create_from_vector)
UTILLIB_JSON_OBJECT_FIELD_END(Rule_Fields)

utillib_json_value_t *utillib_rule_json_object_create(void *base,
                                                      size_t offset) {
  if (base == &utillib_rule_null) {
    return utillib_json_null_create();
  }
  return utillib_json_object_create(base, offset, Rule_Fields);
}

static utillib_json_value_t *
rule_index_rule_json_array_create_from_vector(void *base, size_t offset) {
  return utillib_json_array_create_from_vector(base, 
                                               utillib_rule_json_object_create);
}

UTILLIB_JSON_OBJECT_FIELD_BEGIN(RuleIndex_Fields)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_rule_index, "min-terminals-value",
                               min_terminal, utillib_json_size_t_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_rule_index,
                               "min-non-terminals-value", min_non_terminal,
                               utillib_json_size_t_create)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_rule_index, "terminal-symbols",
                               terminals,
                               utillib_symbol_json_array_create_from_vector)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_rule_index,
                               "non-terminal-symbols", non_terminals,
                               utillib_symbol_json_array_create_from_vector)
UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_rule_index, "rules", rules,
                               rule_index_rule_json_array_create_from_vector)
UTILLIB_JSON_OBJECT_FIELD_END(RuleIndex_Fields)

utillib_json_value_t *utillib_rule_index_json_object_create(void *base,
                                                            size_t offset) {
  return utillib_json_object_create(base, offset, RuleIndex_Fields);
}
