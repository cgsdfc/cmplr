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
#include <stdlib.h> // malloc
#include <limits.h> // ULONG_MAX

struct utillib_rule utillib_rule_null;

static struct utillib_rule * rule_index_rule_create (
    struct utillib_symbol const *symbols, 
    struct utillib_rule_literal const *rule_literal)
{
  struct utillib_rule * self=malloc(sizeof *self);
  utillib_vector_init(&self->RHS);
  int LHS_LIT=rule_literal->LHS_LIT;
  self->LHS=&symbols[LHS_LIT];
  for (int const *pi=rule_literal->RHS_LIT; *pi != UT_SYM_NULL; ++pi) {
    struct utillib_symbol const * symbol;
    if (*pi == UT_SYM_EPS)
      symbol=UTILLIB_SYMBOL_EPS;
    else
      symbol=&symbols[*pi];
    utillib_vector_push_back(&self->RHS, (utillib_element_t) symbol);
  }
  return self;
}

void utillib_rule_index_init(struct utillib_rule_index * self, 
    struct utillib_symbol const * symbols,
    struct utillib_rule_literal const * rule_literals)
{
  int value;
  size_t * pvalue;
  struct utillib_vector * pvector;

  utillib_vector_init(&self->rules);
  utillib_vector_init(&self->terminals);
  utillib_vector_init(&self->non_terminals);
  self->min_non_terminal=ULONG_MAX;
  self->min_terminal=ULONG_MAX;

  for (struct utillib_rule_literal const * rule_literal=rule_literals;
      rule_literal->LHS_LIT != UT_SYM_NULL; ++rule_literal) {
    struct utillib_rule * rule=rule_index_rule_create(symbols, rule_literal);
    utillib_vector_push_back(&self->rules, rule);
  }
  /* Since `EOF' always takes the "zero" place, we start from "1" */
  for (struct utillib_symbol const * symbol=symbols+1;
      utillib_symbol_name(symbol) != NULL; ++symbol) {
      value=utillib_symbol_value(symbol);
    if (utillib_symbol_kind(symbol) == UT_SYMBOL_TERMINAL) {
      pvalue=&self->min_terminal;
      pvector=&self->terminals;
    } else {
      pvalue=&self->min_non_terminal;
      pvector=&self->non_terminals;
    }
    if (value < *pvalue)
      *pvalue=value;
    utillib_vector_push_back(pvector, (utillib_element_t) symbol);
  }
}

static void rule_index_rule_destroy(struct utillib_rule *self)
{
  utillib_vector_destroy(&self->RHS);
  free(self);
}

void utillib_rule_index_destroy(struct utillib_rule_index *self) {
  utillib_vector_destroy(&self->terminals);
  utillib_vector_destroy(&self->non_terminals);
  utillib_vector_destroy_owning(&self->rules, (utillib_destroy_func_t *)
      rule_index_rule_destroy);

}

size_t utillib_rule_index_non_terminal_index(struct utillib_rule_index const*self, size_t value)
{
  return value-self->min_non_terminal;
}

size_t utillib_rule_index_terminal_index(struct utillib_rule_index const*self, size_t value)
{
  return value-self->min_terminal;
}

/**
 * Implements JSON format interface.
 */
static utillib_json_value_t * rule_RHS_json_array_create_from_vector(void *base, size_t offset)
{
  return utillib_json_array_create_from_vector(base, offset, utillib_symbol_json_object_create);
}

UTILLIB_JSON_OBJECT_FIELD_BEGIN(Rule_Fields)
  UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_rule, "left-hand-side",LHS,  utillib_symbol_json_object_pointer_create)
  UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_rule, "right-hand-side", RHS, rule_RHS_json_array_create_from_vector)
UTILLIB_JSON_OBJECT_FIELD_END(Rule_Fields)

utillib_json_value_t * utillib_rule_json_object_create(void * base, size_t offset)
{
  return utillib_json_object_create(base, offset, Rule_Fields);
}

static utillib_json_value_t * rule_index_rule_json_array_create_from_vector(void *base, size_t offset)
{
  return utillib_json_array_create_from_vector(base, offset, utillib_rule_json_object_create);
}

UTILLIB_JSON_OBJECT_FIELD_BEGIN(RuleIndex_Fields)
  UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_rule_index, "min-terminals-value", min_terminal, utillib_json_size_t_create)
  UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_rule_index, "min-non-terminals-value", min_non_terminal, utillib_json_size_t_create)
  UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_rule_index, "terminal-symbols", terminals, utillib_symbol_json_array_create_from_vector)
  UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_rule_index, "non-terminal-symbols", non_terminals, utillib_symbol_json_array_create_from_vector)
  UTILLIB_JSON_OBJECT_FIELD_ELEM(struct utillib_rule_index, "rules", rules,  rule_index_rule_json_array_create_from_vector)
UTILLIB_JSON_OBJECT_FIELD_END(RuleIndex_Fields)

utillib_json_value_t * utillib_rule_index_json_object_create(void *base, size_t offset)
{
  return utillib_json_object_create(base, offset, RuleIndex_Fields);
}

