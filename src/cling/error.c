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
#include "symbols.h"

#include <utillib/print.h>

#include <assert.h>
#include <stdlib.h>

static struct cling_error *
cling_error_create(int kind, struct utillib_token_scanner *input,
    size_t context ) {
  struct cling_error *self = malloc(sizeof *self);
  self->kind = kind;
  self->row = input->chars.row;
  self->col = input->chars.col;
  self->context=cling_symbol_cast(context);
  return self;
}

static struct cling_error *
name_context_error(int kind, struct utillib_token_scanner *input,
    char const *name, size_t context) {
  struct cling_error *self = cling_error_create(kind, input, context);
  /* Free needed */
  self->einfo[0] = strdup(name);
  return self;
}

struct cling_error *cling_expected_error(struct utillib_token_scanner *input,
                                         size_t expected, size_t context) {
  struct cling_error *self = cling_error_create(CL_EEXPECT, input, context);
  self->einfo[0] = cling_symbol_cast(expected);
  self->einfo[1] = cling_symbol_cast(utillib_token_scanner_lookahead(input));
  return self;
}

struct cling_error *cling_unexpected_error(struct utillib_token_scanner *input,
                                            size_t context) {
  struct cling_error *self = cling_error_create(CL_EUNEXPECTED, input, context);
  self->einfo[0] = cling_symbol_cast(utillib_token_scanner_lookahead(input));
  return self;
}

struct cling_error *cling_redefined_error(struct utillib_token_scanner *input,
                                        char const *name, size_t context) {
  return name_context_error(CL_EREDEFINED, input,name, context);
}

struct cling_error *cling_undefined_name_error(struct utillib_token_scanner *input,
    char const *name, size_t context) {
  return name_context_error(CL_EUNDEFINED,input, name, context);
}

struct cling_error *cling_not_lvalue_error(struct utillib_token_scanner *input,
    char const *name, size_t context)
{
  return name_context_error(CL_ENOTLVALUE,input, name, context);
}

struct cling_error * cling_incompatible_type_error(struct utillib_token_scanner *input, 
    int actual_type, int expected_type, size_t context)
{
  struct cling_error *self=cling_error_create(input, CL_EINCTYPE, context);
  self->einfo[0]=cling_symbol_entry_kind_tostring(actual_type);
  self->einfo[1]=cling_symbol_entry_kind_tostring(expected_type);
  return self;
}

void cling_error_print(struct cling_error const *self) {
  utillib_error_printf("ERROR at line %lu, column %lu:\n", self->row + 1,
                       self->col + 1);
  char const *const *einfo = self->einfo;
  char const *context=self->context;

  switch (self->kind) {
  case CL_EEXPECT:
    utillib_error_printf("expected `%s', got `%s' in `%s'", einfo[0], einfo[1], context);
    break;
  case CL_EUNEXPECTED:
    utillib_error_printf("unexpected token `%s' in `%s'", einfo[0], context);
    break;
  case CL_EREDEFINED:
    utillib_error_printf("identifier `%s' was redefined in `%s'", einfo[0], context);
    break;
  case CL_EUNDEFINED:
    utillib_error_printf("undefined reference to `%s' in `%s'", einfo[0], context);
    break;
  case CL_ENOTLVALUE:
    utillib_error_printf("`%s' is not assignable in `%s'", einfo[0], context);
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
    free(self->einfo[0]);
    break;
  }
  free(self);
}

