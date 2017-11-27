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

#include <utillib/symbol.h>

#include <assert.h>
#include <stdlib.h>
struct cling_rd_parser_error *
cling_rd_parser_error(int kind, struct utillib_token_scanner * input)
{
  struct cling_rd_parser_error * self=calloc(sizeof *self, 1);
  self->kind=kind;
  self->row=input->chars.row;
  self->col=input->chars.col;
}

void cling_rd_parser_error_destroy(
    struct cling_rd_parser_error *self)
{
  free(self);
}

bool cling_rd_parser_expect(
    struct utillib_token_scanner *input, 
    size_t expected)
{
  size_t actual=utillib_token_scanner_lookahead(input);
  if (expected != actual) {
    return true;
  }
  utillib_token_scanner_shiftaway(input);
  return false;
}

bool cling_rd_parser_skipto(struct utillib_token_scanner *input, 
    size_t target)
{
  size_t code;
  while ((code=utillib_token_scanner_lookahead(input)) != target) {
    if (code == UT_SYM_EOF || code == UT_SYM_ERR)
      return true;
    utillib_token_scanner_shiftaway(input);
  }
  return false;
}

struct cling_rd_parser_error *
cling_rd_parser_expected_error(
    struct utillib_token_scanner *input,
    struct utillib_symbol const *expected,
    struct utillib_symbol const *actual,
    struct utillib_symbol const *context)
{
  struct cling_rd_parser_error *self=cling_rd_parser_error(CL_EEXPECT, input);
  self->einfo[0]=expected->name;
  self->einfo[1]=actual->name;
  self->einfo[2]=context->name;
  return self;
}

struct cling_rd_parser_error *
cling_rd_parser_noargs_error(
    struct utillib_token_scanner *input,
    char const * name)
{
  struct cling_rd_parser_error *self=cling_rd_parser_error(CL_ENOARGS, input);
  self->einfo[0]=name;
  return self;
}

