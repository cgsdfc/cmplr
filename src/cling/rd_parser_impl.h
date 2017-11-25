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
#ifndef CLING_RD_PARSER_IMPH_H
#define CLING_RD_PARSER_IMPH_H

#include <utillib/enum.h>
#include <utillib/scanner.h>

/*
 * Error for RD parser defined here
 */

UTILLIB_ENUM_BEGIN(cling_rd_parser_error_kind)
  UTILLIB_ENUM_ELEM_INIT(CL_EAFMAIN, 1)
  UTILLIB_ENUM_ELEM(CL_EEXPECT)
UTILLIB_ENUM_END(cling_rd_parser_error_kind);

struct cling_rd_parser_error {
  int kind;
  union {
    int expected;
  };
};

struct cling_rd_parser_error *
cling_rd_parser_error_create(int kind);

void cling_rd_parser_error_destroy(
    struct cling_rd_parser_error *self);

void cling_rd_parser_shipto(void *input, struct utillib_scanner_op const *scanner,
    size_t target);

#endif /* CLING_RD_PARSER_IMPH_H */

