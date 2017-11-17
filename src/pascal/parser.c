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
#include "pascal_ll1_table.c"
#include "rules.h"
#include "symbols.h"
#include "ast.h"
#include <stdlib.h>
#include <utillib/pair.h>
#include <utillib/print.h> 
#include <utillib/ll1_builder.h>
#include <assert.h>

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

static void parser_terminal_handler(struct pascal_parser *self, 
    struct utillib_symbol const * terminal,
                                    void const *semantic) {
  struct pascal_ast_node * prev_node;
  struct utillib_vector * ast_nodes=&self->ast_nodes;
  union pascal_ast_union node;

  puts(terminal->name);
  switch (terminal->value) {
  case SYM_IDEN:
    prev_node=utillib_vector_back(ast_nodes);
    switch(prev_node->kind) {
      case PAS_AST_CONST:
        node.const_decl=prev_node->as_ptr;
        utillib_vector_push_back(&node.const_decl->items, semantic);
        break;
      case PAS_AST_VAR:
        node.var_decl=prev_node->as_ptr;
        utillib_vector_push_back(&node.var_decl->items, semantic);
        break;
      case PAS_AST_PROC:
        node.proc_decl=prev_node->as_ptr;
        node.proc_decl->name=semantic;
        break;
      case PAS_AST_ASSIGN:
        node.assign_stmt=prev_node->as_ptr;
        node.assign_stmt->LHS=semantic;
        break;
      default:
        assert("SYM_IDEN:prev_node: unimpled");
    }
  case SYM_UINT:
    prev_node=utillib_vector_back(ast_nodes);
    switch(prev_node->kind) { }
    puts(semantic);
    printf("%lu\n", (size_t) semantic);
    break;
  }
}

static void parser_rule_handler(struct pascal_parser *self, struct utillib_rule const *rule)
{
  struct utillib_symbol const * symbol=rule->LHS;
  struct utillib_vector * ast_nodes=&self->ast_nodes;
  int kind;

  switch (symbol->value) {
  case SYM_PROGRAM:
    kind=PAS_AST_PROGRAM;
    break;
  case SYM_PROC_DECL:
    kind=PAS_AST_PROC;
    break;
  case SYM_CONST_DECL:
    kind=PAS_AST_CONST;
    break;
  case SYM_WRITE_STMT:
    kind=PAS_AST_WRITE;
    break;
  case SYM_READ_STMT:
    kind=PAS_AST_READ;
    break;
  case SYM_ASS_STMT:
    kind=PAS_AST_ASSIGN;
    break;
  case SYM_LOOP_STMT:
    kind=PAS_AST_LOOP;
    break;
  }
  utillib_vector_push_back(ast_nodes, pascal_ast_node_create_empty(kind));
  struct utillib_json_value_t *val=utillib_rule_json_object_create(rule, 0);
  utillib_json_pretty_print(val, stdout);
  utillib_json_value_destroy(val);
}

static const struct utillib_ll1_parser_op pascal_parser_op = {
  .error_handler=(void*) parser_error_handler,
  .terminal_handler=(void*) parser_terminal_handler,
  .rule_handler=(void*) parser_rule_handler,
};

/*
 * Public interface
 */

void pascal_parser_factory_init(struct utillib_ll1_factory *self)
{
  utillib_ll1_factory_gen_init(self, ll1_parser_table, pascal_symbols, pascal_rules);
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

