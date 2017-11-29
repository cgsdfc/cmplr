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

#include <stdlib.h>
#include <assert.h>


struct cling_error *
cling_error_create(int kind, struct utillib_token_scanner *input) {
  struct cling_error *self = calloc(sizeof *self, 1);
  self->kind = kind;
  self->row = input->chars.row;
  self->col = input->chars.col;
}

void cling_error_destroy(struct cling_error *self) {
  switch (self->kind) {
  case CL_EREDEFINED:
    free(self->einfo[0]);
    break;
  }
  free(self);
}


struct cling_error *cling_expected_error(
    struct utillib_token_scanner *input,
    size_t expected,
    size_t actual, size_t context)
{
  struct cling_error *self = cling_error_create(CL_EEXPECT, input);
  self->einfo[0] = cling_symbol_cast(expected);
  self->einfo[1] = cling_symbol_cast(actual);
  self->einfo[2] = cling_symbol_cast(context);
  return self;
}

struct cling_error *
cling_noargs_error(struct utillib_token_scanner *input,
                             char const *name) {
  struct cling_error *self = cling_error_create(CL_ENOARGS, input);
  self->einfo[0] = name;
  return self;
}

struct cling_error *
cling_unexpected_error(struct utillib_token_scanner *input,
                                 size_t unexpected,
                                 size_t context) {
  struct cling_error *self =
      cling_error_create(CL_EUNEXPECTED, input);
  self->einfo[0] = cling_symbol_cast(unexpected);
  self->einfo[1] = cling_symbol_cast(context);
  return self;
}

struct cling_error *
cling_redined_error(struct utillib_token_scanner *input,
    char const * name,
    size_t context) {
  struct cling_error * self=cling_error_create(CL_EREDEFINED,input );
  /* Free needed */
  self->einfo[0]=strdup(name);
  self->einfo[1]=cling_symbol_cast(context);
  return self;
}

void cling_error_print(struct cling_error const *self) {
  utillib_error_printf("ERROR at line %lu, column %lu:\n", self->row + 1,
                       self->col + 1);
  char const * const* einfo=self->einfo;
  switch (self->kind) {
  case CL_EEXPECT:
    utillib_error_printf("expected `%s', got `%s' in `%s'",
                         einfo[0], einfo[1], einfo[2]);
    break;
  case CL_ENOARGS:
    utillib_error_printf(
        "Function `%s' expects at least one argument, but none was given",
        einfo[0]);
    break;
  case CL_EUNEXPECTED:
    utillib_error_printf("unexpected token `%s' in `%s'",
                         einfo[0], einfo[1]);
    break;
  case CL_EREDEFINED:
    utillib_error_printf("identifier `%s' was redefined in `%s'",
        einfo[0], einfo[1]);
    break;
  case CL_EVAGUE:
    utillib_error_printf("%s", einfo[0]);
    break;
  case CL_EPREMATURE:
    utillib_error_printf("fatal: premature end of input in `%s'",
        einfo[0]);
    break;
  default:
    assert(false && "unimplemented");
  }
  utillib_error_printf(".\n");
}

struct cling_error *
cling_vague_error(struct utillib_token_scanner *input,
    char const * errmsg)
{
  struct cling_error * self=cling_error_create(CL_EVAGUE, input);
  self->einfo[0]=errmsg;
  return self;
}

struct cling_error *
cling_premature_error(struct utillib_token_scanner *input,
    size_t context)
{
  struct cling_error *self=cling_error_create(CL_EPREMATURE, input);
  self->einfo[0]=cling_symbol_cast(context);
  return self;
}


