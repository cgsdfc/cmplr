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
#include <utillib/json.h>

#define RD_PARSER_EMAX 4
#define RD_PARSER_SKIP_MAX 4

struct cling_rd_parser;

/*
 * Error for RD parser defined here
 */

UTILLIB_ENUM_BEGIN(rd_parser_error_kind)
UTILLIB_ENUM_ELEM_INIT(CL_EAFMAIN, 1)
UTILLIB_ENUM_ELEM(CL_EEXPECT)
UTILLIB_ENUM_ELEM(CL_ENOARGS)
UTILLIB_ENUM_ELEM(CL_EUNEXPECTED)
UTILLIB_ENUM_ELEM(CL_EREDEFINED)
UTILLIB_ENUM_END(rd_parser_error_kind);

struct rd_parser_error {
  int kind;
  size_t row;
  size_t col;
  char const *einfo[RD_PARSER_EMAX];
};

struct rd_parser_skip_target {
  size_t expected;
  int tars[RD_PARSER_SKIP_MAX];
};

/**
 * \function rd_parser_error
 * Creates an error recording the kind of
 * the error, the location in the source file.
 * Notes that caller can store arbitary error info
 * into it to a limit of `CL_RD_PARSER_EMAX'.
 */
struct rd_parser_error *
rd_parser_error(int kind, struct utillib_token_scanner *input);

/**
 * \function rd_parser_error_destroy
 * Destroy this error.
 */
void rd_parser_error_destroy(struct rd_parser_error *self);

void rd_parser_skip_target_init(struct rd_parser_skip_target * self,
    size_t expected);

size_t rd_parser_skipto(struct rd_parser_skip_target const *self,
    struct utillib_token_scanner *input); 

struct rd_parser_error *rd_parser_expected_error(
    struct utillib_token_scanner *input,
    size_t expected,
    size_t actual, size_t context);

struct rd_parser_error *
rd_parser_noargs_error(struct utillib_token_scanner *input,
                             char const *name);

struct rd_parser_error *
rd_parser_unexpected_error(struct utillib_token_scanner *input,
                                 size_t unexpected,
                                 size_t context);

void rd_parser_error_print(struct rd_parser_error const *error);

void rd_parser_insert_const(struct cling_rd_parser *self,
    struct utillib_token_scanner * input,
    struct utillib_json_value * object);

#endif /* CLING_RD_PARSER_IMPH_H */
