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

#ifndef CLING_AST_CHECK_H
#define CLING_AST_CHECK_H

#include <utillib/json.h>
#include <utillib/scanner.h>

#include <stddef.h>

/*
 * AST Semantic Checks
 * All these functions return CL_UNDEF as
 * a signal of failure which caller should note.
 */
struct cling_rd_parser;

/*
 * Specially for scanf_stmt where
 * only idens are allows as arguments.
 * Checks whether an iden is lvalue.
 * That is not const, array and function
 */
int cling_ast_check_iden_assignable(struct utillib_json_value const *self,
                                    struct cling_rd_parser *parser,
                                    struct utillib_token_scanner const *input,
                                    size_t context);

/*
 * Excluding assign_expr and boolean_expr.
 * Since the grammar singles out assign_expr to be assign_stmt and boolean_expr
 * to be condition, the door for these two guys to be expr
 * is closed here although in parsing, it was opened.
 */
int cling_ast_check_expression(struct utillib_json_value const *self,
                               struct cling_rd_parser *parser,
                               struct utillib_token_scanner const *input,
                               size_t context);

/*
 * Since the grammar only permits limited kinds
 * of expr_stmt, namely, assign_stmt, call_stmt,
 * we provide special check for that.
 */
int cling_ast_check_expr_stmt(struct utillib_json_value const *self,
                              struct cling_rd_parser *parser,
                              struct utillib_token_scanner const *input,
                              size_t context);

/*
 * Check whether the type of the return expr match the one declared
 * in the function prototype.
 */
int cling_ast_check_returnness(struct utillib_json_value const *self,
                               struct cling_rd_parser *parser,
                               struct utillib_token_scanner const *input,
                               size_t context, bool void_flag);
/*
 * The following functions are due to grammar limitness.
 * That is the grammar forbits certain kinds of contructs
 * which we should check about.
 */
/*
 * iden = expr
 * That all we can do in `for' the initial part.
 */
int cling_ast_check_for_init(struct utillib_json_value const *self,
                             struct cling_rd_parser *parser,
                             struct utillib_token_scanner const *input,
                             size_t context);
/*
 * condition ::= expr relop expr
 */
int cling_ast_check_condition(struct utillib_json_value const *self,
                              struct cling_rd_parser *parser,
                              struct utillib_token_scanner const *input,
                              size_t context);

/*
 * iden = iden (+|-) non-zero integer
 * That is for `for' step part.
 */
int cling_ast_check_for_step(struct utillib_json_value const *self,
                             struct cling_rd_parser *parser,
                             struct utillib_token_scanner const *input,
                             size_t context);

#endif /* CLING_AST_CHECK_H */
