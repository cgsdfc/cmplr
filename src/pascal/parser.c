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
#include "pascal_ll1_table.c" // ll1_parser_table
#include "rules.h"            // pascal_rules
#include "symbols.h"          // pascal_symbols
#include <stdlib.h>
#include <utillib/pair.h>
#include <utillib/print.h> // utillib_error_printf
#include <utillib/ll1_builder.h>

/**
 * \function parser_ast_node_create
 * Creates an AST node wrapping another node
 * or terminal semantic.
 */
static struct pascal_parser_ast_node *parser_ast_node_create(int kind,
                                                             void const *data) {
  struct pascal_parser_ast_node *self = malloc(sizeof *self);
  self->kind = kind;
  switch (kind) {
  case PAS_AST_UINT:
    self->as_uint = *(size_t const *)data;
    return self;
  default:
    self->as_ptr = (void *)data;
    return self;
  }
}

/**
 * \function parser_const_decl_create
 * Creates a const declaration node.
 */
static struct pascal_parser_ast_node *parser_const_decl_create(void) {
  struct pascal_parser_const_decl *self = malloc(sizeof *self);
  utillib_vector_init(&self->items);
  return parser_ast_node_create(PAS_AST_CONST, self);
}

static struct pascal_parser_ast_node *parser_var_decl_create(void) {
  struct pascal_parser_var_decl *self = malloc(sizeof *self);
  utillib_vector_init(&self->items);
  return parser_ast_node_create(PAS_AST_VAR, self);
}

static struct pascal_parser_ast_node *parser_subprogram_create(void) {
  struct pascal_parser_subprogram *self = malloc(sizeof *self);
  return parser_ast_node_create(PAS_AST_SUBPROG, self);
}

static void parser_handle_program(struct pascal_parser *self) {
  utillib_error_printf("parser_handle_program!!!\n");
  self->program = parser_subprogram_create();
  utillib_vector_push_back(&self->ast_nodes, self->program);
}

static void parser_handle_subprogram(struct pascal_parser *self) {
  utillib_vector_push_back(&self->ast_nodes, parser_subprogram_create());
}

static void pascal_parser_handle_const_decl(struct pascal_parser *self) {
  struct pascal_parser_ast_node *const_decl = parser_const_decl_create();
  utillib_vector_push_back(&self->ast_nodes, const_decl);
}

static void pascal_parser_handle_var_decl(struct pascal_parser *self) {
  struct pascal_parser_ast_node *var_decl = parser_var_decl_create();
  utillib_vector_push_back(&self->ast_nodes, var_decl);
}

static void pascal_parser_procedure_add(struct pascal_parser_procedure *self,
                                        struct pascal_parser *parser, int code,
                                        void const *semantic) {}

static void
pascal_parser_subprogram_callback(struct pascal_parser_subprogram *self,
                                  struct pascal_parser *parser, int code,
                                  void const *semantic) {
}

static void
pascal_parser_const_decl_callback(struct pascal_parser_const_decl *self,
                                  struct pascal_parser *parser, int code,
                                  void const *semantic) {
  struct utillib_pair_t *pair;
  struct pascal_parser_subprogram *subprogram;
  switch (code) {
  case SYM_IDEN:
    pair = utillib_make_pair(semantic, NULL);
    utillib_vector_push_back(&self->items, pair);
    return;
  case SYM_UINT:
    pair = utillib_vector_back(&self->items);
    pair->up_second = semantic;
    return;
  case SYM_SEMI:
    utillib_vector_pop_back(&parser->ast_nodes);
    subprogram = utillib_vector_back(&parser->ast_nodes);
    subprogram->const_decl = self;
    return;
  case UT_SYM_EPS:
    subprogram = utillib_vector_back(&parser->ast_nodes);
    subprogram->const_decl = NULL;
    return;
  }
}

static void pascal_parser_var_callback(struct pascal_parser_var_decl *self,
                                       struct pascal_parser *parser, int code,
                                       void const *semantic) {
  struct pascal_parser_subprogram *subprogram;
  switch (code) {
  case SYM_IDEN:
    utillib_vector_push_back(&self->items, semantic);
    return;
  case SYM_SEMI:
    utillib_vector_pop_back(&parser->ast_nodes);
    subprogram = utillib_vector_back(&parser->ast_nodes);
    subprogram->var_decl = self;
    return;
  }
}

/*
 * handlers for rule, termianl and error
 */

static void parser_error_handler(void *self,
    struct utillib_ll1_parser_error const*error) {
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

}

static void parser_rule_handler(void *self, struct utillib_rule const *rule)
{
  utillib_rule_json_pretty_print(rule);
}

/*
 * Public interface
 */

void pascal_parser_init(struct pascal_parser *self) {
  utillib_rule_index_init(&self->rule_index, pascal_symbols, pascal_rules);
  /* Builts the table on the fly. */
  struct utillib_ll1_builder builder;
  utillib_ll1_builder_init(&builder, &self->rule_index);
  utillib_ll1_builder_check(&builder);
  utillib_ll1_builder_build_table(&builder, &self->table);
  utillib_ll1_builder_destroy(&builder);

  /* utillib_rule_index_load_table(&self->rule_index, &self->table, */
  /*                               ll1_parser_table); */
  utillib_vector_init(&self->ast_nodes);
  utillib_ll1_parser_init(&self->parser, &self->rule_index, &self->table, self,
                          parser_terminal_handler,
                          parser_rule_handler,
                          parser_error_handler);
}

void pascal_parser_destroy(struct pascal_parser *self) {
  utillib_rule_index_destroy(&self->rule_index);
  utillib_vector2_destroy(&self->table);
  utillib_ll1_parser_destroy(&self->parser);
}

bool pascal_parser_parse(struct pascal_parser *self, void *input,
                         struct utillib_scanner_op const *scanner) {
  return utillib_ll1_parser_parse(&self->parser, input, scanner);
}

