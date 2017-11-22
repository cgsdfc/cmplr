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
#ifndef PASCAL_AST_H
#define PASCAL_AST_H

#include <utillib/enum.h>
#include <utillib/vector.h>

UTILLIB_ENUM_BEGIN(pascal_ast_kind)

UTILLIB_ENUM_ELEM(PAS_AST_UINT)
UTILLIB_ENUM_ELEM(PAS_AST_IDEN)
UTILLIB_ENUM_ELEM(PAS_AST_PROC)
UTILLIB_ENUM_ELEM(PAS_AST_SUBPROG)
UTILLIB_ENUM_ELEM(PAS_AST_PROGRAM)
UTILLIB_ENUM_ELEM(PAS_AST_TERM)
UTILLIB_ENUM_ELEM(PAS_AST_VAR)
UTILLIB_ENUM_ELEM(PAS_AST_CONST)
UTILLIB_ENUM_ELEM(PAS_AST_WRITE)
UTILLIB_ENUM_ELEM(PAS_AST_READ)
UTILLIB_ENUM_ELEM(PAS_AST_ASSIGN)
UTILLIB_ENUM_ELEM(PAS_AST_CALL)
UTILLIB_ENUM_ELEM(PAS_AST_EXPR)
UTILLIB_ENUM_ELEM(PAS_AST_LOOP)

UTILLIB_ENUM_END(pascal_ast_kind);

/**
 * \struct pascal_ast_node
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

struct pascal_ast_node {
  int kind;
  union {
    char const *as_iden;
    size_t as_uint;
    void *as_ptr;
  };
};

/**
 * \struct pascal_ast_const_decl
 * Each item in `items' is a pair of
 * `iden' and `uint' that defines one
 * constant.
 */
struct pascal_ast_const_decl {
  struct utillib_vector items;
};

/**
 * \struct pascal_ast_procedure
 * Procedure has a name a subprogram.
 */
struct pascal_ast_proc_decl {
  char const *name;
  void const *subprogram;
};

/**
 * \struct pascal_ast_var_decl
 * Each item in `items' is one string
 * that corresponds to an `iden'.
 */
struct pascal_ast_var_decl {
  struct utillib_vector items;
};

/**
 * \struct pascal_ast_assign_stmt
 * `LHS' is an `iden' while `RHS' is
 * an `expr'.
 */
struct pascal_ast_assign_stmt {
  void *LHS;
  void *RHS;
};

/**
 * \struct pascal_ast_read_stmt
 * A list of `iden's.
 */
struct pascal_ast_read_stmt {
  struct utillib_vector items;
};

/**
 * \struct pascal_ast_write_stmt
 * A list of `expr's.
 */
struct pascal_ast_write_stmt {
  struct utillib_vector items;
};

/**
 * \struct pascal_ast_term
 * Two operands grouped by `mul_op'.
 * `op' takes one of `SYM_MUL' and `SYM_DIV'.
 */
struct pascal_ast_term {
  int op;
  void *LHS;
  void *RHS;
};

/**
 * \struct pascal_ast_expr
 * Two operands grouped by `add_op'.
 * `op' takes one of `SYM_ADD' and `SYM_MINUS'.
 * An optional field `sign' is default to false
 * as non negative and true if a `-' was found
 * in the source.
 */

struct pascal_ast_expr {
  int sign;
  int op;
  void *LHS;
  void *RHS;
};

/* FACTOR is stored directly in ast_node */

/**
 * \struct pascal_ast_cond_stmt
 * A `if-then' conditional statement.
 * `cond' is a `pascal_ast_condition'.
 * `then' is any other statement.
 */
struct pascal_ast_cond_stmt {
  void *cond;
  void *then;
};

/**
 * \struct pascal_ast_comp_stmt
 * Composite statement.
 * A list of other statements.
 */
struct pascal_ast_comp_stmt {
  struct utillib_vector items;
};

/**
 * \struct pascal_ast_loop_stmt
 * A `while-do' statement.
 * `cond' is a `pascal_ast_condition'
 * which controls the loop body.
 * `do_' is the loop body.
 */
struct pascal_ast_loop_stmt {
  void *cond;
  void *do_;
};

struct pascal_ast_call_stmt {
  void *proc_name;
};

/**
 * \struct pascal_ast_condition
 * A condition that is tested against
 * in `while' or `if' statement.
 * `odd' is true if it takes the form `odd expr'
 * and false if not.
 * if `odd' is true, `LHS' should be `expr' while
 * `RHS' will be `NULL'.
 * Otherwise, `op' is the `rel_op' in which `expr rel_op expr'
 * is defined.
 */
struct pascal_ast_condition {
  bool odd;
  int op;
  void *LHS;
  void *RHS;
};

/**
 * \struct pascal_ast_subprogram
 * A segment of subprogram that takes
 * optional `const_decl', `var_decl'
 * and `proc_decl', but a manditory
 * `stmt' which is one of the statements.
 * When those optional fields are absent,
 * their should NULL.
 */
struct pascal_ast_subprogram {
  void *const_decl;
  void *var_decl;
  struct utillib_vector proc_list;
  void *comp_stmt;
};

union pascal_ast_union {
  struct pascal_ast_node *ast_node;
  struct pascal_ast_const_decl *const_decl;
  struct pascal_ast_proc_decl *proc_decl;
  struct pascal_ast_var_decl *var_decl;
  struct pascal_ast_cond_stmt *cond_stmt;
  struct pascal_ast_assign_stmt *assign_stmt;
  struct pascal_ast_read_stmt *read_stmt;
  struct pascal_ast_write_stmt *write_stmt;
  struct pascal_ast_comp_stmt *comp_stmt;
  struct pascal_ast_loop_stmt *loop_stmt;
  struct pascal_ast_call_stmt *call_stmt;

  struct pascal_ast_term *term;
  struct pascal_ast_expr *expr;
  struct pascal_ast_condition *cond;
  struct pascal_ast_subprogram *subprogram;
};

struct pascal_ast_node *pascal_ast_node_create(int kind);
void pascal_ast_const_decl_init(struct pascal_ast_const_decl *self);
void pascal_ast_var_decl_init(struct pascal_ast_var_decl *self);
void pascal_ast_proc_decl_init(struct pascal_ast_proc_decl *self);
void pascal_ast_assign_stmt_init(struct pascal_ast_assign_stmt *self);
void pascal_ast_read_stmt_init(struct pascal_ast_read_stmt *self);
void pascal_ast_write_stmt_init(struct pascal_ast_write_stmt *self);
void pascal_ast_term_init(struct pascal_ast_term *self);
void pascal_ast_expr_init(struct pascal_ast_expr *self);
void pascal_ast_cond_stmt_init(struct pascal_ast_cond_stmt *self);
void pascal_ast_comp_stmt_init(struct pascal_ast_comp_stmt *self);
void pascal_ast_loop_stmt_init(struct pascal_ast_loop_stmt *self);
void pascal_ast_call_stmt_init(struct pascal_ast_call_stmt *self);
void pascal_ast_condition_init(struct pascal_ast_condition *self);
void pascal_ast_subprogram_init(struct pascal_ast_subprogram *self);
void pascal_ast_node_destroy(struct pascal_ast_node *self);

#endif /* PASCAL_AST_H */
