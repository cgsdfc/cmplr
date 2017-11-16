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
#include "parser.h"
#include "scanner.h"
#include "pascal_ll1_table.c" // ll1_parser_table
#include "rules.h"            // pascal_rules
#include "symbols.h"          // pascal_symbols
#include <stdlib.h>
#include <utillib/pair.h>
#include <utillib/print.h> // utillib_error_printf
#include <utillib/ll1_builder.h>

/*
 * handlers for rule, termianl and error
 */

static void parser_error_handler(void *self, void * input,
    struct utillib_ll1_parser_error const*error) {
  size_t row;
  size_t col;
  pascal_scanner_getpos(input, &row, &col);

  utillib_error_printf("At %lu:%lu: ", row, col);
  switch (error->kind) {
    case UT_LL1_EBADTOKEN:
      utillib_error_printf("ERROR: expected `%s', got `%s'\n",
          error->stack_top_symbol->name,
          error->lookahead_symbol->name);
      return;
    case UT_LL1_ENORULE:
      utillib_error_printf("ERROR: unexpected `%s' at the end of `%s'\n",
        error->lookahead_symbol->name,
        error->stack_top_symbol->name);
      return;
    case UT_LL1_EEMPTY:
      utillib_error_printf("ERROR: unexpected `%s' at the end of input\n",
          error->lookahead_symbol->name);
    return;
  }
}

static void parser_terminal_handler(void *self, 
    struct utillib_symbol const * terminal,
                                    void const *semantic) {
  puts(terminal->name);
  switch (terminal->value) {
  case SYM_IDEN:
    puts(semantic);
    free(semantic);
    break;
  case SYM_UINT:
    printf("%lu\n", *(size_t const*) semantic);
    free(semantic);
    break;
  }
}

static void parser_rule_handler(void *self, struct utillib_rule const *rule)
{
  struct utillib_json_value_t *val=utillib_rule_json_object_create(rule, 0);
  utillib_json_pretty_print(val, stdout);
  utillib_json_value_destroy(val);
}

static const struct utillib_ll1_parser_op pascal_parser_op = {
  .error_handler=parser_error_handler,
  .terminal_handler=parser_terminal_handler,
  .rule_handler=parser_rule_handler,
};

/*
 * Public interface
 */

void pascal_parser_factory_init(struct utillib_ll1_factory *self)
{
  utillib_ll1_factory_init(self, pascal_symbols, pascal_rules);
}

void pascal_parser_init(struct pascal_parser *self, struct utillib_ll1_factory const *factory)
{
  utillib_vector_init(&self->ast_nodes);
  utillib_ll1_factory_parser_init(factory, &self->parser,
      self, &pascal_parser_op);
}

void pascal_parser_destroy(struct pascal_parser *self) {
  utillib_ll1_parser_destroy(&self->parser);
  utillib_vector_destroy(&self->ast_nodes);
}

bool pascal_parser_parse(struct pascal_parser *self, void *input,
                         struct utillib_scanner_op const *scanner) {
  return utillib_ll1_parser_parse(&self->parser, input, scanner);
}

