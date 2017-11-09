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
#include "ll1_parser.h"
#include <stdlib.h>

/**
 * \function ll1_parser_context_create
 */
static struct utillib_ll1_parser_context *
ll1_parser_context_create(size_t RHS_count, size_t rule_id) {
  struct utillib_ll1_parser_context *self = malloc(sizeof *self);
  self->RHS_count = RHS_count;
  self->rule_id = rule_id;
  return self;
}

/**
 * \function ll1_parser_context_destroy
 */
static void
ll1_parser_context_destroy(struct utillib_ll1_parser_context *self) {
  free(self);
}

/**
 * \function ll1_parser_match_symbol
 */
static void ll1_parser_match_symbol(struct utillib_ll1_parser *self) {
  utillib_vector_pop_back(&self->symbol_stack);
  /* struct utillib_ll1_parser_context *context = */
  /*     utillib_vector_back(&self->context); */
  /* --context->RHS_count; */
  /* if (context->RHS_count == 0) { */
  /*   printf("Reduce rule `%lu'\n", 1 + context->rule_id); */
  /*   utillib_ll1_parser_callback_t *callback =
   * self->callbacks[context->rule_id]; */
  /*   callback(self->client_data, self); */
  /*   ll1_parser_context_destroy(context); */
  /*   utillib_vector_pop_back(&self->context); */
  /* } */
}

/**
 * \function ll1_parser_start_deduct
 */
static void ll1_parser_start_deduct(struct utillib_ll1_parser *self,
                                    struct utillib_rule const *rule) {
  if (rule == UTILLIB_RULE_EPS) {
    utillib_vector_push_back(&self->tree_stack, NULL);
    utillib_vector_pop_back(&self->symbol_stack);
    return;
  }
  printf("Deduct Rule `%lu'\n", 1 + utillib_rule_id(rule));
  struct utillib_symbol const *LHS = utillib_rule_lhs(rule);
  struct utillib_vector const *RHS = utillib_rule_rhs(rule);
  size_t RHS_size = utillib_vector_size(RHS);
  /* utillib_vector_push_back( */
  /*     &self->context, */
  /*     ll1_parser_context_create(RHS_size, utillib_rule_id(rule))); */
  utillib_vector_pop_back(&self->symbol_stack);
  for (int i = RHS_size - 1; i >= 0; --i) {
    struct utillib_symbol *symbol = utillib_vector_at(RHS, i);
    utillib_vector_push_back(&self->symbol_stack, symbol);
  }
}

static void ll1_parser_symbol_stack_init(struct utillib_ll1_parser *self) {
  utillib_vector_push_back(&self->symbol_stack, UTILLIB_SYMBOL_EOF);
  utillib_vector_push_back(&self->symbol_stack,
                           utillib_rule_index_top_symbol(self->rule_index));
}

static struct utillib_rule *
ll1_parser_table_lookup(struct utillib_ll1_parser *self,
                        struct utillib_symbol const *input_symbol,
                        struct utillib_symbol const *top_symbol) {
  struct utillib_rule_index const *rule_index = self->rule_index;
  size_t input_symbol_index =
      utillib_rule_index_symbol_index(rule_index, input_symbol);
  size_t top_symbol_index =
      utillib_rule_index_symbol_index(rule_index, top_symbol);
  struct utillib_rule *rule =
      utillib_vector2_at(self->table, top_symbol_index, input_symbol_index);
  return rule;
}

/**
 * \function utillib_ll1_parser_init
 */
void utillib_ll1_parser_init(struct utillib_ll1_parser *self,
                             struct utillib_rule_index const *rule_index,
                             struct utillib_vector2 *table, void *client_data,
                             const utillib_ll1_parser_callback_t **callbacks) {
  utillib_vector_init(&self->symbol_stack);
  utillib_vector_init(&self->error_stack);
  utillib_vector_init(&self->tree_stack);
  utillib_vector_init(&self->context);
  utillib_vector_init(&self->rule_seq);
  self->rule_index = rule_index;
  self->table = table;
  self->callbacks = callbacks;
  self->client_data = client_data;
}

/**
 * \function utillib_ll1_parser_destroy
 */
void utillib_ll1_parser_destroy(struct utillib_ll1_parser *self) {
  utillib_vector_destroy(&self->error_stack);
  utillib_vector_destroy(&self->symbol_stack);
  utillib_vector_destroy(&self->tree_stack);
  utillib_vector_destroy(&self->rule_seq);
  utillib_vector_destroy_owning(&self->context,
                                (void *)ll1_parser_context_destroy);
  utillib_vector2_destroy(self->table);
}

/**
 * \function utillib_ll1_parser_parse
 */
int utillib_ll1_parser_parse(struct utillib_ll1_parser *self, void *input,
                             struct utillib_scanner_op const *scanner) {
  struct utillib_rule_index const *rule_index = self->rule_index;
  ll1_parser_symbol_stack_init(self);

  while (true) {
    size_t isym = scanner->lookahead(input);
    struct utillib_symbol const *isymbol =
        utillib_rule_index_symbol_at(rule_index, isym);
    struct utillib_symbol const *top_sym =
        utillib_vector_back(&self->symbol_stack);
    size_t top_val = utillib_symbol_value(top_sym);
    size_t top_kind = utillib_symbol_kind(top_sym);
    if (top_kind == UT_SYMBOL_TERMINAL) {
      if (top_val == UT_SYM_EOF && isym == top_val) {
        return UT_LL1_PARSER_OK;
      }
      if (top_val == isym) {
        printf("Match Symbol `%s'\n", utillib_symbol_name(isymbol));
        utillib_vector_pop_back(&self->symbol_stack);
        scanner->shiftaway(input);
        continue;
      }
      return UT_LL1_PARSER_ERR;
    } else { /* non-terminal */
      struct utillib_rule const *rule =
          ll1_parser_table_lookup(self, isymbol, top_sym);
      if (!rule) {
        return UT_LL1_PARSER_ERR;
      }
      ll1_parser_start_deduct(self, rule);
    }
  }
}
