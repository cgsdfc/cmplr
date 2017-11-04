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
struct utillib_rule utillib_rule_null;

static struct utillib_rule * rule_index_rule_create (
    struct utillib_symbol const *symbols, 
    struct utillib_rule_literal const *rule_literal)
{
  struct utillib_rule * self=malloc(sizeof *self);
  utillib_vector_init(&self->RHS);
  self->LHS=&symbols[rule_literal->LHS_LIT];
  for (int const *pi=rule_literal->RHS_LIT; *pi != UT_SYM_NULL; ++pi) {
    utillib_vector_push_back(&self->RHS, (utillib_element_t) &symbols[*pi]);
  }
  return self;
}

static void rule_index_rule_destroy(struct utillib_rule *self)
{
  utillib_vector_destroy(&self->RHS);
  free(self);
}

void utillib_rule_index_init(struct utillib_rule_index * self, 
    struct utillib_symbol const * symbols,
    struct utillib_rule_literal const * rule_literals)
{
  utillib_vector_init(&self->rules);
  utillib_vector_init(&self->terminals);
  utillib_vector_init(&self->non_terminals);

  for (struct utillib_rule_literal const * rule_literal=rule_literals;
      rule_literal->LHS_LIT != UT_SYM_NULL; ++rule_literal) {
    struct utillib_rule * rule=rule_index_rule_create(symbols, rule_literal);
    utillib_vector_push_back(&self->rules, rule);
  }
  for (struct utillib_symbol const * symbol=symbols;
      UTILLIB_SYMBOL_NAME(symbol) != NULL; ++symbol) {
    if (UTILLIB_SYMBOL_KIND(symbol) == UT_SYMBOL_TERMINAL) {
      utillib_vector_push_back(&self->terminals, (utillib_element_t) symbol);
    } else {
      utillib_vector_push_back(&self->non_terminals, (utillib_element_t) symbol);
    }
  }
}

void utillib_rule_index_destroy(struct utillib_rule_index *self) {
  utillib_vector_destroy(&self->terminals);
  utillib_vector_destroy(&self->non_terminals);
  utillib_vector_destroy_owning(&self->rules, (utillib_destroy_func_t *)
      rule_index_rule_destroy);

}

