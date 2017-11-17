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

/**
 * \file utillib/ll1_parser.c
 * This file contains implementation of ll1_parser
 * and ll1_factory.
 * As a table-based parser, ll1_parser needs the knowledge
 * of the grammar, but it must not store such meta information.
 * Therefore, ll1_factory is meant to hold such knowledge
 * and grands it to a parser.
 * An ll1_parser is a combination of the grammar and the semantic action
 * from the client. It therefore knows nothing about the both but just
 * does the parsing and calls client's handlers.
 * An ll1_factory can be shared among all the parsers that parse the same
 * grammar but may have different semantic actions.
 * This kind of sharing is enabled by calling `utillib_ll1_factory_parser_init'
 * to initialize a parser from a factory instead of calling
 * `utillib_ll1_parser_init'
 * directly.
 */

/**
 * \enum utillib_ll1_parser_error_kind
 * The errors an ll1_parser may seen.
 * \value UT_LL1_ENORULE The parser run into a NULL entry of rule.
 * \value UT_LL1_EBADTOKEN The input token failed to match the token at
 * the top of the parser's stack.
 * \value UT_LL1_EEMPTY The parser's stack becomes empty while input does not
 * reach eof.
 */
UTILLIB_ENUM_BEGIN(utillib_ll1_parser_error_kind)
UTILLIB_ENUM_ELEM_INIT(UT_LL1_ENORULE, 1)
UTILLIB_ENUM_ELEM(UT_LL1_EBADTOKEN)
UTILLIB_ENUM_ELEM(UT_LL1_EEMPTY)
UTILLIB_ENUM_END(utillib_ll1_parser_error_kind);

/**
 * \struct utillib_ll1_parser_error
 * The struct that lets client know the details of the error.
 * \feild lookahead_symbol The input symbol lookaheaded when error
 * occurred.
 * \feild stack_top_symbol The symbol on the top parser stack when
 * error occurred.
 */
struct utillib_ll1_parser_error {
  int kind;
  struct utillib_symbol const *lookahead_symbol;
  struct utillib_symbol const *stack_top_symbol;
};

/**
 * \struct utillib_ll1_parser_op
 * Function table that defines the semantic behaviour
 * of the parser.
 * \field terminal_handler Called when an input token matches the token on top.
 * Passes in the symbol just matched and the semantic of it return by
 * `utillib_scanner_op->semantic'.
 * \field rule_handler Called when the parser decides to replace the top symbol
 * with the rhs symbols of a certain rule. Passes in the rule that defines this
 * replacement.
 * \field error_handler Called when the parser encounters an error. Passes in
 * the
 * scanner which has more knowledge about the input and therefore called `input'
 * and the error.
 */
struct utillib_ll1_parser_op {
  void (*terminal_handler)(void *client_data, struct utillib_symbol const *,
                           void const *semantic);
  void (*rule_handler)(void *client_data, struct utillib_rule const *);
  void (*error_handler)(void *client_data, void *input,
                        struct utillib_ll1_parser_error const *);
};

/**
 * \struct utillib_ll1_factory
 * Holds the rules, symbols and table
 * of a LL1 grammar.
 */
struct utillib_ll1_factory {
  struct utillib_rule_index rule_index;
  struct utillib_vector2 table;
};

/**
 * \struct utillib_ll1_parser
 * Parser for the LL(1) grammar.
 */
struct utillib_ll1_parser {
  struct utillib_vector symbol_stack;
  struct utillib_rule_index const *rule_index;
  struct utillib_vector2 const *table;
  void *client_data;
  struct utillib_ll1_parser_op const *op;
};

/**
 * \function utillib_ll1_factory_build_init
 * Initializes an LL1 factory by calling an ll1_builder on
 * the symbols and rules and build the table on the fly.
 */
void utillib_ll1_factory_build_init(struct utillib_ll1_factory *self,
                                    struct utillib_symbol const *symbols,
                                    struct utillib_rule_literal const *rules);

/**
 * \function utillib_ll1_factory_gen_init
 * Initializes an LL1 factory from generated table.
 * by ll1_generator.
 */
void utillib_ll1_factory_gen_init(struct utillib_ll1_factory *self,
                                  int const *gentable,
                                  struct utillib_symbol const *symbols,
                                  struct utillib_rule_literal const *rules);

void utillib_ll1_factory_destroy(struct utillib_ll1_factory *self);

/**
 * \function utillib_ll1_factory_parser_init
 * Binds an ll1_parser with the knowledge of the LL1 grammar
 * and the semantic actions from its client.
 * \param parser The parser to be initialized.
 * \param client_data The data of client that parser may access.
 * \param op The function table of the client that parser may call.
 */
void utillib_ll1_factory_parser_init(struct utillib_ll1_factory const *self,
                                     struct utillib_ll1_parser *parser,
                                     void *client_data,
                                     struct utillib_ll1_parser_op const *op);

/* Uses utillib_ll1_factory_parser_init instead */
void utillib_ll1_parser_init(struct utillib_ll1_parser *self,
                             struct utillib_rule_index const *rule_index,
                             struct utillib_vector2 const *table,
                             void *client_data,
                             struct utillib_ll1_parser_op const *op);

void utillib_ll1_parser_destroy(struct utillib_ll1_parser *self);

/**
 * \function utillib_ll1_parser_parse
 * Does the parsing and calls semantic actions.
 * \param input In fact it is the scanner struct itself but
 * writing `scanner->lookahead(input)' is much better than
 * `scanner_op->lookahead(scanner)' so choose such names.
 * \param scanner The function table of the scanner.
 */
bool utillib_ll1_parser_parse(struct utillib_ll1_parser *self, void *input,
                              struct utillib_scanner_op const *scanner);
#endif /* UTILLIB_LL1_PARSER_H */
