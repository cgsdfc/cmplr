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
#include "pascal_ll1_table.c"
#include "rules.h"
#include "json_ast.h"
#include "scanner.h"
#include "symbols.h"
#include <assert.h>
#include <stdlib.h>
#include <utillib/ll1_builder.h>
#include <utillib/pair.h>
#include <utillib/print.h>

/*
 * handlers for rule, terminal and error
 */

static void parser_error_handler(void *self, void *input,
                                 struct utillib_ll1_parser_error const *error) {
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

/**
 * \function parser_peek_pop
 * Pops the stack into `popped'
 * and puts the new top of the stack into `top'.
 * A common operation.
 */
static void parser_peek_pop(struct utillib_vector *ast_nodes, 
    struct utillib_json_value ** popped,
    struct utillib_json_value ** top)
{
  *popped=utillib_vector_back(ast_nodes);
  utillib_vector_pop_back(ast_nodes);
  *top=utillib_vector_back(ast_nodes);
}

static void parser_add_var(struct utillib_vector *ast_nodes)
{
  struct utillib_json_value * iden;
  struct utillib_json_value * var_decl;
  struct utillib_json_value * items;

  iden=utillib_vector_back(ast_nodes);
  utillib_vector_pop_back(ast_nodes);
  var_decl=utillib_vector_back(ast_nodes);
  pascal_json_ast_add_item(var_decl, iden);
}

static void parser_add_const(struct utillib_vector *ast_nodes)
{
  struct utillib_json_value * iden;
  struct utillib_json_value * uint;
  struct utillib_json_value * pair;
  struct utillib_json_value * const_decl;

  uint=utillib_vector_back(ast_nodes);
  utillib_vector_pop_back(ast_nodes);
  iden=utillib_vector_back(ast_nodes);
  utillib_vector_pop_back(ast_nodes);
  pair=utillib_json_object_create_empty();
  utillib_json_object_push_back(pair, "iden", iden);
  utillib_json_object_push_back(pair, "uint", uint);
  const_decl=utillib_vector_back(ast_nodes);
  pascal_json_ast_add_item(const_decl, pair);
}

static void parser_comma_handler(struct utillib_vector *ast_nodes) {
  struct utillib_json_value * val=utillib_vector_back(ast_nodes);
  struct utillib_json_value * items;
  switch (val->kind) {
    case UT_JSON_STRING:
      parser_add_var(ast_nodes);
      return;
    case UT_JSON_SIZE_T:
      parser_add_const(ast_nodes);
      return;
    default:
      assert(false && "SYM_COMMA should follow uint or iden");
  }
}

static void parser_semi_handler(struct utillib_vector *ast_nodes)
{
  struct utillib_json_value *val;
  struct utillib_json_value *handle;

  val=utillib_vector_back(ast_nodes);
  utillib_vector_pop_back(ast_nodes);
  handle=utillib_vector_back(ast_nodes);
  int kind=pascal_json_ast_kind(handle);
  switch(kind) {
  case SYM_PROC_DECL:
    /* utillib_json_object_set(handle, "name", val); */
    return;
  }

  




}

static void parser_rp_handler(struct utillib_vector * ast_nodes)
{
  struct utillib_json_value * val;
  struct utillib_json_value * handle;
  parser_peek_pop(ast_nodes, &val, &handle);
  int kind=pascal_json_ast_kind(handle);
  switch (kind) {
    case SYM_WRITE_STMT:
      break;
  }





}

static void parser_terminal_handler(struct pascal_parser *self,
                                    struct utillib_symbol const *terminal,
                                    void const *semantic) {
  struct utillib_vector *ast_nodes=&self->ast_nodes;
  size_t uint;
  switch(terminal->value) {
    case SYM_IDEN:
      utillib_vector_push_back(ast_nodes,
          utillib_json_string_create(&semantic));
      return;
    case SYM_UINT:
      uint=(size_t) semantic;
      utillib_vector_push_back(ast_nodes,
          utillib_json_size_t_create(&uint));
      return;
  }
  switch(terminal->value) {
    case SYM_COMMA:
      parser_comma_handler(ast_nodes);
      return;
    case SYM_SEMI:
      parser_semi_handler(ast_nodes);
      return;
    case SYM_RP:
      parser_rp_handler(ast_nodes);
      return;
  }



}

static void parser_rule_handler(struct pascal_parser *self,
                                struct utillib_rule const *rule) {
  struct utillib_symbol const *symbol = rule->LHS;
  struct utillib_json_value * node=pascal_json_ast_create(symbol->value);
  switch (symbol->value) {
  case SYM_READ_STMT:
  case SYM_WRITE_STMT:
  case SYM_CONST_DECL:
  case SYM_COMP_STMT:
  case SYM_VAR_DECL:
    utillib_json_object_push_back(node, "items", 
        utillib_json_array_create_empty());
    break;
  case SYM_KW_PROC:
    break;
  case SYM_SUBPRG:
    break;
  case SYM_LOOP_STMT:
  case SYM_COND_STMT:
    break;



  }
  utillib_vector_push_back(&self->ast_nodes, node);
}

static const struct utillib_ll1_parser_op pascal_parser_op = {
    .error_handler = parser_error_handler,
    .terminal_handler = parser_terminal_handler,
    .rule_handler = parser_rule_handler,
};

/*
 * Public interface
 */

void pascal_parser_factory_init(struct utillib_ll1_factory *self) {
  utillib_ll1_factory_gen_init(self, ll1_parser_table, pascal_symbols,
                               pascal_rules);
}

void pascal_parser_init(struct pascal_parser *self,
                        struct utillib_ll1_factory const *factory) {
  utillib_vector_init(&self->ast_nodes);
  utillib_ll1_factory_parser_init(factory, &self->parser, self,
                                  &pascal_parser_op);
}

void pascal_parser_destroy(struct pascal_parser *self) {
  utillib_ll1_parser_destroy(&self->parser);
  utillib_vector_destroy(&self->ast_nodes);
}

bool pascal_parser_parse(struct pascal_parser *self, void *input,
                         struct utillib_scanner_op const *scanner) {
  return utillib_ll1_parser_parse(&self->parser, input, scanner);
}
