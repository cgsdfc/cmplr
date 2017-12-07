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
#include "ll1_builder.h"
#include <assert.h>
#include <stdlib.h>

UTILLIB_ETAB_BEGIN(utillib_ll1_parser_error_kind)
UTILLIB_ETAB_ELEM_INIT(UT_LL1_ENORULE, "no rule can be expanded")
UTILLIB_ETAB_ELEM_INIT(UT_LL1_EBADTOKEN, "unexpected token")
UTILLIB_ETAB_ELEM_INIT(UT_LL1_EEMPTY, "symbol stack goes empty")
UTILLIB_ETAB_END(utillib_ll1_parser_error_kind);

#define ll1_parser_rule_handler_check(HANDLER)                                 \
  do {                                                                         \
    assert((HANDLER) && "Rule handler should not be NULL");                    \
  } while (0)

void utillib_ll1_factory_build_init(struct utillib_ll1_factory *self,
                                    struct utillib_symbol const *symbols,
                                    struct utillib_rule_literal const *rules) {
  struct utillib_ll1_builder builder;
  size_t errnum;
  utillib_rule_index_init(&self->rule_index, symbols, rules);
  utillib_ll1_builder_init(&builder, &self->rule_index);
  if ((errnum = utillib_ll1_builder_check(&builder))) {
    utillib_ll1_builder_print_errors(&builder);
    exit(errnum);
  }
  utillib_ll1_builder_build_table(&builder, &self->table);
  utillib_ll1_builder_destroy(&builder);
}

void utillib_ll1_factory_gen_init(struct utillib_ll1_factory *self,
                                  int const *gentable,
                                  struct utillib_symbol const *symbols,
                                  struct utillib_rule_literal const *rules) {
  utillib_rule_index_init(&self->rule_index, symbols, rules);
  utillib_rule_index_load_table(&self->rule_index, &self->table, gentable);
}

void utillib_ll1_factory_destroy(struct utillib_ll1_factory *self) {
  utillib_rule_index_destroy(&self->rule_index);
  utillib_vector2_destroy(&self->table);
}

void utillib_ll1_factory_parser_init(
    struct utillib_ll1_factory const *self, struct utillib_ll1_parser *parser,
    void *client_data, struct utillib_ll1_parser_callback const *callback) {
  utillib_ll1_parser_init(parser, &self->rule_index, &self->table, client_data,
                          callback);
}

/**
 * \function ll1_parser_expand_rule
 * Takes a rule, replaces the top of the symbol stack with
 * the RHS of the rule, the components of which are pushed
 * in reversed order onto the stack.
 * And calls the `rule_handler' of the popped-off symbol.
 */

static void ll1_parser_expand_rule(struct utillib_ll1_parser *self,
                                   struct utillib_rule const *rule) {
  if (rule == UTILLIB_RULE_EPS) {
    struct utillib_symbol const *LHS = utillib_vector_back(&self->symbol_stack);
    self->callback->terminal_handler(self->client_data, UTILLIB_SYMBOL_EPS,
                                     NULL);
    utillib_vector_pop_back(&self->symbol_stack);
    return;
  }
  self->callback->rule_handler(self->client_data, rule);
  utillib_vector_pop_back(&self->symbol_stack);
  struct utillib_vector const *RHS = &rule->RHS;
  size_t RHS_size = utillib_vector_size(RHS);
  for (int i = RHS_size - 1; i >= 0; --i) {
    struct utillib_symbol *symbol = utillib_vector_at(RHS, i);
    utillib_vector_push_back(&self->symbol_stack, symbol);
  }
}

/**
 * \function ll1_parser_symbol_stack_init
 * Initializes the symbol stack as [EOF, TOP].
 * This is required by LL1 parsing.
 */

static void ll1_parser_symbol_stack_init(struct utillib_ll1_parser *self) {
  utillib_vector_push_back(&self->symbol_stack, UTILLIB_SYMBOL_EOF);
  utillib_vector_push_back(&self->symbol_stack,
                           utillib_rule_index_top_symbol(self->rule_index));
}

/**
 * \function ll1_parser_table_lookup
 * Looks up one rule from the table.
 * \param input_symbol The lookahead terminal symbol. Used as
 * column index.
 * \param top_symbol The stack-top non terminal symbol. Used as
 * row index.
 * \return What is at [row][col] in the table. May be NULL.
 */
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
 * Initializes a LL1 parser. The parser works as follow.
 *
 * <para>Semantic actions: when one lookahead symbol matches
 * a terminal symbol on stack top, both symbols are shift away
 * and the `terminal_handler' is called to notify `client_data';
 * when the lookahead symbol causes the parser to expand the non
 * terminal symbol on stack top with a rule where it shows up as
 * LHS, the `rule_handler' at that rule is called.
 * </para>
 *
 * <para>Error handling: since the scanner it uses should handle scanning
 * error transparently, the types of errors the parser may encounter
 * are: 1. stack top terminal symbol mismatches the lookahead one. This
 * means unexpected token shows up. 2. Lookahead symbol leads to a NULL
 * rule, which in turn means unexpected token.
 * The current implementation simply skips that error token in both cases,
 * passes on the error information to `client_data' and continues as if error
 * did not happen. That is, if it is case 1, it pretends that the token matches,
 * and still shifts off the stack top symbol. If it is case 2, it discards the
 *error
 * token and keep the symbol stack unchanged.
 * </para>
 *
 *<para> In both cases, it is the client's responsibility to assemble readable
 *error messages
 * from the hints of parser. The parser simpily does the skipping and notifies
 *client.
 * In the future, more sophisticated recovery may let client make more decisions
 *based
 * on the knowledge of the grammar structure, .i.e., shipping to different token
 *according
 * to the involved rule.
 * <\para>
 *
 */
void utillib_ll1_parser_init(
    struct utillib_ll1_parser *self,
    struct utillib_rule_index const *rule_index,
    struct utillib_vector2 const *table, void *client_data,
    struct utillib_ll1_parser_callback const *callback) {
  utillib_vector_init(&self->symbol_stack);
  self->rule_index = rule_index;
  self->table = table;
  self->client_data = client_data;
  self->callback = callback;
}

static void
ll1_parser_error_init(struct utillib_ll1_parser_error *self, int kind,
                      struct utillib_symbol const *lookahead_symbol,
                      struct utillib_symbol const *stack_top_symbol) {
  self->lookahead_symbol = lookahead_symbol;
  self->stack_top_symbol = stack_top_symbol;
  self->kind = kind;
}

/*
 * Public interface
 */

/**
 * \function utillib_ll1_parser_destroy
 */
void utillib_ll1_parser_destroy(struct utillib_ll1_parser *self) {
  utillib_vector_destroy(&self->symbol_stack);
}

/**
 * \function utillib_ll1_parser_parse
 */
bool utillib_ll1_parser_parse(struct utillib_ll1_parser *self, void *input,
                              struct utillib_scanner_op const *scanner) {
  struct utillib_rule_index const *rule_index = self->rule_index;
  ll1_parser_symbol_stack_init(self);
  size_t input_symbol_value;
  struct utillib_symbol const *top_symbol;
  struct utillib_symbol const *input_symbol;
  struct utillib_rule const *rule;
  struct utillib_ll1_parser_error error;
  bool result = true;

  while (true) {
    input_symbol_value = scanner->lookahead(input);
    if (input_symbol_value) {
      input_symbol =
          utillib_rule_index_symbol_at(rule_index, input_symbol_value);
    } else {
      input_symbol = UTILLIB_SYMBOL_EOF;
    }
    /* When the symbol stack goes empty but input was not consumed */
    if (utillib_vector_empty(&self->symbol_stack)) {
      ll1_parser_error_init(&error, UT_LL1_EEMPTY, input_symbol,
                            UTILLIB_SYMBOL_ERR);
      self->callback->error_handler(self->client_data, input, &error);
      return result;
    }
    top_symbol = utillib_vector_back(&self->symbol_stack);
    if (top_symbol->kind == UT_SYMBOL_TERMINAL) {
      if (top_symbol->value == input_symbol_value) {
        self->callback->terminal_handler(self->client_data, input_symbol,
                                         input);
      } else {
        result = false;
        ll1_parser_error_init(&error, UT_LL1_EBADTOKEN, input_symbol,
                              top_symbol);
        self->callback->error_handler(self->client_data, input, &error);
      }
      utillib_vector_pop_back(&self->symbol_stack);
      scanner->shiftaway(input);
      if (input_symbol_value == UT_SYM_EOF) {
        return result;
      }
    } else { /* non-terminal */
      rule = ll1_parser_table_lookup(self, input_symbol, top_symbol);
      if (rule) {
        ll1_parser_expand_rule(self, rule);
      } else { /* rule == NULL */
        result = false;
        ll1_parser_error_init(&error, UT_LL1_ENORULE, input_symbol, top_symbol);
        self->callback->error_handler(self->client_data, input, &error);
        scanner->shiftaway(input);
        utillib_vector_pop_back(&self->symbol_stack);
      }
    }
  }
}
