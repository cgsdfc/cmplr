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
#include "ast_build.h"
#include "ast_check.h"
#include "error.h"
#include "opg_parser.h"
#include "symbol_table.h"
#include "symbols.h"

#include <utillib/hashmap.h>

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/**
 * File cling/rd_parser.c
 * All the recursive decent subroutines
 * go here.
 */

#define rd_parser_skip_init(self, ...)                                         \
  do {                                                                         \
    rd_parser_skip_target_init(self, __VA_ARGS__, -1);                         \
  } while (0)

static bool rd_parser_is_stmt_lookahead(size_t code);

static void rd_parser_fatal(struct cling_rd_parser *self) {
  longjmp(self->fatal_saver, 1);
}

static void rd_parser_skip_target_init(struct cling_rd_parser *self, ...) {
  va_list ap;
  va_start(ap, self);
  for (int i = 0, tar = va_arg(ap, int); tar != -1 && i < RD_PARSER_SKIP_MAX;
       tar = va_arg(ap, int), ++i) {
    self->tars[i] = tar;
  }
  self->tars[RD_PARSER_SKIP_MAX - 1] = -1;
  va_end(ap);
}

/*
 * When the skip targets are less than 4, use this
 * function. Otherwise, use special skipto_XXX functions.
 */
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
}

/*
 * Skips to the lookaheads of a statement.
 */

static int rd_parser_skipto_stmt(struct cling_rd_parser *self,
                                 struct utillib_token_scanner *input) {
  size_t code;
  while (true) {
    code = utillib_token_scanner_lookahead(input);
    if (code == UT_SYM_EOF || code == UT_SYM_ERR)
      rd_parser_fatal(self);
    if (rd_parser_is_stmt_lookahead(code))
      return code;
    utillib_token_scanner_shiftaway(input);
  }
}

/*
 * Skips to lookaheads of a declaration, const_decls, var_decls,
 * func_decls.
 */
static int rd_parser_skipto_decl(struct cling_rd_parser *self,
                                 struct utillib_token_scanner *input) {
  size_t code;
  while (true) {
    code = utillib_token_scanner_lookahead(input);
    if (code == UT_SYM_EOF || code == UT_SYM_ERR)
      rd_parser_fatal(self);
    if (code == SYM_KW_CONST || code == SYM_KW_INT || code == SYM_KW_VOID ||
        code == SYM_CHAR)
      return code;
    utillib_token_scanner_shiftaway(input);
  }
}

bool cling_rd_parser_has_errors(struct cling_rd_parser const *self) {
  return !utillib_vector_empty(&self->elist);
}

void rd_parser_error_push_back(struct cling_rd_parser *self,
                               struct cling_error *error) {
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
                                           char *first_iden, int scope_kind);

static struct utillib_json_value *for_stmt(struct cling_rd_parser *self,
                                           struct utillib_token_scanner *input);

/*
 * composite_stmt may enter scope or parse decl.
 * based on this enum, it may do both or none.
 */
enum { COMP_NO_SCOPE_AND_DECL = 0, COMP_ALLOW_DECL = 1, COMP_ENTER_SCOPE = 2 };

/*
 * Handles the SYM_LB for composite_stmt.
 * Passes comp_flag to it.
 */
static struct utillib_json_value *
composite_stmt_nolookahead(struct cling_rd_parser *self,
                           struct utillib_token_scanner *input, int comp_flag);

static struct utillib_json_value *
composite_stmt(struct cling_rd_parser *self,
               struct utillib_token_scanner *input, int comp_flag);

static struct utillib_json_value *
formal_arglist(struct cling_rd_parser *self,
               struct utillib_token_scanner *input);

/*
 * Common construct used in if_stmt and switch_stmt.
 */
static struct utillib_json_value *
lp_expr_rp(struct cling_rd_parser *self, struct utillib_token_scanner *input,
           /*
            * Since it is part of other constructs,
            * it needs context from those.
            */
           size_t context);

/*
 * composite_stmt is considered one kind of statement,
 * as C does.
 */
static struct utillib_json_value *
statement(struct cling_rd_parser *self, struct utillib_token_scanner *input);

/*
 * Unlike C, we have printf and scanf built-in
 * the grammar.
 */
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

/*
 * For test.
 */
static struct utillib_json_value *mock(struct cling_rd_parser *self,
                                       struct utillib_token_scanner *input);

/*
 * init/destroy
 */
void cling_rd_parser_init(struct cling_rd_parser *self,
                          struct cling_symbol_table *symbol_table) {
  self->curfunc = NULL;
  self->symbol_table = symbol_table;
  utillib_vector_init(&self->elist);
}

void cling_rd_parser_destroy(struct cling_rd_parser *self) {
  utillib_vector_destroy_owning(&self->elist, cling_error_destroy);
}

struct utillib_json_value *
cling_rd_parser_parse(struct cling_rd_parser *self,
                      struct utillib_token_scanner *input) {
  switch (setjmp(self->fatal_saver)) {
  case 0:
    return program(self, input);
  default:
    if (self->root)
      utillib_json_value_destroy(self->root);
    return NULL;
  }
}

/*
 * Call when cling_rd_parser_has_errors returns true.
 */
void cling_rd_parser_report_errors(struct cling_rd_parser const *self) {
  struct cling_error const *error;

  UTILLIB_VECTOR_FOREACH(error, &self->elist) { cling_error_print(error); }
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
  char const *name;
  struct utillib_json_value *object, *array, *value;
  struct cling_error *error;
  const size_t context = SYM_CONST_DEF;

  array = utillib_json_array_create_empty();

  while (true) {
    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_IDEN) {
      error = cling_expected_error(input, SYM_IDEN, context);
      goto skip;
    }
    object = utillib_json_object_create_empty();
    name = utillib_token_scanner_semantic(input);
    if (cling_symbol_table_exist_name(self->symbol_table, name, scope_kind)) {
      error = cling_redefined_error(input, name, context);
      goto skip;
    }

    cling_symbol_table_reserve(self->symbol_table, name, scope_kind);
    cling_ast_set_name(object, name);
    utillib_token_scanner_shiftaway(input);

    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_EQ) {
      error = cling_expected_error(input, SYM_EQ, context);
      goto skip;
    }
    utillib_token_scanner_shiftaway(input);

    code = utillib_token_scanner_lookahead(input);
    if (code != expected_initializer) {
      error = cling_expected_error(input, expected_initializer, context);
      goto skip;
    }

    value = cling_ast_string(utillib_token_scanner_semantic(input));
    utillib_token_scanner_shiftaway(input);

    utillib_json_object_push_back(object, "value", value);
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
      error = cling_unexpected_error(input, context);
      goto skip;
    }
  }
return_array:
  return array;

skip:
  utillib_json_value_destroy(object);
  rd_parser_error_push_back(self, error);
  rd_parser_skip_init(self, SYM_COMMA, SYM_SEMI);
  switch (rd_parser_skipto(self, input)) {
  case SYM_COMMA:
    goto before_comma;
  case SYM_SEMI:
    goto return_array;
  }
}

/**
 * Lookahead: SYM_KW_CONST
 * Maybenull
 */
static struct utillib_json_value *
single_const_decl(struct cling_rd_parser *self,
                  struct utillib_token_scanner *input, int scope_kind) {
  size_t code;
  struct utillib_json_value *object;
  const size_t context = SYM_CONST_DECL;

  utillib_token_scanner_shiftaway(input);
  object = utillib_json_object_create_empty();
  code = utillib_token_scanner_lookahead(input);

  if (code != SYM_KW_CHAR && code != SYM_KW_INT) {
    /* This is a serious error: const without typename */
    rd_parser_error_push_back(
        self, cling_expected_error(input, SYM_TYPENAME, context));
    goto skip;
  }
  cling_ast_set_type(object, code);
  utillib_token_scanner_shiftaway(input);
  utillib_json_object_push_back(
      object, "const_defs", /* expected_initializer */
      const_defs(self, input, code == SYM_KW_INT ? SYM_UINT : SYM_CHAR,
                 scope_kind));

  code = utillib_token_scanner_lookahead(input);
  if (code != SYM_SEMI) {
    rd_parser_error_push_back(self,
                              cling_expected_error(input, SYM_SEMI, context));
  } else {
    utillib_token_scanner_shiftaway(input);
  }
  return object;

return_null:
  utillib_json_value_destroy(object);
  return NULL;

skip:
  rd_parser_skip_init(self, SYM_SEMI, SYM_KW_CONST);
  switch (rd_parser_skipto(self, input)) {
  case SYM_SEMI:
    utillib_token_scanner_shiftaway(input);
    goto return_null;
  case SYM_KW_CONST:
    /*
     * This const is deliberately left for
     * `multiple_const_decl' to parse.
     */
    goto return_null;
  }
}

/**
 * Lookahead: SYM_KW_CONST
 * Nonull
 */
static struct utillib_json_value *
multiple_const_decl(struct cling_rd_parser *self,
                    struct utillib_token_scanner *input, int scope_kind) {
  size_t code;
  struct utillib_json_value *array;
  struct utillib_json_value *object;

  array = utillib_json_array_create_empty();
  while (true) {
    object = single_const_decl(self, input, scope_kind);
    if (object != NULL) {
      cling_symbol_table_insert_const(self->symbol_table, object);
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
                                           char *first_iden, int scope_kind) {
  size_t code;
  size_t expected;
  size_t const context = SYM_VAR_DEF;
  struct utillib_json_value *array = utillib_json_array_create_empty();
  struct utillib_json_value *object = utillib_json_object_create_empty();
  struct cling_error *error;

  if (cling_symbol_table_exist_name(self->symbol_table, first_iden,
                                    scope_kind)) {
    goto redefined;
  }
  cling_ast_set_name(object, first_iden);
  cling_symbol_table_reserve(self->symbol_table, first_iden, scope_kind);
  utillib_json_array_push_back(array, object);

  while (true) {
  loop:
    code = utillib_token_scanner_lookahead(input);
    switch (code) {
    case SYM_LK:
      utillib_token_scanner_shiftaway(input);
      code = utillib_token_scanner_lookahead(input);
      if (code != SYM_UINT) {
        expected = SYM_UINT;
        goto expected;
      }
      char const *extend = utillib_token_scanner_semantic(input);
      utillib_json_object_push_back(object, "extend", cling_ast_string(extend));
      utillib_token_scanner_shiftaway(input);
      code = utillib_token_scanner_lookahead(input);
      if (code != SYM_RK) {
        expected = SYM_RK;
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
        expected = SYM_IDEN;
        goto expected;
      }
      first_iden = utillib_token_scanner_semantic(input);
      if (cling_symbol_table_exist_name(self->symbol_table, first_iden,
                                        scope_kind)) {
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
  switch (expected) {
  case SYM_UINT:
    rd_parser_skip_init(self, SYM_RK, SYM_COMMA, SYM_SEMI);
    switch (rd_parser_skipto(self, input)) {
    case SYM_RK:
      goto before_rk;
    case SYM_COMMA:
      goto before_comma;
    case SYM_SEMI:
      goto return_array;
    }
  case SYM_IDEN:
    rd_parser_skip_init(self, SYM_COMMA, SYM_SEMI);
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
 * Nonull
 */
static struct utillib_json_value *
singel_var_decl(struct cling_rd_parser *self,
                struct utillib_token_scanner *input, size_t type,
                char *first_iden, int scope_kind) {
  size_t code;
  struct utillib_json_value *object;

  object = utillib_json_object_create_empty();
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
    rd_parser_error_push_back(
        self, cling_expected_error(input, SYM_SEMI, SYM_VAR_DECL));
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
                         char **first_iden, int scope_kind) {
  size_t code;
  struct cling_error *error;
  struct utillib_json_value *object;
  struct utillib_json_value *array;
  const size_t context = SYM_VAR_DECL;

  *type = utillib_token_scanner_lookahead(input);
  utillib_token_scanner_shiftaway(input);
  array = utillib_json_array_create_empty();

  while (true) {
    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_IDEN) {
      error = cling_expected_error(input, SYM_IDEN, context);
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
      free(*first_iden);
      /*
       * These names were checked in var_defs.
       */
      cling_symbol_table_insert_variable(self->symbol_table, object);
      utillib_json_array_push_back(array, object);
      code = utillib_token_scanner_lookahead(input);
      if (code != SYM_KW_INT && code != SYM_KW_CHAR)
        return array;
      *type = code;
      utillib_token_scanner_shiftaway(input);
      break;
    default:
      /* We are screw */
      error = cling_unexpected_error(input, context);
      goto skip;
    }
  }
return_array:
  return array;

skip:
  rd_parser_error_push_back(self, error);
  rd_parser_skip_init(self, SYM_SEMI);
  switch (rd_parser_skipto(self, input)) {
  case SYM_SEMI:
    utillib_token_scanner_shiftaway(input);
    goto return_array;
  }
}

/**
 * Lookahead SYM_KW_SCANF
 * Maybenull if **any** error happened.
 * scanf(iden...)
 */
static struct utillib_json_value *
scanf_stmt(struct cling_rd_parser *self, struct utillib_token_scanner *input) {
  size_t code;
  struct utillib_json_value *name;
  struct utillib_json_value *array;
  struct utillib_json_value *object;
  /*
   * This error is also used to indicate whether
   * an error ever happened. After all, what is
   * the need for an errored branch to be left
   * in the ast?
   */
  struct cling_error *error = NULL;

  const size_t context = SYM_SCANF_STMT;
  utillib_token_scanner_shiftaway(input);
  object = utillib_json_object_create_empty();
  cling_ast_set_type(object, SYM_SCANF_STMT);
  array = utillib_json_array_create_empty();
  utillib_json_object_push_back(object, "arglist", array);
  code = utillib_token_scanner_lookahead(input);

  if (code != SYM_LP) {
    error = cling_expected_error(input, SYM_LP, context);
    goto skip;
  }
  utillib_token_scanner_shiftaway(input);

  while (true) {
    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_IDEN) {
      error = cling_expected_error(input, SYM_IDEN, context);
      goto skip;
    }
  before_iden:
    name = cling_ast_factor(code, utillib_token_scanner_semantic(input));
    if (CL_UNDEF != cling_ast_check_iden_assignable(name, self, input, context))
      utillib_json_array_push_back(array, name);
    utillib_token_scanner_shiftaway(input);
    code = utillib_token_scanner_lookahead(input);
    switch (code) {
    before_comma:
    case SYM_COMMA:
      utillib_token_scanner_shiftaway(input);
      break;
    case SYM_RP:
      utillib_token_scanner_shiftaway(input);
      goto return_object;
    default:
      error = cling_unexpected_error(input, context);
      goto skip;
    }
  }
return_object:
  if (!error)
    return object;
  utillib_json_value_destroy(object);
  return NULL;

skip:
  rd_parser_error_push_back(self, error);
  rd_parser_skip_init(self, SYM_IDEN, SYM_SEMI, SYM_COMMA);
  switch (rd_parser_skipto(self, input)) {
  case SYM_COMMA:
    goto before_comma;
  case SYM_IDEN:
    goto before_iden;
  case SYM_SEMI:
    goto return_object;
  }
}

/*
 * Lookahead SYM_LP
 * Nonull
 */
static struct utillib_json_value *
formal_arglist(struct cling_rd_parser *self,
               struct utillib_token_scanner *input) {
  size_t code;
  char const *name;
  struct utillib_json_value *object;
  struct utillib_json_value *array;
  struct cling_error *error;
  const size_t context = SYM_NARGS;

  utillib_token_scanner_shiftaway(input);
  array = utillib_json_array_create_empty();
  code = utillib_token_scanner_lookahead(input);
  if (code == SYM_RP) {
    /*
     * Check empty.
     */
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
        rd_parser_error_push_back(
            self, cling_expected_error(input, SYM_IDEN, context));
        goto skip;
      }
      name = utillib_token_scanner_semantic(input);
      if (cling_symbol_table_exist_name(self->symbol_table, name, CL_LOCAL)) {
        utillib_json_value_destroy(object);
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
  rd_parser_error_push_back(self, cling_unexpected_error(input, context));
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
  rd_parser_skip_init(self, SYM_RP);
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
  if (expr == NULL) {
    goto expected_expr;
  }
  if (CL_UNDEF == cling_ast_check_expr_stmt(expr, self, input, context)) {
    utillib_json_value_destroy(expr);
    goto return_null;
  }

  utillib_json_object_push_back(object, "expr", expr);
  cling_opg_parser_destroy(&opg_parser);
  return object;

return_null:
  cling_opg_parser_destroy(&opg_parser);
  utillib_json_value_destroy(object);
  return NULL;

expected_expr:
  /*
   * An expression statement can go wrong easily.
   * Any grammatical error causes the whole expr
   * to lose due to the nature of opg_parser.
   * And we skip to SYM_SEMI.
   */
  rd_parser_error_push_back(
      self, cling_expected_error(input, SYM_EXPR, SYM_EXPR_STMT));
  rd_parser_skip_init(self, SYM_SEMI);
  switch (rd_parser_skipto(self, input)) {
  case SYM_SEMI:
    goto return_null;
  }
}

/*
 * Lookahead SYM_KW_RETURN
 * Maybenull
 */
static struct utillib_json_value *
return_stmt(struct cling_rd_parser *self, struct utillib_token_scanner *input) {
  size_t code;
  struct utillib_json_value *expr = NULL, *object;
  struct cling_opg_parser opg_parser;
  struct cling_error *error;
  const size_t context = SYM_RETURN_STMT;
  bool void_flag;

  utillib_token_scanner_shiftaway(input);
  cling_opg_parser_init(&opg_parser, SYM_SEMI);
  object = cling_ast_statement(SYM_RETURN_STMT);
  code = utillib_token_scanner_lookahead(input);

  switch (code) {
  case SYM_SEMI:
    void_flag = true;
    goto check_return;
  case SYM_LP:
    expr = cling_opg_parser_parse(&opg_parser, input);
    if (expr == NULL) {
      /*
       * If the expr is null, which is a syntax error,
       * there is no need to check for returnness.
       */
      rd_parser_error_push_back(self,
                                cling_expected_error(input, SYM_EXPR, context));
      goto return_null;
    }
    void_flag = false;
    goto check_return;
  default:
    rd_parser_error_push_back(self, cling_unexpected_error(input, context));
    goto skip;
  }

check_return:
  if (CL_UNDEF ==
      cling_ast_check_returnness(expr, self, input, context, void_flag))
    goto return_null;
  goto return_object;

return_object:
  if (!void_flag)
    /*
     * We are guarding NULL entering our
     * AST, which is a disaster.
     */
    utillib_json_object_push_back(object, "expr", expr);
  cling_opg_parser_destroy(&opg_parser);
  return object;

return_null:
  /*
   * Destroy everything.
   */
  if (expr)
    utillib_json_value_destroy(expr);
  cling_opg_parser_destroy(&opg_parser);
  utillib_json_value_destroy(object);
  return NULL;

skip:
  /*
   * Again, just skip to SYM_SEMI and
   * abort the whole statement.
   */
  rd_parser_skip_init(self, SYM_SEMI);
  switch (rd_parser_skipto(self, input)) {
  case SYM_SEMI:
    goto return_null;
  }
}

/**
 * Lookahead None
 * Maybe null
 * Skips to SYM_RP
 */
static struct utillib_json_value *
lp_expr_rp(struct cling_rd_parser *self, struct utillib_token_scanner *input,
           size_t context) {
  size_t code;
  struct cling_opg_parser opg_parser;
  struct utillib_json_value *expr;
  struct cling_error *error;

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
  if (expr == NULL) {
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
  rd_parser_skip_init(self, SYM_RP);
  switch (rd_parser_skipto(self, input)) {
  case SYM_RP:
    goto return_expr;
  }
}

/**
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
  if (expr != NULL) {
    if (CL_UNDEF != cling_ast_check_condition(expr, self, input, context))
      utillib_json_object_push_back(object, "expr", expr);
    else
      utillib_json_value_destroy(expr);
  }
  then_clause = statement(self, input);
  if (then_clause)
    utillib_json_object_push_back(object, "then", then_clause);
  code = utillib_token_scanner_lookahead(input);
  if (code != SYM_KW_ELSE) {
    goto return_object;
  }
  utillib_token_scanner_shiftaway(input);
  else_clause = statement(self, input);
  if (else_clause)
    utillib_json_object_push_back(object, "else", else_clause);

return_object:
  return object;
}

/*
 * We use cling_primary as key to check case label duplication.
 */
static const struct utillib_hashmap_callback label_map_callback = {
    .compare_handler = cling_primary_intcmp,
    .hash_handler = cling_primary_inthash,
};

/*
 * Lookahead SYM_KW_CASE | SYM_KW_DEFAULT
 * Maybenull
 */
static struct utillib_json_value *
switch_stmt_case_clause(struct cling_rd_parser *self,
                        struct utillib_token_scanner *input,
                        struct utillib_hashmap *label_map) {
  size_t code;
  union cling_primary label;
  /*
   * If the error is ever written to,
   * the whole "case":"stmt" will be aborted.
   */
  struct cling_error *error = NULL;
  struct utillib_json_value *object, *stmt;
  const size_t context = SYM_CASE_CLAUSE;

  object = utillib_json_object_create_empty();
  code = utillib_token_scanner_lookahead(input);
  utillib_token_scanner_shiftaway(input);
  switch (code) {
  case SYM_KW_DEFAULT:
    utillib_json_object_push_back(object, "default", &utillib_json_true);
    goto parse_colon_stmt;
  case SYM_KW_CASE:
    code = utillib_token_scanner_lookahead(input);
    switch (code) {
    case SYM_INTEGER:
    case SYM_UINT:
    case SYM_CHAR:
      cling_primary_init(&label, code, utillib_token_scanner_semantic(input));
      cling_primary_toint(&label, code);
      if (utillib_hashmap_exist_key(label_map, &label)) {
        /*
         * If a case label is duplicated, we parse the stmt
         * but not add it to ast.
         */
        rd_parser_error_push_back(self, error = cling_dupcase_error(
                                            input, label.signed_int, context));
        /*
         * Get rid of this duplicated case
         */
        utillib_token_scanner_shiftaway(input);
        goto parse_colon_stmt;
      }
      utillib_hashmap_insert(label_map, cling_primary_copy(&label), NULL);
      /*
       * Reuse this cling_primary.
       */
      label.string = utillib_token_scanner_semantic(input);
      utillib_json_object_push_back(object, "case",
                                    cling_ast_factor(code, label.string));
      utillib_token_scanner_shiftaway(input);
      goto parse_colon_stmt;
    default:
      rd_parser_error_push_back(
          self, error = cling_expected_error(input, SYM_CONSTANT, context));
      goto skip;
    }
  default:
    assert(false);
  }
parse_colon_stmt:
  code = utillib_token_scanner_lookahead(input);
  if (code != SYM_COLON) {
    rd_parser_error_push_back(
        self, error = cling_expected_error(input, SYM_COLON, context));
    rd_parser_skipto_stmt(self, input);
  } else {
    utillib_token_scanner_shiftaway(input);
  }
  stmt = statement(self, input);
  if (stmt) {
    utillib_json_object_push_back(object, "stmt", stmt);
  }
  if (error)
    goto return_null;
  return object;

return_null:
  utillib_json_value_destroy(object);
  return NULL;
skip:
  rd_parser_skip_init(self, SYM_KW_CASE, SYM_KW_DEFAULT);
  switch (rd_parser_skipto(self, input)) {
  case SYM_KW_CASE:
  case SYM_KW_DEFAULT:
    goto return_null;
  }
}

/*
 * Lookahead SYM_KW_CASE
 * Nonull
 */
static struct utillib_json_value *
switch_stmt_cases(struct cling_rd_parser *self,
                  struct utillib_token_scanner *input) {
  size_t code;
  struct utillib_json_value *array, *object;
  struct utillib_hashmap label_map;

  const size_t context = SYM_CASE_CLAUSE;
  array = utillib_json_array_create_empty();
  utillib_hashmap_init(&label_map, &label_map_callback);

  while (true) {
  loop:
    code = utillib_token_scanner_lookahead(input);
    switch (code) {
    case SYM_KW_DEFAULT:
      object = switch_stmt_case_clause(self, input, &label_map);
      if (object)
        utillib_json_array_push_back(array, object);
      goto return_array;
    case SYM_KW_CASE:
      object = switch_stmt_case_clause(self, input, &label_map);
      if (object)
        utillib_json_array_push_back(array, object);
      break;
    default:
      goto unexpected;
    }
  }

return_array:
  utillib_hashmap_destroy_owning(&label_map, free, NULL);
  return array;

unexpected:
  rd_parser_error_push_back(self, cling_unexpected_error(input, context));
  rd_parser_skip_init(self, SYM_KW_CASE, SYM_KW_DEFAULT, SYM_RB);
  switch (rd_parser_skipto(self, input)) {
  case SYM_KW_CASE:
  case SYM_KW_DEFAULT:
    goto loop;
  case SYM_RB:
    goto return_array;
  }
}

/*
 * Lookahead SYM_KW_SWITCH
 * cases and expr may both be absent.
 * Nonull
 */
static struct utillib_json_value *
switch_stmt(struct cling_rd_parser *self, struct utillib_token_scanner *input) {
  size_t code;
  struct utillib_json_value *object, *expr, *cases;
  struct cling_error *error;

  const size_t context = SYM_SWITCH_STMT;
  utillib_token_scanner_shiftaway(input);
  object = cling_ast_statement(SYM_SWITCH_STMT);
  expr = lp_expr_rp(self, input, context);
  if (NULL == expr) {
    /*
     * If lp_expr_rp returns NULL, it has
     * skipped to SYM_RP.
     */
    goto parse_lb;
  }
  if (CL_UNDEF != cling_ast_check_expression(expr, self, input, context))
    utillib_json_object_push_back(object, "expr", expr);
  else
    utillib_json_value_destroy(expr);

parse_lb:
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
    error = cling_expected_error(input, SYM_RB, context);
    goto expected_rb;
  }

return_object:
  utillib_token_scanner_shiftaway(input);
  return object;

expected_lb:
  rd_parser_error_push_back(self, error);
  rd_parser_skip_init(self, SYM_KW_CASE, SYM_RB);
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
  const size_t context = SYM_STMT;

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
  case SYM_KW_SWITCH:
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
    object = composite_stmt_nolookahead(self, input, COMP_NO_SCOPE_AND_DECL);
    goto return_object;
  case SYM_SEMI:
    /*
     * It is important that the empty
     * statement is retained as json_null.
     * because it is critical to grammatical
     * correctness which the AST should reflect.
     */
    utillib_token_scanner_shiftaway(input);
    return utillib_json_null_create();
  default:
    rd_parser_error_push_back(self, cling_unexpected_error(input, context));
    goto unexpected;
  }
return_object:
  return object;
return_null:
  utillib_token_scanner_shiftaway(input);
  return NULL;

unexpected:
  rd_parser_skip_init(self, SYM_SEMI);
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
 * Nonull
 */
static struct utillib_json_value *
for_stmt(struct cling_rd_parser *self, struct utillib_token_scanner *input) {
  size_t code;
  struct cling_opg_parser opg_parser;
  struct utillib_json_value *init, *cond, *step, *stmt, *object;
  struct cling_error *error;

  size_t context = SYM_FOR_STMT;
  cling_opg_parser_init(&opg_parser, SYM_SEMI);
  object = cling_ast_statement(SYM_FOR_STMT);

  utillib_token_scanner_shiftaway(input);
  code = utillib_token_scanner_lookahead(input);
  if (code != SYM_LP) {
    rd_parser_error_push_back(self,
                              cling_expected_error(input, SYM_LP, context));
    goto expected_lp;
  }

  context = SYM_FOR_INIT;
  utillib_token_scanner_shiftaway(input);
  init = cling_opg_parser_parse(&opg_parser, input);
  if (init == NULL) {
    rd_parser_error_push_back(self,
                              cling_expected_error(input, SYM_EXPR, context));
    goto expected_init;
  }
  if (CL_UNDEF != cling_ast_check_for_init(init, self, input, context))
    utillib_json_object_push_back(object, "init", init);
  else
    utillib_json_value_destroy(init);

parse_cond:
  context = SYM_CONDITION;
  utillib_token_scanner_shiftaway(input);
  cling_opg_parser_reinit(&opg_parser, SYM_SEMI);
  cond = cling_opg_parser_parse(&opg_parser, input);
  if (cond == NULL) {
    rd_parser_error_push_back(self,
                              cling_expected_error(input, SYM_EXPR, context));
    goto expected_cond;
  }
  if (CL_UNDEF != cling_ast_check_condition(cond, self, input, context))
    utillib_json_object_push_back(object, "cond", cond);
  else
    utillib_json_value_destroy(cond);

parse_step:
  context = SYM_FOR_STEP;
  utillib_token_scanner_shiftaway(input);
  cling_opg_parser_reinit(&opg_parser, SYM_RP);
  step = cling_opg_parser_parse(&opg_parser, input);
  if (step == NULL) {
    rd_parser_error_push_back(self,
                              cling_expected_error(input, SYM_EXPR, context));
    goto expected_step;
  }
  if (CL_UNDEF != cling_ast_check_for_step(step, self, input, context))
    utillib_json_object_push_back(object, "step", step);
  else
    utillib_json_value_destroy(step);
  code = utillib_token_scanner_lookahead(input);
  if (code != SYM_RP) {
    rd_parser_error_push_back(self,
                              cling_expected_error(input, SYM_RP, context));
    goto expected_rp;
  }

parse_stmt:
  utillib_token_scanner_shiftaway(input);
  stmt = statement(self, input);
  if (stmt != NULL)
    utillib_json_object_push_back(object, "stmt", stmt);
  return object;

expected_lp:
  rd_parser_skip_init(self, SYM_SEMI, SYM_RP);
  switch (rd_parser_skipto(self, input)) {
  case SYM_SEMI:
    goto parse_cond;
  case SYM_RP:
    goto parse_stmt;
  }

expected_init:
  rd_parser_skip_init(self, SYM_SEMI, SYM_RP);
  switch (rd_parser_skipto(self, input)) {
  case SYM_SEMI:
    goto parse_cond;
  case SYM_RP:
    goto parse_stmt;
  }
expected_cond:
  rd_parser_skip_init(self, SYM_SEMI, SYM_RP);
  switch (rd_parser_skipto(self, input)) {
  case SYM_SEMI:
    goto parse_step;
  case SYM_RP:
    goto parse_stmt;
  }
expected_step:
  rd_parser_skip_init(self, SYM_RP);
  switch (rd_parser_skipto(self, input)) {
  case SYM_RP:
    goto parse_stmt;
  }
expected_rp:
  goto parse_stmt;
}

static bool rd_parser_is_stmt_lookahead(size_t code) {
  return code == SYM_KW_PRINTF || code == SYM_KW_SCANF || code == SYM_KW_FOR ||
         code == SYM_KW_IF || code == SYM_KW_SWITCH || code == SYM_KW_RETURN ||
         code == SYM_LB || code == SYM_SEMI || code == SYM_IDEN;
}

/*
 * Lookahead None
 * Nonull
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
    if (object != NULL)
      utillib_json_array_push_back(array, object);
  }
}

/*
 * Ensures a lookahead of `SYM_LB' and then call
 * composite_stmt.
 * Normally all the functions require their caller to
 * ensure lookahead token for them, which makes this
 * parser predictive, but in case of composite_stmt,
 * those lookahead-ensuring code is put into **this**
 * function, which should always be used.
 * Nonull
 */
static struct utillib_json_value *
composite_stmt_nolookahead(struct cling_rd_parser *self,
                           struct utillib_token_scanner *input, int comp_flag) {

  size_t code;
  const size_t context = SYM_COMP_STMT;

  code = utillib_token_scanner_lookahead(input);
  if (code != SYM_LB) {
    goto expected_lb;
  }
  utillib_token_scanner_shiftaway(input);

parse_comp:
  return composite_stmt(self, input, comp_flag);

expected_lb:
  /*
   * Missing a SYM_LB is very typical.
   * In that we just do some favour for composite_stmt
   * by skipping to its Lookaheads.
   */
  rd_parser_error_push_back(self, cling_expected_error(input, SYM_LB, context));
  rd_parser_skip_init(self, SYM_KW_CONST, SYM_KW_INT, SYM_KW_CHAR);
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
 * Notes the stmts field may be missing because {}
 * Nonull
 */
static struct utillib_json_value *
composite_stmt(struct cling_rd_parser *self,
               struct utillib_token_scanner *input, int comp_flag) {
  size_t code;
  size_t type;
  char *first_iden;
  struct utillib_json_value *object, *const_decls, *var_decls, *stmts;
  const size_t context = SYM_COMP_STMT;

  object = cling_ast_statement(SYM_COMP_STMT);
  if (comp_flag & COMP_ENTER_SCOPE)
    /*
     * Sometimes scope is entered by `function_args_body'
     */
    cling_symbol_table_enter_scope(self->symbol_table);
  if (comp_flag & COMP_ALLOW_DECL) {
    /*
     * function-scope composite_stmt can
     * have decls.
     */
    code = utillib_token_scanner_lookahead(input);
    if (code == SYM_KW_CONST) {
      const_decls = multiple_const_decl(self, input, CL_LOCAL);
      utillib_json_object_push_back(object, "const_decls", const_decls);
    }
    code = utillib_token_scanner_lookahead(input);
    if (code == SYM_KW_CHAR || code == SYM_KW_INT) {
      var_decls =
          maybe_multiple_var_decls(self, input, &type, &first_iden, CL_LOCAL);
      utillib_json_object_push_back(object, "var_decls", var_decls);
    }
  }
  code = utillib_token_scanner_lookahead(input);
  if (code == SYM_RB)
    /*
     * Handle the empty composite_stmt.
     */
    goto parse_rb;

  code = utillib_token_scanner_lookahead(input);
  if (!rd_parser_is_stmt_lookahead(code)) {
    rd_parser_error_push_back(self, cling_unexpected_error(input, context));
    rd_parser_skipto_stmt(self, input);
  }
  stmts = multiple_statement(self, input);
  utillib_json_object_push_back(object, "stmts", stmts);

parse_rb:
  code = utillib_token_scanner_lookahead(input);
  if (code != SYM_RB) {
    rd_parser_error_push_back(
        self, cling_expected_error(input, SYM_RB, SYM_COMP_STMT));
  } else {
    utillib_token_scanner_shiftaway(input);
  }

  if (comp_flag & COMP_ENTER_SCOPE)
    cling_symbol_table_leave_scope(self->symbol_table);
  return object;
}

/*
 * Lookahead SYM_KW_PRINTF
 * printf(string)
 * printf(string, expr)
 * printf(expr)
 * Nonull
 */
static struct utillib_json_value *
printf_stmt(struct cling_rd_parser *self, struct utillib_token_scanner *input) {
  size_t code;
  struct cling_opg_parser opg_parser;
  struct utillib_json_value *array, *object, *expr, *string;
  const size_t context = SYM_PRINTF_STMT;

  cling_opg_parser_init(&opg_parser, SYM_RP);
  array = utillib_json_array_create_empty();
  object = cling_ast_statement(SYM_PRINTF_STMT);
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
    string = cling_ast_factor(code, utillib_token_scanner_semantic(input));
    utillib_json_array_push_back(array, string);
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
  case SYM_INTEGER:
  case SYM_CHAR:
  parse_expr:
    expr = cling_opg_parser_parse(&opg_parser, input);
    if (NULL == expr) {
      rd_parser_error_push_back(self,
                                cling_expected_error(input, SYM_EXPR, context));
      goto skip;
    }
    if (CL_UNDEF == cling_ast_check_expression(expr, self, input, context)) {
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
                              cling_expected_error(input, SYM_RP, context));
    goto skip;
  }
  utillib_token_scanner_shiftaway(input);

return_object:
  cling_opg_parser_destroy(&opg_parser);
  return object;

unexpected:
  rd_parser_error_push_back(self, cling_unexpected_error(input, context));

skip:
  rd_parser_skip_init(self, SYM_RP, SYM_SEMI);
  switch (rd_parser_skipto(self, input)) {
  case SYM_RP:
    goto parse_rp;
  case SYM_SEMI:
    goto return_object;
  }
}

/*
 * Lookahead SYM_LP
 * Nonull
 */
static struct utillib_json_value *
function_args_body(struct cling_rd_parser *self,
                   struct utillib_token_scanner *input,
                   struct utillib_json_value *object) {
  size_t code;
  struct utillib_json_value *comp_stmt, *arglist;

  cling_symbol_table_enter_scope(self->symbol_table);
  arglist = formal_arglist(self, input);
  cling_symbol_table_insert_arglist(self->symbol_table, arglist);
  utillib_json_object_push_back(object, "arglist", arglist);
  if (utillib_json_object_at(object, "name")) {
    /*
     * A function may lose its name due to redefinition.
     * Check here.
     */
    cling_symbol_table_insert_function(self->symbol_table, object);
  }

  /*
   * Handle maybe-missing SYM_LB
   */
  comp_stmt = composite_stmt_nolookahead(self, input, COMP_ALLOW_DECL);
  utillib_json_object_push_back(object, "comp", comp_stmt);
  cling_symbol_table_leave_scope(self->symbol_table);
  /*
   * It is important to reset the curfunc field
   * since it may cause false positive.
   */
  self->curfunc = NULL;
  return object;
}

/*
 * Lookahead SYM_KW_VOID | SYM_KW_INT | SYM_KW_CHAR
 * Nonull
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
  self->curfunc = cling_ast_set_name(object, name);

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
  rd_parser_error_push_back(self, cling_redefined_error(input, name, context));
  goto parse_args_body;

unexpected:
  /*
   * We lost the name of the function.
   */
  self->curfunc = NULL;
  rd_parser_error_push_back(self, cling_unexpected_error(input, context));
  rd_parser_skip_init(self, SYM_LP, SYM_RP, SYM_LB);
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
  rd_parser_skip_init(self, SYM_RP, SYM_LB);
  rd_parser_error_push_back(self, cling_expected_error(input, SYM_LP, context));
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
  comp = composite_stmt_nolookahead(self, input,
                                    COMP_ALLOW_DECL | COMP_ENTER_SCOPE);
  utillib_json_object_push_back(object, "comp", comp);
  return object;
}

/*
 * Lookahead SYM_KW_CHAR | SYM_KW_INT SYM_IDEN SYM_LP
 * Nonull
 */
static struct utillib_json_value *
first_ret_function(struct cling_rd_parser *self,
                   struct utillib_token_scanner *input, size_t type,
                   char const *name) {
  const size_t context = SYM_FUNC_DECL;
  struct utillib_json_value *object;
  object = utillib_json_object_create_empty();
  cling_ast_set_type(object, type);

  if (cling_symbol_table_exist_name(self->symbol_table, name, CL_GLOBAL)) {
    rd_parser_error_push_back(self,
                              cling_redefined_error(input, name, context));
  } else {
    cling_symbol_table_reserve(self->symbol_table, name, CL_GLOBAL);
    self->curfunc = cling_ast_set_name(object, name);
  }
  return function_args_body(self, input, object);
}

/*
 * Lookahead SYM_KW_VOID | SYM_LP
 * Nonull
 */
static struct utillib_json_value *
multiple_function(struct cling_rd_parser *self,
                  struct utillib_token_scanner *input, size_t maybe_type,
                  char *maybe_name) {
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
    /*
     * maybe_name is strdup by `maybe_multiple_var_decls'.
     */
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
  rd_parser_error_push_back(self, cling_unexpected_error(input, SYM_FUNC_DECL));
  utillib_json_value_destroy(array);
  rd_parser_fatal(self);
}

/*
 * Lookahead None
 * Nonull
 */
static struct utillib_json_value *program(struct cling_rd_parser *self,
                                          struct utillib_token_scanner *input) {
  size_t code;
  size_t type;
  char *first_iden;
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
    var_decls =
        maybe_multiple_var_decls(self, input, &type, &first_iden, CL_GLOBAL);
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
  rd_parser_error_push_back(self, cling_unexpected_error(input, SYM_PROGRAM));
  switch (rd_parser_skipto_decl(self, input)) {
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
  struct cling_opg_parser opg_parser;
  cling_opg_parser_init(&opg_parser, UT_SYM_EOF);
  bool is_main;
  struct utillib_json_value *val;
  /* val = cling_opg_parser_parse(&opg_parser, input); */
  /* cling_opg_parser_destroy(&opg_parser); */
  /* return program(self, input); */
  /* return switch_stmt_cases(self, input); */
  /* return printf_stmt(self, input); */
  val = function(self, input, &is_main);
  utillib_json_pretty_print(val);
  return val;
}
