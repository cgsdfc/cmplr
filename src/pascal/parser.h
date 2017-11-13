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

#include <utillib/enum.h>
#include <utillib/ll1_parser.h>

UTILLIB_ENUM_BEGIN(pascal_parser_AST_kind)
UTILLIB_ENUM_ELEM(PAS_AST_UINT)
UTILLIB_ENUM_ELEM(PAS_AST_IDEN)
UTILLIB_ENUM_ELEM(PAS_AST_PROC)
UTILLIB_ENUM_ELEM(PAS_AST_SUBPROG)
UTILLIB_ENUM_ELEM(PAS_AST_CONST)
UTILLIB_ENUM_ELEM(PAS_AST_VAR)
UTILLIB_ENUM_END(pascal_parser_AST_kind);

/**
 * \struct pascal_parser_ast_node
 * This and the following structs are
 * aimed to reflect the structure of
 * pascal grammar so do not complain
 * on their verbitage.
 * Various actions are taken according to the `kind'.
 * Specially, if the `kind' corresponds to a small
 * object that can fit into this node, it is stored
 * here directly.
 * These are `iden', `uint' and `factor'.
 *
 */

struct pascal_parser_ast_node {
  int kind;
  union {
    char const *as_iden;
    size_t as_uint;
    void *as_ptr;
  };
};

/**
 * \struct pascal_parser_const_decl
 * Each item in `items' is a pair of
 * `iden' and `uint' that defines one
 * constant.
 */
struct pascal_parser_const_decl {
  struct utillib_vector items;
};

/**
 * \struct pascal_parser_var_decl
 * Each item in `items' is one string
 * that corresponds to an `iden'.
 */
struct pascal_parser_var_decl {
  struct utillib_vector items;
};

/**
 * \struct pascal_parser_assign_stmt
 * `LHS' is an `iden' while `RHS' is
 * an `expr'.
 */
struct pascal_parser_assign_stmt {
  char const *LHS;
  void const *RHS;
};

/**
 * \struct pascal_parser_read_stmt
 * A list of `iden's.
 */
struct pascal_parser_read_stmt {
  struct utillib_vector items;
};

/**
 * \struct pascal_parser_write_stmt
 * A list of `expr's.
 */
struct pascal_parser_write_stmt {
  struct utillib_vector items;
};

/**
 * \struct pascal_parser_term
 * Two operands grouped by `mul_op'.
 * `op' takes one of `SYM_MUL' and `SYM_DIV'.
 */
struct pascal_parser_term {
  int op;
  void const *LHS;
  void const *RHS;
};

/**
 * \struct pascal_parser_expr
 * Two operands grouped by `add_op'.
 * `op' takes one of `SYM_ADD' and `SYM_MINUS'.
 * An optional field `sign' is default to false
 * as non negative and true if a `-' was found
 * in the source.
 */

struct pascal_parser_expr {
  int sign;
  int op;
  void const *LHS;
  void const *RHS;
};

/* FACTOR is stored directly in ast_node */

/**
 * \struct pascal_parser_cond_stmt
 * A `if-then' conditional statement.
 * `cond' is a `pascal_parser_condition'.
 * `then' is any other statement.
 */
struct pascal_parser_cond_stmt {
  void const *cond;
  void const *then;
};

/**
 * \struct pascal_parser_comp_stmt
 * Composite statement.
 * A list of other statements.
 */
struct pascal_parser_comp_stmt {
  struct utillib_vector items;
};

/**
 * \struct pascal_parser_loop_stmt
 * A `while-do' statement.
 * `cond' is a `pascal_parser_condition'
 * which controls the loop body.
 * `do_' is the loop body.
 */
struct pascal_parser_loop_stmt {
  void const *cond;
  void const *do_;
};
/* CALL is stored directly in ast_node */

/**
 * \struct pascal_parser_condition
 * A condition that is tested against
 * in `while' or `if' statement.
 * `odd' is true if it takes the form `odd expr'
 * and false if not.
 * if `odd' is true, `LHS' should be `expr' while
 * `RHS' will be `NULL'.
 * Otherwise, `op' is the `rel_op' in which `expr rel_op expr'
 * is defined.
 */
struct pascal_parser_condition {
  bool odd;
  int op;
  void const *LHS;
  void const *RHS;
};

/**
 * \struct pascal_parser_subprogram
 * A segment of subprogram that takes
 * optional `const_decl', `var_decl'
 * and `proc_decl', but a manditory
 * `stmt' which is one of the statements.
 * When those optional fields are absent,
 * their should NULL.
 */
struct pascal_parser_subprogram {
  void const *const_decl;
  void const *var_decl;
  void const *proc_decl;
  void const *stmt;
};

/**
 * \struct pascal_parser_procedure
 * Procedure has a name a list of
 * subprograms.
 */
struct pascal_parser_procedure {
  char const *name;
  struct utillib_vector subprogram;
};

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

void pascal_parser_init(struct pascal_parser *self);
void pascal_parser_destroy(struct pascal_parser *self);
bool pascal_parser_parse(struct pascal_parser *self, void *input,
                         struct utillib_scanner_op const *scanner);

#endif /* PASCAL_PARSER_H */
