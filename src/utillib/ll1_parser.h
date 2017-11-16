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
#ifndef UTILLIB_LL1_PARSER_H
#define UTILLIB_LL1_PARSER_H
#include "enum.h"
#include "rule.h"
#include "scanner.h"
#include "vector.h"
#include "vector2.h"

UTILLIB_ENUM_BEGIN(utillib_ll1_parser_error_kind)
UTILLIB_ENUM_ELEM_INIT(UT_LL1_ENORULE, 1)
UTILLIB_ENUM_ELEM(UT_LL1_EBADTOKEN)
UTILLIB_ENUM_ELEM(UT_LL1_EEMPTY)
UTILLIB_ENUM_END(utillib_ll1_parser_error_kind);

struct utillib_ll1_parser_error {
  int kind;
  struct utillib_symbol const * lookahead_symbol;
  struct utillib_symbol const * stack_top_symbol;
};

struct utillib_ll1_parser_op {
  void (* terminal_handler) (void *client_data, struct utillib_symbol const *, void const * semantic);
  void (* rule_handler) (void *client_data, struct utillib_rule const *);
  void (* error_handler)(void *client_data, void *input, struct utillib_ll1_parser_error const*);
}; 


struct utillib_ll1_factory {
  struct utillib_rule_index rule_index;
  struct utillib_vector2 table;
};

/**
 * \struct utillib_ll1_parser
 * Parser for the LL(1) grammar.
 * Handles the runtime parsing of input.
 */

struct utillib_ll1_parser {
  /* LL1 parsing related */
  struct utillib_vector symbol_stack;
  struct utillib_rule_index const *rule_index;
  struct utillib_vector2 const *table;

  /* Semantic actions related */
  void *client_data;
  struct utillib_ll1_parser_op const * op;
};

void utillib_ll1_factory_init(struct utillib_ll1_factory *self,
    struct utillib_symbol const *symbols,
    struct utillib_rule_literal const *rules);

void utillib_ll1_factory_destroy(struct utillib_ll1_factory *self);

void utillib_ll1_factory_parser_init(struct utillib_ll1_factory *self,
    struct utillib_ll1_parser *parser, void *client_data, 
    struct utillib_ll1_parser_op const *op);

void utillib_ll1_parser_init(struct utillib_ll1_parser *self,
                             struct utillib_rule_index const *rule_index,
                             struct utillib_vector2 const*table, 
                             void *client_data,
                             struct utillib_ll1_parser_op const *op);

void utillib_ll1_parser_destroy(struct utillib_ll1_parser *self);
bool utillib_ll1_parser_parse(struct utillib_ll1_parser *self, void *input,
                             struct utillib_scanner_op const *scanner);
#endif /* UTILLIB_LL1_PARSER_H */
