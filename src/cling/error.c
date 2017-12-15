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
#include "error.h"
#include "ast_pretty.h"
#include "symbol_table.h"
#include "symbols.h"

#include <utillib/print.h>
#include <string.h>

#include <assert.h>
#include <stdlib.h>

static struct cling_error *
cling_error_create(int kind, struct utillib_token_scanner *input,
                   size_t context) {
  struct cling_error *self = malloc(sizeof *self);
  self->kind = kind;
  self->row = input->chars.row;
  self->col = input->chars.col;
  self->context = cling_symbol_cast(context);
  return self;
}

static struct cling_error *name_error(int kind,
                                      struct utillib_token_scanner *input,
                                      char const *name, size_t context) {
  struct cling_error *self = cling_error_create(kind, input, context);
  /* Free needed */
  self->einfo[0].str = strdup(name);
  return self;
}

static struct cling_error *ast_error(int kind,
                                     struct utillib_token_scanner *input,
                                     struct utillib_json_value const *value,
                                     size_t context) {
  struct utillib_string string;
  struct cling_error *self;

  self = cling_error_create(kind, input, context);
  cling_ast_pretty_expr(value, &string);
  /* Free needed */
  self->einfo[0].str = utillib_string_c_str(&string);
  return self;
}

struct cling_error *cling_expected_error(struct utillib_token_scanner *input,
                                         size_t expected, size_t context) {
  struct cling_error *self = cling_error_create(CL_EEXPECT, input, context);
  self->einfo[0].str = cling_symbol_cast(expected);
  self->einfo[1].str =
      cling_symbol_cast(utillib_token_scanner_lookahead(input));
  return self;
}

struct cling_error *cling_unexpected_error(struct utillib_token_scanner *input,
                                           size_t context) {
  struct cling_error *self = cling_error_create(CL_EUNEXPECTED, input, context);
  self->einfo[0].str =
      cling_symbol_cast(utillib_token_scanner_lookahead(input));
  return self;
}

struct cling_error *cling_redefined_error(struct utillib_token_scanner *input,
                                          char const *name, size_t context) {
  return name_error(CL_EREDEFINED, input, name, context);
}

struct cling_error *
cling_undefined_name_error(struct utillib_token_scanner *input,
                           char const *name, size_t context) {
  return name_error(CL_EUNDEFINED, input, name, context);
}

struct cling_error *cling_not_lvalue_error(struct utillib_token_scanner *input,
                                           struct utillib_json_value const *value,
                                           size_t context) {
  return ast_error(CL_ENOTLVALUE, input, value, context);
}

struct cling_error *
cling_incompatible_type_error(struct utillib_token_scanner *input,
                              int actual_type, int expected_type,
                              size_t context) {
  struct cling_error *self = cling_error_create(CL_EINCTYPE, input, context);
  self->einfo[0].str = cling_symbol_entry_kind_tostring(actual_type);
  self->einfo[1].str = cling_symbol_entry_kind_tostring(expected_type);
  return self;
}

struct cling_error *
cling_incompatible_arg_error(struct utillib_token_scanner *input, size_t argpos,
                             int actual_type, int expected_type,
                             size_t context) {
  struct cling_error *self = cling_error_create(CL_EINCARG, input, context);
  self->einfo[0].uint = argpos;
  self->einfo[1].str = cling_symbol_kind_tostring(actual_type);
  self->einfo[2].str = cling_symbol_kind_tostring(expected_type);
  return self;
}

struct cling_error *
cling_argc_unmatched_error(struct utillib_token_scanner *input,
                           char const *func_name, int actual_argc,
                           int expected_argc, size_t context) {
  struct cling_error *self = cling_error_create(CL_EARGCUNMAT, input, context);
  self->einfo[0].str = strdup(func_name);
  self->einfo[1].uint = actual_argc;
  self->einfo[2].uint = expected_argc;
  return self;
}

struct cling_error *
cling_invalid_expr_error(struct utillib_token_scanner *input,
                         struct utillib_json_value const *value, size_t context) {
  return ast_error(CL_EINVEXPR, input, value, context);
}

struct cling_error *cling_dupcase_error(struct utillib_token_scanner *input,
                                        int label, size_t context) {
  struct cling_error *self = cling_error_create(CL_EDUPCASE, input, context);
  self->einfo[0].int_ = label;
  return self;
}

void cling_error_print(struct cling_error const *self) {
#define positive_number(X) ((X)?(X):1)

  char const *context = self->context;
  union cling_einfo const *einfo = self->einfo;

  utillib_error_printf("ERROR at line %lu, column %lu:\n", positive_number(self->row),
      positive_number(self->col));
  switch (self->kind) {
  case CL_EEXPECT:
    utillib_error_printf("expected `%s', got `%s' in `%s'", einfo[0].str,
                         einfo[1].str, context);
    break;
  case CL_EUNEXPECTED:
    utillib_error_printf("unexpected token `%s' in `%s'", einfo[0].str,
                         context);
    break;
  case CL_EREDEFINED:
    utillib_error_printf("identifier `%s' was redefined in `%s'", einfo[0].str,
                         context);
    break;
  case CL_EUNDEFINED:
    utillib_error_printf("undefined reference to `%s' in `%s'", einfo[0].str,
                         context);
    break;
  case CL_ENOTLVALUE:
    utillib_error_printf("`%s' is not lvalue in `%s'", einfo[0].str, context);
    break;
  case CL_EINCARG:
    utillib_error_printf(
        "incompatible type of argument %lu in `%s': expected `%s', got `%s'",
        einfo[0].uint, context, einfo[2].str, einfo[3].str);
    break;
  case CL_EARGCUNMAT:
    utillib_error_printf(
        "in `%s', function `%s' expects %lu arguments, got %lu", context,
        einfo[0].str, einfo[2].uint, einfo[1].uint);
    break;
  case CL_EINCTYPE:
    utillib_error_printf("in `%s', incompatible type: expected `%s', got `%s'",
                         context, einfo[1].str, einfo[0].str);
    break;
  case CL_EINVEXPR:
    utillib_error_printf("in `%s', expression `%s' is not allowed", context,
                         einfo[0].str);
    break;
  case CL_EDUPCASE:
    utillib_error_printf("in `%s', duplicated case label %d", context,
                         einfo[0].int_);
    break;
  default:
    assert(false && "unimplemented");
  }
  utillib_error_printf(".\n");
}

void cling_error_destroy(struct cling_error *self) {
  switch (self->kind) {
  case CL_EREDEFINED:
  case CL_EUNDEFINED:
  case CL_ENOTLVALUE:
  case CL_EARGCUNMAT:
    free(self->einfo[0].str);
    break;
  }
  free(self);
}
