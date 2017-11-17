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
#ifndef PASCAL_PARSER_H
#define PASCAL_PARSER_H
#include <utillib/ll1_parser.h>

/**
 * \struct pascal_parser
 * Ast constructor.
 * Its parsing is implemented in terms of
 * `utillib_ll1_parser' and a list of semantic
 * actions as subroutines. Each time the ll1 parser
 * expands a LHS with its RHS, the action associated
 * with the rule in which the LHS appears is called.
 * And each time an input terminal symbol matches the
 * stack-top terminal symbol, the terminal handler is called.
 * For details about rule-handler and terminal-handler, please
 * refer to utillib/ll1_parser.h
 *
 */

struct pascal_parser {
  struct utillib_rule_index rule_index;
  struct utillib_vector2 table;
  struct utillib_ll1_parser parser;
  struct utillib_vector ast_nodes;
  struct pascal_parser_ast_node *program;
};

void pascal_parser_factory_init(struct utillib_ll1_factory *self);
void pascal_parser_init(struct pascal_parser *self,
                        struct utillib_ll1_factory const *factory);
void pascal_parser_destroy(struct pascal_parser *self);
bool pascal_parser_parse(struct pascal_parser *self, void *input,
                         struct utillib_scanner_op const *scanner);

#endif /* PASCAL_PARSER_H */
