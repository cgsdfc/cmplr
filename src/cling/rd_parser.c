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

#include "rd_parser.h"
#include "ast.h"
#include "error.h"
#include "ir.h"
#include "opg_parser.h"
#include "symbol_table.h"
#include "symbols.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/**
 * File cling/rd_parser.c
 * All the recursive decent subroutines
 * go here.
 */

static void rd_parser_fatal(struct cling_rd_parser *self) {
  longjmp(self->fatal_saver, 1);
}

static void rd_parser_skip_target_init(struct cling_rd_parser *self) {
  memset(self->tars, -1, sizeof self->tars);
}

static size_t rd_parser_skipto(struct cling_rd_parser *self,
                               struct utillib_token_scanner *input) {
  size_t code;
  while (true) {
    code = utillib_token_scanner_lookahead(input);
    if (code == UT_SYM_EOF || code == UT_SYM_ERR)
      rd_parser_fatal(self);
    for (int const *pi = self->tars; *pi != -1; ++pi)
      if (*pi == code)
        return code;
    utillib_token_scanner_shiftaway(input);
  }
  assert(false);
}

static void rd_parser_error_push_back(struct cling_rd_parser *self,
    struct cling_error *error)
{
  utillib_vector_push_back(&self->elist, error);
}

/*
 * Forward Declarations
 */

static struct utillib_json_value *
single_const_decl(struct cling_rd_parser *self,
                  struct utillib_token_scanner *input, int scope_kind);

static struct utillib_json_value *
multiple_const_decl(struct cling_rd_parser *self,
                    struct utillib_token_scanner *input, int scope_kind);

static struct utillib_json_value *
scanf_stmt(struct cling_rd_parser *self, struct utillib_token_scanner *input);

static struct utillib_json_value *var_defs(struct cling_rd_parser *self,
                                           struct utillib_token_scanner *input,
                                           char const *first_iden, int scope_kind);

static struct utillib_json_value *for_stmt(struct cling_rd_parser *self,
                                           struct utillib_token_scanner *input);

static struct utillib_json_value *
composite_stmt_nolookahead(struct cling_rd_parser *self,
               struct utillib_token_scanner *input,
               bool new_scope);

static struct utillib_json_value *
composite_stmt(struct cling_rd_parser *self,
               struct utillib_token_scanner *input,
               bool new_scope);

static struct utillib_json_value *
formal_arglist(struct cling_rd_parser *self,
               struct utillib_token_scanner *input);

static struct utillib_json_value *
lp_expr_rp(struct cling_rd_parser *self,
               struct utillib_token_scanner *input,
               /*
                * Since it is part of other constructs,
                * it needs context from those.
                */
               size_t context);

static struct utillib_json_value *
statement(struct cling_rd_parser *self, struct utillib_token_scanner *input);

static struct utillib_json_value *
printf_stmt(struct cling_rd_parser *self, struct utillib_token_scanner *input);

static struct utillib_json_value *
function_args_body(struct cling_rd_parser *self,
                   struct utillib_token_scanner *input,
                   struct utillib_json_value *object);

static struct utillib_json_value *function(struct cling_rd_parser *self,
                                           struct utillib_token_scanner *input,
                                           bool *is_main);

static struct utillib_json_value *program(struct cling_rd_parser *self,
                                          struct utillib_token_scanner *input);

static struct utillib_json_value *mock(struct cling_rd_parser *self,
                                       struct utillib_token_scanner *input);

/*
 * init/destroy
 */
void cling_rd_parser_init(struct cling_rd_parser *self,
                          struct cling_symbol_table *symbol_table,
                          struct cling_entity_list *entities) {
  self->symbol_table = symbol_table;
  self->entities = entities;
  utillib_vector_init(&self->elist);
}

void cling_rd_parser_destroy(struct cling_rd_parser *self) {
  utillib_vector_destroy_owning(&self->elist, free);
}

struct utillib_json_value *
cling_rd_parser_parse(struct cling_rd_parser *self,
                      struct utillib_token_scanner *input) {
  int code;
  switch (code = setjmp(self->fatal_saver)) {
  case 0:
    return mock(self, input);
  default:
    /* utillib_json_value_destroy(self->root); */
    return NULL;
  }
}

void cling_rd_parser_report_errors(struct cling_rd_parser const *self) {
  UTILLIB_VECTOR_FOREACH(struct cling_error const *, error, &self->elist) {
    cling_error_print(error);
  }
  fprintf(stderr, "%lu errors detected.\n", utillib_vector_size(&self->elist));
}

/*
 * Recursive Decent Subroutines
 */

/**
 * Lookahead: SYM_KW_CHAR | SYM_KW_INT
 */
static struct utillib_json_value *
const_defs(struct cling_rd_parser *self, struct utillib_token_scanner *input,
           size_t expected_initializer, int scope_kind) {

  size_t code;
  char const * name;
  size_t value;
  struct utillib_json_value *object, *array;
  struct cling_error * error;
  const size_t context=SYM_CONST_DEF;

  array = utillib_json_array_create_empty();

  while (true) {
    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_IDEN) {
      error=cling_expected_error(input, SYM_IDEN, context);
      goto skip;
    }
    object = utillib_json_object_create_empty();
    name = utillib_token_scanner_semantic(input);
    if (cling_symbol_table_exist_name(self->symbol_table, name, scope_kind)) {
      error=cling_redefined_error(input, name, context);
      goto skip;
    }

    cling_symbol_table_reserve(self->symbol_table, name, scope_kind);
    cling_ast_set_name(object, name);
    utillib_token_scanner_shiftaway(input);

    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_EQ) {
      error=cling_expected_error(input, SYM_EQ, context);
      goto skip;
    }
    utillib_token_scanner_shiftaway(input);

    code = utillib_token_scanner_lookahead(input);
    if (code != expected_initializer) {
      error=cling_expected_error(input, expected_initializer, context);
      goto skip;
    }

    value = utillib_token_scanner_semantic(input);
    utillib_token_scanner_shiftaway(input);

    utillib_json_object_push_back(object, "value",
                                  utillib_json_size_t_create(&value));
    utillib_json_array_push_back(array, object);

    code = utillib_token_scanner_lookahead(input);
    switch (code) {
    case SYM_SEMI:
      goto return_array;
    before_comma:
    case SYM_COMMA:
      utillib_token_scanner_shiftaway(input);
      break;
    default:
      /* unexpected */
      error=cling_unexpected_error(input, context);
      goto skip;
    }
  }
return_array:
  return array;

skip:
  rd_parser_error_push_back(self, error);
  rd_parser_skip_target_init(self);
  self->tars[0] = SYM_COMMA;
  self->tars[1] = SYM_SEMI;
  switch (rd_parser_skipto(self, input)) {
  case SYM_COMMA:
    goto before_comma;
  case SYM_SEMI:
    goto return_array;
  }
}

/**
 * Lookahead: SYM_KW_CONST
 */
static struct utillib_json_value *
single_const_decl(struct cling_rd_parser *self,
                  struct utillib_token_scanner *input, int scope_kind) {
  size_t code = utillib_token_scanner_lookahead(input);
  assert(code == SYM_KW_CONST);
  utillib_token_scanner_shiftaway(input);

  struct utillib_json_value *object;
  struct cling_error *error;
  const size_t context=SYM_CONST_DECL;
  code = utillib_token_scanner_lookahead(input);

  if (code != SYM_KW_CHAR && code != SYM_KW_INT) {
    /* This is a serious error: const without typename */
    error=cling_expected_error(input, SYM_TYPENAME, context);
    goto skip;
  }
  object = utillib_json_object_create_empty();
  cling_ast_set_type(object, code);
  utillib_token_scanner_shiftaway(input);
  utillib_json_object_push_back(
      object, "const_defs", /* expected_initializer */
      const_defs(self, input, code == SYM_KW_INT ? SYM_UINT : SYM_CHAR, scope_kind));

  code = utillib_token_scanner_lookahead(input);
  if (code != SYM_SEMI) {
    error=cling_expected_error(input, SYM_SEMI, context);
  } else {
    utillib_token_scanner_shiftaway(input);
  }
  return object;

skip:
  rd_parser_error_push_back(self, error);
  rd_parser_skip_target_init(self);
  self->tars[0] = SYM_SEMI;
  self->tars[1] = SYM_KW_CONST;
  switch (rd_parser_skipto(self, input)) {
    case SYM_SEMI:
      utillib_token_scanner_shiftaway(input);
      return utillib_json_null_create();
    case SYM_KW_CONST:
      /*
       * This const is deliberately left for
       * `multiple_const_decl' to parse.
       */
      return utillib_json_null_create();
  }
}

/**
 * Lookahead: SYM_KW_CONST
 * Nonull
 */
static struct utillib_json_value *
multiple_const_decl(struct cling_rd_parser *self,
                    struct utillib_token_scanner *input, int scope_kind) {
  size_t code = utillib_token_scanner_lookahead(input);
  assert(code == SYM_KW_CONST);
  struct utillib_json_value *array = utillib_json_array_create_empty();
  struct utillib_json_value *object;
  while (true) {
    object = single_const_decl(self, input, scope_kind);
    if (object != &utillib_json_null) {
      cling_ast_insert_const(object, self->symbol_table, scope_kind);
      utillib_json_array_push_back(array, object);
    }
    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_KW_CONST)
      return array;
  }
}

/**
 * Lookahead: SYM_LK | SYM_SEMI | SYM_COMMA
 */
static struct utillib_json_value *var_defs(struct cling_rd_parser *self,
                                           struct utillib_token_scanner *input,
                                           char const *first_iden,
                                           int scope_kind) {
  size_t code;
  size_t extend;
  size_t expected;
  size_t const context = SYM_VAR_DEF;
  struct utillib_json_value *array = utillib_json_array_create_empty();
  struct utillib_json_value *object = utillib_json_object_create_empty();
  struct cling_error *error;

  if (cling_symbol_table_exist_name(self->symbol_table, first_iden, scope_kind)) {
    rd_parser_skip_target_init(self);
    goto redefined;
  }
  cling_ast_set_name(object, first_iden);
  cling_symbol_table_reserve(self->symbol_table, first_iden, scope_kind);
  utillib_json_array_push_back(array, object);

  while (true) {
  loop:
    code = utillib_token_scanner_lookahead(input);
    switch (code) {
    /* Those that can follow iden are SYM_LK and SYM_COMMA*/
    case SYM_LK:
      utillib_token_scanner_shiftaway(input);
      code = utillib_token_scanner_lookahead(input);
      if (code != SYM_UINT) {
        expected=SYM_UINT;
        goto expected;
      }
      extend = utillib_token_scanner_semantic(input);
      cling_ast_set_extend(object, extend);
      utillib_token_scanner_shiftaway(input);
      code = utillib_token_scanner_lookahead(input);
      if (code != SYM_RK) {
        expected=SYM_RK;
        goto expected;
      }
    before_rk:
      utillib_token_scanner_shiftaway(input);
      break;
    before_comma:
    case SYM_COMMA:
      utillib_token_scanner_shiftaway(input);
      code = utillib_token_scanner_lookahead(input);
      if (code != SYM_IDEN) {
        expected=SYM_IDEN;
        goto expected;
      }
      first_iden = utillib_token_scanner_semantic(input);
      if (cling_symbol_table_exist_name(self->symbol_table, first_iden, scope_kind)) {
        goto redefined;
      }
      object = utillib_json_object_create_empty();
      cling_ast_set_name(object, first_iden);
      cling_symbol_table_reserve(self->symbol_table, first_iden, scope_kind);
      utillib_json_array_push_back(array, object);
      utillib_token_scanner_shiftaway(input);
      break;
    return_array:
    default:
      return array;
    }
  }
redefined:
  rd_parser_error_push_back(self, 
      cling_redefined_error(input, first_iden, context));
  goto loop;

expected:
  rd_parser_error_push_back(self,
      cling_expected_error(input, expected, context));
  switch (self->expected) {
  case SYM_UINT:
    self->tars[0] = SYM_RK;
    self->tars[1] = SYM_COMMA;
    self->tars[2] = SYM_SEMI;
    switch (rd_parser_skipto(self, input)) {
    case SYM_RK:
      goto before_rk;
    case SYM_COMMA:
      goto before_comma;
    case SYM_SEMI:
      goto return_array;
    default:
      assert(false);
    }
  case SYM_IDEN:
    self->tars[0] = SYM_COMMA;
    self->tars[1] = SYM_SEMI;
    switch (rd_parser_skipto(self, input)) {
    case SYM_COMMA:
      goto before_comma;
    case SYM_SEMI:
      goto return_array;
    }
  }
}

/**
 * Lookahead SYM_KW_INT | SYM_KW_CHAR SYM_IDEN
 */

static struct utillib_json_value *
singel_var_decl(struct cling_rd_parser *self,
                struct utillib_token_scanner *input, size_t type,
                char const *first_iden,
                int scope_kind) {
  size_t code;
  struct utillib_json_value *object = utillib_json_object_create_empty();

  cling_ast_set_type(object, type);
  /*
   * Var_decl differs from const_decls in that
   * the first_iden and type are lookaheaded
   * which means there won't be a null one returned
   * by var_defs.
   */
  utillib_json_object_push_back(object, "var_defs",
                                var_defs(self, input, first_iden, scope_kind));
  code = utillib_token_scanner_lookahead(input);
  if (code != SYM_SEMI) {
    rd_parser_error_push_back(self,
        cling_expected_error(input, SYM_SEMI, SYM_VAR_DECL));
  } else {
    utillib_token_scanner_shiftaway(input);
  }
  return object;
}

/*
 * Lookahead SYM_KW_INT | SYM_KW_CHAR
 * Nonull
 */
static struct utillib_json_value *
maybe_multiple_var_decls(struct cling_rd_parser *self,
                         struct utillib_token_scanner *input, size_t *type,
                         char const **first_iden,
                         int scope_kind) {
  size_t code = utillib_token_scanner_lookahead(input);
  const size_t context = SYM_VAR_DECL;
  struct cling_error *error;
  assert(code == SYM_KW_INT || code == SYM_KW_CHAR);
  utillib_token_scanner_shiftaway(input);
  *type = code;

  struct utillib_json_value *object;
  struct utillib_json_value *array = utillib_json_array_create_empty();

  while (true) {
    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_IDEN) {
      error=cling_expected_error(input, SYM_IDEN, context); 
      goto skip;
    }
    /* Must strdup first_iden */
    *first_iden = strdup(utillib_token_scanner_semantic(input));
    utillib_token_scanner_shiftaway(input);
    code = utillib_token_scanner_lookahead(input);
    switch (code) {
    case SYM_LP:
      /* We may have function here */
      goto return_array;
    case SYM_LK:
    case SYM_COMMA:
    case SYM_SEMI:
      object = singel_var_decl(self, input, *type, *first_iden, scope_kind);
      /*
       * This object has as least one first_iden
       * so it should not be null.
       */
      assert (object != &utillib_json_null);
      free(*first_iden);
      /*
       * These names were checked in var_defs.
       */
      cling_ast_insert_variable(object, self->symbol_table, scope_kind);
      utillib_json_array_push_back(array, object);
      code = utillib_token_scanner_lookahead(input);
      if (code != SYM_KW_INT && code != SYM_KW_CHAR)
        return array;
      *type = code;
      utillib_token_scanner_shiftaway(input);
      break;
    default:
      /* We are screw */
      error=cling_unexpected_error(input, context);
      goto skip;
    }
  }
return_array:
  return array;

skip:
  rd_parser_skip_target_init(self);
  rd_parser_error_push_back(self, error);
  self->tars[0] = SYM_SEMI;
  switch (rd_parser_skipto(self, input)) {
  case SYM_SEMI:
    utillib_token_scanner_shiftaway(input);
    goto return_array;
  }
}

/**
 * Lookahead SYM_KW_SCANF
 * Nonull
 */
static struct utillib_json_value *
scanf_stmt(struct cling_rd_parser *self, struct utillib_token_scanner *input) {
  size_t code;
  char const *name;
  struct utillib_json_value *array;
  struct utillib_json_value *object;
  struct cling_error * error;

  const size_t context = SYM_SCANF_STMT;
  utillib_token_scanner_shiftaway(input);
  object = utillib_json_object_create_empty();
  cling_ast_set_type(object, SYM_SCANF_STMT);
  array = utillib_json_array_create_empty();
  utillib_json_object_push_back(object, "arglist", array);
  code = utillib_token_scanner_lookahead(input);

  if (code != SYM_LP) {
    error=cling_expected_error(input, SYM_LP, context);
    goto expected_lp;
  }
  utillib_token_scanner_shiftaway(input);

  while (true) {
    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_IDEN) {
     error= cling_expected_error(input, SYM_IDEN, context);
      goto expected_iden;
    }
  before_iden:
    name = utillib_token_scanner_semantic(input);
    utillib_json_array_push_back(array, utillib_json_string_create(&name));
    utillib_token_scanner_shiftaway(input);
    code = utillib_token_scanner_lookahead(input);
    switch (code) {
    case SYM_COMMA:
      utillib_token_scanner_shiftaway(input);
      break;
    case SYM_RP:
      utillib_token_scanner_shiftaway(input);
      goto return_object;
    default:
      error=cling_unexpected_error(input, context);
      goto unexpected;
    }
  }
return_object:
  return object;

expected_lp:
  /*
   * If we are missing '(' after `scanf', skip
   * to ';' directly and return null if succeed.
   */
  rd_parser_error_push_back(self, error);
  rd_parser_skip_target_init(self);
  self->tars[0] = SYM_SEMI;
  switch (rd_parser_skipto(self, input)) {
  case SYM_SEMI:
    utillib_token_scanner_shiftaway(input);
    return utillib_json_null_create();
  }
expected_iden:
  /*
   * If we are missing iden after '(' or ',',
   * try to skip to the next iden or ';'.
   */
  rd_parser_error_push_back(self, error);
  rd_parser_skip_target_init(self);
  self->tars[0] = SYM_IDEN;
  self->tars[1] = SYM_SEMI;
  switch (rd_parser_skipto(self, input)) {
  case SYM_IDEN:
    goto before_iden;
  case SYM_SEMI:
    goto return_object;
  }
unexpected:
  /*
   * If we see an unexpected token after
   * iden, we handle it by skipping to a semi,
   * since the scanf_stmt is incomplete.
   */
  rd_parser_skip_target_init(self);
  rd_parser_error_push_back(self, error);
  self->tars[0] = SYM_SEMI;
  switch (rd_parser_skipto(self, input)) {
  case SYM_SEMI:
    goto return_object;
  }
}

/*
 * Lookahead SYM_LP
 */
static struct utillib_json_value *
formal_arglist(struct cling_rd_parser *self,
               struct utillib_token_scanner *input) {
  size_t code;
  char const *name;
  struct utillib_json_value *object;
  struct utillib_json_value *array;
  struct cling_error *error;
  const size_t context=SYM_NARGS;

  utillib_token_scanner_shiftaway(input);
  array = utillib_json_array_create_empty();
  code = utillib_token_scanner_lookahead(input);
  if (code == SYM_RP) {
    goto return_array;
  }

  while (true) {
    code = utillib_token_scanner_lookahead(input);
    switch (code) {
    case SYM_KW_INT:
    case SYM_KW_CHAR:
      object = utillib_json_object_create_empty();
      cling_ast_set_type(object, code);
      utillib_token_scanner_shiftaway(input);
      code = utillib_token_scanner_lookahead(input);
      if (code != SYM_IDEN) {
        rd_parser_error_push_back(self,
            cling_expected_error(input, SYM_IDEN, context));
        goto skip;
      }
      name = utillib_token_scanner_semantic(input);
      if (cling_symbol_table_exist_name(self->symbol_table, name, CL_LOCAL)) {
        rd_parser_error_push_back(self,
            cling_redefined_error(input, name, context));
        goto skip;
      }
      cling_symbol_table_reserve(self->symbol_table, name, CL_LOCAL);
      cling_ast_set_name(object, name);
      utillib_token_scanner_shiftaway(input);
      utillib_json_array_push_back(array, object);
      code = utillib_token_scanner_lookahead(input);
      switch (code) {
      case SYM_COMMA:
        utillib_token_scanner_shiftaway(input);
        break;
      case SYM_RP:
        goto return_array;
      default:
        goto unexpected;
      }
      break;
    default:
      goto unexpected;
    }
  }

return_array:
  utillib_token_scanner_shiftaway(input);
  return array;

unexpected:
  rd_parser_error_push_back(self,
      cling_unexpected_error(input, context));
  goto skip;

skip:
  /*
   * If some error happened, we skip to SYM_RP
   * because any attempt to recover at the midway
   * of an arglist may be wroing-guessing.
   * For example:
   * int foo(int i,char, char b)
   * can we interpret it as
   * int foo(int i, char b) ?
   */
  rd_parser_skip_target_init(self);
  self->tars[0] = SYM_RP;
  switch (rd_parser_skipto(self, input)) {
  case SYM_RP:
    goto return_array;
  }
}

/*
 * Lookahead SYM_IDEN
 * Nonull, but expr will be absent
 * when error.
 */
static struct utillib_json_value *
expr_stmt(struct cling_rd_parser *self, struct utillib_token_scanner *input) {
  size_t code;
  struct cling_opg_parser opg_parser;
  struct utillib_json_value *object, *expr;
  const size_t context = SYM_EXPR_STMT;

  object = cling_ast_statement(SYM_EXPR_STMT);
  cling_opg_parser_init(&opg_parser, SYM_SEMI);

  expr = cling_opg_parser_parse(&opg_parser, input);
  if (expr == &utillib_json_null) {
    goto expected_expr;
  }
  utillib_json_object_push_back(object, "expr", expr);

return_object:
  cling_opg_parser_destroy(&opg_parser);
  return object;

expected_expr:
  /*
   * An expression statement can go wrong easily.
   * Any grammatical error causes the whole expr
   * to lose due to the nature of opg_parser.
   * And we skip to SYM_SEMI.
   */
  rd_parser_error_push_back(self,
      cling_expected_error (input, SYM_EXPR, SYM_EXPR_STMT));
  rd_parser_skip_target_init(self);
  self->tars[0] = SYM_SEMI;
  switch (rd_parser_skipto(self, input)) {
  case SYM_SEMI:
    goto return_object;
  }
}

/*
 * Lookahead SYM_KW_RETURN
 * Nonull, but expr maybe absent.
 */
static struct utillib_json_value *
return_stmt(struct cling_rd_parser *self, struct utillib_token_scanner *input) {
  size_t code;
  struct utillib_json_value *expr, *object;
  struct cling_opg_parser opg_parser;
  struct cling_error *error;
  const size_t context=SYM_RETURN_STMT;

  utillib_token_scanner_shiftaway(input);
  cling_opg_parser_init(&opg_parser, SYM_SEMI);
  object = cling_ast_statement(SYM_RETURN_STMT);
  code = utillib_token_scanner_lookahead(input);

  switch (code) {
  case SYM_SEMI:
    /*
     * If the optional expr is absent,
     * the expr member of this object is also absent.
     */
    goto return_object;
  case SYM_LP:
    /*
     * If the expr is null, it will be absent.
     */
    expr=cling_opg_parser_parse(&opg_parser, input);
    if (expr != &utillib_json_null) 
      utillib_json_object_push_back(object, "expr", expr);
    goto return_object;
  default:
    rd_parser_error_push_back(self, 
        cling_unexpected_error(input, context));
    goto skip;
  }

return_object:
  cling_opg_parser_destroy(&opg_parser);
  return object;

skip:
  /*
   * Again, just skip to SYM_SEMI and
   * abort the whole expression.
   */
  rd_parser_skip_target_init(self);
  self->tars[0] = SYM_SEMI;
  switch (rd_parser_skipto(self, input)) {
  case SYM_SEMI:
    goto return_object;
  }
}

/**
 * Lookahead None
 * Maybe null
 */
static struct utillib_json_value *
lp_expr_rp(struct cling_rd_parser *self, 
    struct utillib_token_scanner *input,
    size_t context){
  size_t code;
  struct cling_opg_parser opg_parser;
  struct utillib_json_value *expr;
  struct cling_error  * error;

  cling_opg_parser_init(&opg_parser, SYM_RP);
  code = utillib_token_scanner_lookahead(input);

  if (code != SYM_LP) {
    /*
     * If the SYM_LP is missing it will be 
     * too naive to abort the whole expression.
     * So we raise an error and go on the parse
     * the expression.
     */
    rd_parser_error_push_back(self,
        cling_expected_error(input, SYM_LP, context));
  } else {
    utillib_token_scanner_shiftaway(input);
  }
  expr = cling_opg_parser_parse(&opg_parser, input);
  if (expr == &utillib_json_null) {
    rd_parser_error_push_back(self,
        cling_expected_error(input, SYM_EXPR, context));
    goto skip;
  }

return_expr:
  utillib_token_scanner_shiftaway(input);
  cling_opg_parser_destroy(&opg_parser);
  return expr;

skip:
  /*
   * Again, the error expr causes us to
   * skip to SYM_RP and return null.
   */
  rd_parser_skip_target_init(self);
  self->tars[0] = SYM_RP;
  switch (rd_parser_skipto(self, input)) {
  case SYM_RP:
    goto return_expr;
  }
}

/**
 * If (expr) stmt [else stmt]
 * Lookahead SYM_KW_IF
 */
static struct utillib_json_value *if_stmt(struct cling_rd_parser *self,
                                          struct utillib_token_scanner *input) {
  size_t code;
  struct utillib_json_value *expr, *object, *then_clause, *else_clause;

  utillib_token_scanner_shiftaway(input);
  const size_t context = SYM_IF_STMT;
  object = cling_ast_statement(SYM_IF_STMT);

  expr = lp_expr_rp(self, input, context);
  utillib_json_object_push_back(object, "expr", expr);
  then_clause = statement(self, input);
  utillib_json_object_push_back(object, "then", then_clause);
  code = utillib_token_scanner_lookahead(input);
  if (code != SYM_KW_ELSE) {
    goto return_object;
  }
  utillib_token_scanner_shiftaway(input);
  else_clause = statement(self, input);
  utillib_json_object_push_back(object, "else", else_clause);

return_object:
  return object;
}

/*
 * Lookahead SYM_KW_CASE
 */
static struct utillib_json_value *
switch_stmt_cases(struct cling_rd_parser *self,
                  struct utillib_token_scanner *input) {
  size_t code;
  struct utillib_json_value *array, *object, *stmt;
  const size_t context=SYM_CASE_CLAUSE;
  array = utillib_json_array_create_empty();

  while (true) {
  loop:
    code = utillib_token_scanner_lookahead(input);
    switch (code) {
    parse_case:
    case SYM_KW_CASE:
      object = utillib_json_object_create_empty();
      utillib_token_scanner_shiftaway(input);
      code = utillib_token_scanner_lookahead(input);
      if (code != SYM_UINT && code != SYM_CHAR) {
        goto expected_case;
      }
      utillib_json_object_push_back(
          object, "case",
          cling_ast_constant(code, utillib_token_scanner_semantic(input)));
      utillib_token_scanner_shiftaway(input);
    parse_colon:
      /*
       * If the SYM_COLON is missing, we just throw an
       * error are directly parse the expression.
       */
      code = utillib_token_scanner_lookahead(input);
      if (code != SYM_COLON) {
        rd_parser_error_push_back(self,
            cling_expected_error(input, SYM_COLON, context));
      }
      code = SYM_KW_CASE;
      goto parse_stmt;
    parse_default:
    case SYM_KW_DEFAULT:
      object = utillib_json_object_create_empty();
      utillib_token_scanner_shiftaway(input);
      code = utillib_token_scanner_lookahead(input);
      if (code != SYM_COLON)
        rd_parser_error_push_back(self,
            cling_expected_error(input, SYM_COLON, context));
      utillib_json_object_push_back(object, "default", &utillib_json_true);
      code = SYM_KW_DEFAULT;
      goto parse_stmt;
    default:
      goto unexpected;
    }
  }
parse_stmt:
  utillib_token_scanner_shiftaway(input);
  stmt = statement(self, input);
  utillib_json_object_push_back(object, "stmt", stmt);
  utillib_json_array_push_back(array, object);
  switch (code) {
  case SYM_KW_CASE:
    goto loop;
  return_array:
  case SYM_KW_DEFAULT:
    return array;
  }

expected_case:
  rd_parser_error_push_back(self,
      cling_expected_error(input, SYM_KW_CASE, context));
  if (code != SYM_COLON)
    utillib_token_scanner_shiftaway(input);
  goto parse_colon;

unexpected:
  rd_parser_error_push_back(self,
      cling_unexpected_error(input, SYM_CASE_CLAUSE));
  rd_parser_skip_target_init(self);
  self->tars[0] = SYM_KW_CASE;
  self->tars[1] = SYM_KW_DEFAULT;
  self->tars[2] = SYM_RB;
  switch (rd_parser_skipto(self, input)) {
  case SYM_KW_CASE:
    goto parse_case;
  case SYM_KW_DEFAULT:
    goto parse_default;
  case SYM_RB:
    goto return_array;
  }
}

/*
 * Lookahead SYM_KW_SWITCH
 * cases and expr may both be absent.
 */
static struct utillib_json_value *
switch_stmt(struct cling_rd_parser *self, struct utillib_token_scanner *input) {
  size_t code;
  struct utillib_json_value *object, *expr, *cases;
  struct cling_error * error;

  const size_t context = SYM_SWITCH_STMT;
  utillib_token_scanner_shiftaway(input);
  object = cling_ast_statement(SYM_SWITCH_STMT);
  expr = lp_expr_rp(self, input, context);
  if (&utillib_json_null != expr) {
    utillib_json_object_push_back(object, "expr", expr);
  }

  code = utillib_token_scanner_lookahead(input);
  if (code != SYM_LB) {
    goto expected_lb;
  }
  utillib_token_scanner_shiftaway(input);

parse_cases:
  cases = switch_stmt_cases(self, input);
  utillib_json_object_push_back(object, "cases", cases);
  code = utillib_token_scanner_lookahead(input);
  if (code != SYM_RB) {
    error=cling_expected_error(input, SYM_RB, context);
    goto expected_rb;
  }

return_object:
  utillib_token_scanner_shiftaway(input);
  return object;

expected_lb:
  rd_parser_error_push_back(self, error);
  rd_parser_skip_target_init(self);
  self->tars[0] = SYM_KW_CASE;
  self->tars[1] = SYM_RB;
  switch (rd_parser_skipto(self, input)) {
  case SYM_KW_CASE:
    goto parse_cases;
  case SYM_RB:
    goto return_object;
  }

expected_rb:
  rd_parser_error_push_back(self, error);
  return object;
}

/**
 * Lookahead None
 * Maybe null
 */
static struct utillib_json_value *
statement(struct cling_rd_parser *self, struct utillib_token_scanner *input) {
  size_t code = utillib_token_scanner_lookahead(input);
  struct utillib_json_value *object;
  const size_t context=SYM_STMT;

  switch (code) {
  case SYM_KW_RETURN:
    object = return_stmt(self, input);
    goto parse_semi;
  case SYM_KW_PRINTF:
    object = printf_stmt(self, input);
    goto parse_semi;
  case SYM_KW_SCANF:
    object = scanf_stmt(self, input);
    goto parse_semi;
  case SYM_KW_FOR:
    object = for_stmt(self, input);
    goto return_object;
  case SYM_KW_IF:
    object = if_stmt(self, input);
    goto return_object;
  case SYW_KW_SWITCH:
    object = switch_stmt(self, input);
    goto return_object;
  case SYM_IDEN:
    /*
     * Although the Lookahead for expression
     * is far vaster than iden, according to
     * the grammar, expr_stmt = assign_stmt | call_stmt,
     * both of which start with iden.
     * Thus, iden is simply enough.
     */
    object = expr_stmt(self, input);
    goto parse_semi;
  case SYM_LB:
    /*
     * Since we are entering a statement
     * not a function, we need a new scope.
     *
     */
    object = composite_stmt_nolookahead(self, input, true);
    goto return_object;
  case SYM_SEMI:
  return_null:
    utillib_token_scanner_shiftaway(input);
    return utillib_json_null_create();
  default:
    rd_parser_error_push_back(self, 
        cling_unexpected_error(input, context));
    goto unexpected;
  }
return_object:
  return object;

unexpected:
  rd_parser_skip_target_init(self);
  self->tars[0] = SYM_SEMI;
  switch (rd_parser_skipto(self, input)) {
  case SYM_SEMI:
    goto return_null;
  }
parse_semi:
  code = utillib_token_scanner_lookahead(input);
  if (code == SYM_SEMI) {
    utillib_token_scanner_shiftaway(input);
    goto return_object;
  }
  rd_parser_error_push_back(self,
      cling_expected_error(input, SYM_SEMI, context));
  goto return_object;
}

/*
 * Lookahead SYM_KW_FOR
 * init, cond, step, stmt may all be absent.
 */
static struct utillib_json_value *
for_stmt(struct cling_rd_parser *self, struct utillib_token_scanner *input) {
  size_t code;
  struct cling_opg_parser opg_parser;
  struct utillib_json_value *init, *cond, *step, *stmt, *object;
  struct cling_error * error;

  const size_t context = SYM_FOR_STMT;
  cling_opg_parser_init(&opg_parser, SYM_SEMI);
  object = cling_ast_statement(SYM_FOR_STMT);

  utillib_token_scanner_shiftaway(input);
  code = utillib_token_scanner_lookahead(input);
  if (code != SYM_LP) {
    rd_parser_error_push_back(self,
        cling_expected_error(input, SYM_LP, context));
    goto expected_lp;
  }

  utillib_token_scanner_shiftaway(input);
  init = cling_opg_parser_parse(&opg_parser, input);
  if (init == &utillib_json_null) {
    rd_parser_error_push_back(self,
        cling_expected_error(input, SYM_EXPR, context));
    goto expected_init;
  }
  utillib_json_object_push_back(object, "init", init);

parse_cond:
  utillib_token_scanner_shiftaway(input);
  cling_opg_parser_reinit(&opg_parser, SYM_SEMI);
  cond = cling_opg_parser_parse(&opg_parser, input);
  if (cond == &utillib_json_null) {
    rd_parser_error_push_back(self,
        cling_expected_error(input, SYM_EXPR, context));
    goto expected_cond;
  }
  utillib_json_object_push_back(object, "cond", cond);

parse_step:
  utillib_token_scanner_shiftaway(input);
  cling_opg_parser_reinit(&opg_parser, SYM_RP);
  step = cling_opg_parser_parse(&opg_parser, input);
  if (step == &utillib_json_null) {
    rd_parser_error_push_back(self,
        cling_expected_error(input, SYM_EXPR, context));
    goto expected_step;
  }
  utillib_json_object_push_back(object, "step", step);
  code = utillib_token_scanner_lookahead(input);
  if (code != SYM_RP) {
    rd_parser_error_push_back(self,
        cling_expected_error(input, SYM_RP, context));
    goto expected_rp;
  }

parse_stmt:
  utillib_token_scanner_shiftaway(input);
  stmt = statement(self, input);
  if (stmt != &utillib_json_null)
    utillib_json_object_push_back(object, "stmt", stmt);
  return object;

expected_lp:
  rd_parser_skip_target_init(self);
  self->tars[0] = SYM_SEMI;
  self->tars[1] = SYM_RP;
  switch (rd_parser_skipto(self, input)) {
  case SYM_SEMI:
    goto parse_cond;
  case SYM_RP:
    goto parse_stmt;
  }

expected_init:
  rd_parser_skip_target_init(self);
  self->tars[0] = SYM_SEMI;
  self->tars[1] = SYM_RP;
  switch (rd_parser_skipto(self, input)) {
  case SYM_SEMI:
    goto parse_cond;
  case SYM_RP:
    goto parse_stmt;
  }
expected_cond:
  rd_parser_skip_target_init(self);
  self->tars[0] = SYM_SEMI;
  self->tars[1] = SYM_RP;
  switch (rd_parser_skipto(self, input)) {
  case SYM_SEMI:
    goto parse_step;
  case SYM_RP:
    goto parse_stmt;
  }
expected_step:
  rd_parser_skip_target_init(self);
  self->tars[0] = SYM_RP;
  switch (rd_parser_skipto(self, input)) {
  case SYM_RP:
    goto parse_stmt;
  }
expected_rp:
  rd_parser_skip_target_init(self);
  goto parse_stmt;
}

static bool rd_parser_is_stmt_lookahead(size_t code) {
  return code == SYM_KW_PRINTF || code == SYM_KW_SCANF || code == SYM_KW_FOR ||
         code == SYM_KW_IF || code == SYW_KW_SWITCH || code == SYM_KW_RETURN ||
         code == SYM_LB || code == SYM_SEMI || code == SYM_IDEN;
}

/*
 * Lookahead None
 */
static struct utillib_json_value *
multiple_statement(struct cling_rd_parser *self,
                   struct utillib_token_scanner *input) {
  size_t code;
  struct utillib_json_value *object;
  struct utillib_json_value *array = utillib_json_array_create_empty();

  while (true) {
    code = utillib_token_scanner_lookahead(input);
    if (!rd_parser_is_stmt_lookahead(code))
      return array;
    object = statement(self, input);
    if (object != &utillib_json_null) 
      utillib_json_array_push_back(array, object);
  }
}

/*
 * Since function, function_args_body 
 * both call composite_stmt, and both
 * need to handle the situation when
 * a SYM_LB is missing, the missing-handling
 * code is extracted into composite_stmt_nolookahead.
 * which ensures a lookahead of `SYM_LB' and then call
 * composite_stmt.
 * Normally all the functions require their caller to
 * ensure lookahead token for them, which makes this
 * parser predictive, but in case of composite_stmt,
 * those lookahead-ensuring code is put into **this**
 * function, which should always be used.
 */
static struct utillib_json_value *
composite_stmt_nolookahead(struct cling_rd_parser *self,
               struct utillib_token_scanner *input,
               bool new_scope) {

  size_t code;
  const size_t context=SYM_COMP_STMT;
  
  code=utillib_token_scanner_lookahead(input);
  if (code != SYM_LB) {
    goto expected_lb;
  }
  utillib_token_scanner_shiftaway(input);

parse_comp:
  return composite_stmt(self, input ,new_scope);

expected_lb:
  /*
   * Missing a SYM_LB is very typical.
   * In that we just do some favour for composite_stmt
   * by skipping to its Lookaheads.
   */
  rd_parser_error_push_back(self, 
      cling_expected_error(input, SYM_LB, context));
  rd_parser_skip_target_init(self);
  self->tars[0] = SYM_KW_CONST;
  self->tars[1]=SYM_KW_INT;
  self->tars[2]=SYM_KW_CHAR;
  switch (rd_parser_skipto(self, input)) {
  case SYM_KW_CONST:
  case SYM_KW_INT:
  case SYM_KW_CHAR:
    goto parse_comp;
  }


}

/*
 * Lookahead SYM_KW_CONST | SYM_KW_INT | SYM_CHAR | 
 * all those for statement.
 */
static struct utillib_json_value *
composite_stmt(struct cling_rd_parser *self,
               struct utillib_token_scanner *input,
               bool new_scope) {
  size_t code;
  size_t type;
  char const *first_iden;
  struct utillib_json_value *object, *const_decls, *var_decls, *stmts;

  object = cling_ast_statement(SYM_COMP_STMT);
  code = utillib_token_scanner_lookahead(input);
  if(new_scope)
    /*
     * Since the arguments of a function belong to
     * the same scope of the its composite_stmt,
     * in which case the insertion of these arguments
     * means the scope should be created
     * by the function rather than composite_stmt.
     * This bool is to tell that.
     */
    cling_symbol_table_enter_scope(self->symbol_table);

  if (code == SYM_KW_CONST) {
    const_decls = multiple_const_decl(self, input, CL_LOCAL);
    utillib_json_object_push_back(object, "const_decls", const_decls);
  }
  code = utillib_token_scanner_lookahead(input);
  if (code == SYM_KW_CHAR || code == SYM_KW_INT) {
    var_decls = maybe_multiple_var_decls(self, input, &type, &first_iden, CL_LOCAL);
    utillib_json_object_push_back(object, "var_decls", var_decls);
  }
  code = utillib_token_scanner_lookahead(input);
  if (rd_parser_is_stmt_lookahead(code)) {
    stmts = multiple_statement(self, input);
    utillib_json_object_push_back(object, "stmts", stmts);
  }
  code = utillib_token_scanner_lookahead(input);
  if (code != SYM_RB) {
    rd_parser_error_push_back(self,
        cling_expected_error(input, SYM_RB,  SYM_COMP_STMT));
  } else {
    utillib_token_scanner_shiftaway(input);
  }

  if (new_scope)
    cling_symbol_table_leave_scope(self->symbol_table);
  return object;

}

/*
 * Lookahead SYM_KW_PRINTF
 */
static struct utillib_json_value *
printf_stmt(struct cling_rd_parser *self, struct utillib_token_scanner *input) {
  size_t code;
  struct cling_opg_parser opg_parser;
  struct utillib_json_value *array;
  struct utillib_json_value *object;
  struct utillib_json_value *expr;
  struct utillib_json_value *strobj;
  char const *string;
  const size_t context=SYM_PRINTF_STMT;

  cling_opg_parser_init(&opg_parser, SYM_RP);
  array = utillib_json_array_create_empty();
  object = utillib_json_object_create_empty();
  cling_ast_set_type(object, SYM_PRINTF_STMT);
  utillib_json_object_push_back(object, "arglist", array);

  utillib_token_scanner_shiftaway(input);
  code = utillib_token_scanner_lookahead(input);

  if (code != SYM_LP) {
    rd_parser_error_push_back(self,
        cling_expected_error(input, SYM_LP, context));
    goto skip;
  }
  utillib_token_scanner_shiftaway(input);

  code = utillib_token_scanner_lookahead(input);
  switch (code) {
  case SYM_STRING:
    /* 
     * We use an is_str attr to distinguish string and iden,
     * {
     *   "is_str":true,
     *   "value": "string"
     * }
     */
    strobj = utillib_json_object_create_empty();
    utillib_json_object_push_back(strobj, "is_str", &utillib_json_true);
    string = utillib_token_scanner_semantic(input);
    utillib_json_object_push_back(strobj, "value",
                                  utillib_json_string_create(&string));
    utillib_json_array_push_back(array, strobj);
    utillib_token_scanner_shiftaway(input);
    code = utillib_token_scanner_lookahead(input);
    switch (code) {
    case SYM_COMMA:
      utillib_token_scanner_shiftaway(input);
      goto parse_expr;
    case SYM_RP:
      goto parse_rp;
    default:
      goto unexpected;
    }
    break;
  case SYM_IDEN:
  case SYM_LP:
  case SYM_UINT:
  case SYM_CHAR:
  parse_expr:
    expr = cling_opg_parser_parse(&opg_parser, input);
    if (&utillib_json_null == expr) {
      rd_parser_error_push_back(self,
          cling_expected_error(input, SYM_EXPR, context));
      goto skip;
    }
    utillib_json_array_push_back(array, expr);
    goto parse_rp;
  default:
    goto unexpected;
  }

parse_rp:
  code = utillib_token_scanner_lookahead(input);
  if (code != SYM_RP) {
    rd_parser_error_push_back(self,
        cling_expected_error(input, SYM_LP, context));
    goto skip;
  }
  utillib_token_scanner_shiftaway(input);

return_object:
  cling_opg_parser_destroy(&opg_parser);
  return object;

unexpected:
  rd_parser_error_push_back(self,
      cling_unexpected_error(input, context));

skip:
  rd_parser_skip_target_init(self);
  self->tars[0] = SYM_RP;
  self->tars[1] = SYM_SEMI;
  switch (rd_parser_skipto(self, input)) {
  case SYM_RP:
    goto parse_rp;
  case SYM_SEMI:
    goto return_object;
  }
}

/*
 * Lookahead SYM_LP
 */
static struct utillib_json_value *
function_args_body(struct cling_rd_parser *self,
                   struct utillib_token_scanner *input,
                   struct utillib_json_value *object) {
  size_t code;
  struct utillib_json_value *comp_stmt, *arglist;

  cling_symbol_table_enter_scope(self->symbol_table);
  arglist = formal_arglist(self, input);
  cling_ast_insert_arglist(arglist, self->symbol_table);
  utillib_json_object_push_back(object, "arglist", arglist);
  cling_ast_insert_function(object, self->symbol_table);

  /*
   * Handle maybe-missing SYM_LB
   */
  comp_stmt = composite_stmt_nolookahead(self, input, false);
  utillib_json_object_push_back(object, "comp", comp_stmt);
  cling_symbol_table_leave_scope(self->symbol_table);
  return object;

}

/*
 * Lookahead SYM_KW_VOID | SYM_KW_INT | SYM_KW_CHAR
 */
static struct utillib_json_value *function(struct cling_rd_parser *self,
                                           struct utillib_token_scanner *input,
                                           bool *is_main) {
  size_t code;
  struct utillib_json_value *object;
  struct utillib_json_value *comp;
  char const *name;

  const size_t context = SYM_FUNC_DECL;
  code = utillib_token_scanner_lookahead(input);
  object = utillib_json_object_create_empty();
  cling_ast_set_type(object, code);
  utillib_token_scanner_shiftaway(input);

  code = utillib_token_scanner_lookahead(input);
  switch (code) {
    /*
     * First of all, we try to recognize
     * its name -- whether it is main.
     */
  case SYM_IDEN:
    *is_main = false;
    name = utillib_token_scanner_semantic(input);
    break;
  case SYM_KW_MAIN:
    *is_main = true;
    name = "main";
    break;
  default:
    goto unexpected;
  }
  if (cling_symbol_table_exist_name(self->symbol_table, name, CL_GLOBAL)) {
    goto redefined;
  }
  cling_symbol_table_reserve(self->symbol_table, name, CL_GLOBAL);
  cling_ast_set_name(object, name);

parse_args_body:
  /* SYM_IDEN */
  utillib_token_scanner_shiftaway(input);
  code = utillib_token_scanner_lookahead(input);
  if (code != SYM_LP) {
    goto expected_lp;
  }
  return function_args_body(self, input, object);
redefined:
  /*
   * Although the name is redefined, we still need
   * to look into its arglist and composite_stmt
   * to pick up more possible errors.
   */
  rd_parser_error_push_back(self,
      cling_redefined_error(input, name, context));
  goto parse_args_body;

unexpected:
  /*
   * We lost the name of the function.
   */
  rd_parser_error_push_back(self,
      cling_unexpected_error(input, context));
  rd_parser_skip_target_init(self);
  self->tars[0]=SYM_LP;
  self->tars[1]=SYM_RP;
  self->tars[2]=SYM_LB;
  switch (rd_parser_skipto(self, input)) {
    case SYM_LP:
      goto parse_args_body;
    case SYM_RP:
      utillib_token_scanner_shiftaway(input);
      goto parse_comp;
    case SYM_LB:
      goto parse_comp;
  }
expected_lp:
  rd_parser_skip_target_init(self);
  rd_parser_error_push_back(self,
      cling_expected_error(input, SYM_LP, context));
  self->tars[0] = SYM_RP;
  self->tars[1] = SYM_LB;
  switch (rd_parser_skipto(self, input)) {
  case SYM_RP:
    utillib_token_scanner_shiftaway(input);
    goto parse_comp;
  case SYM_LB:
    goto parse_comp;
  }
parse_comp:
  /*
   * Since the arglist is missing totally,
   * we directly enter a new scope in 
   * composite_stmt_nolookahead.
   */
  comp = composite_stmt_nolookahead(self, input, true);
  utillib_json_object_push_back(object, "comp", comp);
  return object;
}

/*
 * Lookahead SYM_KW_CHAR | SYM_KW_INT SYM_IDEN SYM_LP
 */
static struct utillib_json_value *
first_ret_function(struct cling_rd_parser *self,
                   struct utillib_token_scanner *input, size_t type,
                   char const *name) {
  struct utillib_json_value *object;

  object = utillib_json_object_create_empty();
  cling_ast_set_type(object, type);
  cling_ast_set_name(object, name);
  return function_args_body(self, input, object);
}

/*
 * Lookahead SYM_KW_VOID | SYM_LP
 */
static struct utillib_json_value *
multiple_function(struct cling_rd_parser *self,
                  struct utillib_token_scanner *input, size_t maybe_type,
                  char const *maybe_name) {
  size_t code;
  size_t type;
  char const *name;
  bool is_main;
  struct utillib_json_value *array;
  struct utillib_json_value *object;

  const size_t context = SYM_FUNC_DECL;
  code = utillib_token_scanner_lookahead(input);
  array = utillib_json_array_create_empty();

  switch (code) {
  case SYM_KW_VOID:
    object = function(self, input, &is_main);
    utillib_json_array_push_back(array, object);
    if (is_main) {
      goto return_array;
    }
    break;
  case SYM_LP:
    object = first_ret_function(self, input, maybe_type, maybe_name);
    free(maybe_name);
    utillib_json_array_push_back(array, object);
    break;
  default:
    /* Checked by `maybe_multiple_var_decls' */
    assert(false);
  }
  while (true) {
    code = utillib_token_scanner_lookahead(input);
    switch (code) {
    case SYM_KW_VOID:
    case SYM_KW_INT:
    case SYM_KW_CHAR:
      object = function(self, input, &is_main);
      utillib_json_array_push_back(array, object);
      if (is_main)
        goto return_array;
      break;
    default:
      goto unexpected;
    }
  }
return_array:
  code = utillib_token_scanner_lookahead(input);
  if (code != UT_SYM_EOF) {
    rd_parser_error_push_back(self, 
        cling_unexpected_error(input, SYM_FUNC_DECL));
  }
  return array;

unexpected:
  rd_parser_error_push_back(self, 
      cling_unexpected_error(input, SYM_FUNC_DECL));
  utillib_json_value_destroy(array);
  rd_parser_fatal(self);
}

/*
 * Lookahead None
 */
static struct utillib_json_value *program(struct cling_rd_parser *self,
                                          struct utillib_token_scanner *input) {
  size_t code;
  size_t type;
  char const *first_iden;
  struct utillib_json_value *const_decls, *var_decls, *func_decls;

  const size_t context = SYM_PROGRAM;
  self->root = utillib_json_object_create_empty();
  code = utillib_token_scanner_lookahead(input);
  switch (code) {
  case SYM_KW_CONST:
  parse_const:
    const_decls = multiple_const_decl(self, input, CL_GLOBAL);
    utillib_json_object_push_back(self->root, "const_decls", const_decls);
  /* Fall through */
  case SYM_KW_INT:
  case SYM_KW_CHAR:
  parse_maybe_var:
    var_decls = maybe_multiple_var_decls(self, input, &type, &first_iden, CL_GLOBAL);
    utillib_json_object_push_back(self->root, "var_decls", var_decls);
  /* Fall through */
  case SYM_KW_VOID:
  parse_function:
    func_decls = multiple_function(self, input, type, first_iden);
    utillib_json_object_push_back(self->root, "func_decls", func_decls);
    break;
  default:
    goto unexpected;
  }
  return self->root;
unexpected:
  rd_parser_error_push_back(self, 
      cling_unexpected_error(input, SYM_PROGRAM));
  self->tars[0] = SYM_KW_CONST;
  self->tars[1] = SYM_KW_INT;
  self->tars[2] = SYM_KW_CHAR;
  self->tars[3] = SYM_KW_VOID;
  switch (rd_parser_skipto(self, input)) {
  case SYM_KW_CONST:
    goto parse_const;
  case SYM_KW_INT:
  case SYM_KW_CHAR:
    goto parse_maybe_var;
  case SYM_KW_VOID:
    goto parse_function;
  }
}

static struct utillib_json_value *mock(struct cling_rd_parser *self,
                                       struct utillib_token_scanner *input) {
  struct cling_opg_parser opg_parser ;
  cling_opg_parser_init(&opg_parser, SYM_RP);
  bool is_main;
  /* return cling_opg_parser_parse(&opg_parser, input); */
  return function(self, input, &is_main);
  /* return program(self, input); */
}
