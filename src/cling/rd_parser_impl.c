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
#include "rd_parser_impl.h"
#include "config.h"
#include "symbols.h"

#include <utillib/print.h>
#include <utillib/symbol.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct rd_parser_error *
rd_parser_error(int kind, struct utillib_token_scanner *input) {
  struct rd_parser_error *self = calloc(sizeof *self, 1);
  self->kind = kind;
  self->row = input->chars.row;
  self->col = input->chars.col;
}

void rd_parser_error_destroy(struct rd_parser_error *self) {
  free(self);
}

void rd_parser_skip_target_init(struct rd_parser_skip_target*self,
    size_t expected)
{
  self->expected=expected;
  memset(self->tars, -1, sizeof self->tars);
}

size_t rd_parser_skipto(struct rd_parser_skip_target const *self,
    struct utillib_token_scanner *input) 
{
  size_t code;
  while (true) {
    code=utillib_token_scanner_lookahead(input);
    if (code == UT_SYM_EOF || code == UT_SYM_ERR)
      return UT_SYM_EOF;
    for (int const * pi=self->tars; *pi!=-1; ++pi)
      if (*pi == code) 
        return code;
    utillib_token_scanner_shiftaway(input);
  }
}

struct rd_parser_error *rd_parser_expected_error(
    struct utillib_token_scanner *input,
    size_t expected,
    size_t actual, size_t context)
{
  struct rd_parser_error *self = rd_parser_error(CL_EEXPECT, input);
  self->einfo[0] = cling_symbol_cast(expected);
  self->einfo[1] = cling_symbol_cast(actual);
  self->einfo[2] = cling_symbol_cast(context);
  return self;
}

struct rd_parser_error *
rd_parser_noargs_error(struct utillib_token_scanner *input,
                             char const *name) {
  struct rd_parser_error *self = rd_parser_error(CL_ENOARGS, input);
  self->einfo[0] = name;
  return self;
}

struct rd_parser_error *
rd_parser_unexpected_error(struct utillib_token_scanner *input,
                                 size_t unexpected,
                                 size_t context) {
  struct rd_parser_error *self =
      rd_parser_error(CL_EUNEXPECTED, input);
  self->einfo[0] = cling_symbol_cast(unexpected);
  self->einfo[1] = cling_symbol_cast(context);
  return self;
}

void rd_parser_error_print(struct rd_parser_error const *error) {
  utillib_error_printf("ERROR at line %lu, column %lu:\n", error->row + 1,
                       error->col + 1);
  switch (error->kind) {
  case CL_EEXPECT:
    utillib_error_printf("expected `%s', got `%s' in `%s'",
                         error->einfo[0], error->einfo[1], error->einfo[2]);
    break;
  case CL_ENOARGS:
    utillib_error_printf(
        "Function `%s' expects at least one argument, but none was given",
        error->einfo[0]);
    break;
  case CL_EUNEXPECTED:
    utillib_error_printf("unexpected token `%s' in `%s'",
                         error->einfo[0], error->einfo[1]);
    break;
  default:
    assert(false && "unimplemented");
  }
  utillib_error_printf(".\n");
}
