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
#define CL_RD_PARSER_EMAX 4
#define cling_symbol_cast(code) \
((code) == UT_SYM_EOF ? UTILLIB_SYMBOL_EOF : &cling_symbols[(code)])

/*
 * Error for RD parser defined here
 */

UTILLIB_ENUM_BEGIN(cling_rd_parser_error_kind)
  UTILLIB_ENUM_ELEM_INIT(CL_EAFMAIN, 1)
  UTILLIB_ENUM_ELEM(CL_EEXPECT)
  UTILLIB_ENUM_ELEM(CL_ENOARGS)
UTILLIB_ENUM_END(cling_rd_parser_error_kind);

struct cling_rd_parser_error {
  int kind;
  size_t row;
  size_t col;
  void const * einfo[CL_RD_PARSER_EMAX];
};

/**
 * \function cling_rd_parser_error
 * Creates an error recording the kind of
 * the error, the location in the source file.
 * Notes that caller can store arbitary error info
 * into it to a limit of `CL_RD_PARSER_EMAX'.
 */
struct cling_rd_parser_error *
cling_rd_parser_error(int kind, struct utillib_token_scanner * input);

/**
 * \function cling_rd_parser_expect
 * Expects a token from the input and returns
 * whether expectation failed.
 * The caller should check like:
 * `if (cling_rd_parser_expect(...)) {
 *    // error...
 * }'
 */
bool cling_rd_parser_expect(
    struct utillib_token_scanner *input, 
    size_t expected);

/**
 * \function cling_rd_parser_error_destroy
 * Destroy this error.
 */
void cling_rd_parser_error_destroy(
    struct cling_rd_parser_error *self);

/**
 * \function cling_rd_parser_skipto
 * Reads and discards token from input until
 * the lookahead token become `target'.
 * Returns whether unfortunately before we can
 * skip to `target', eof was reached.
 * This indicates a fatal error and caller should
 * signal it.
 */
bool cling_rd_parser_skipto(struct utillib_token_scanner *input, 
    size_t target);

struct cling_rd_parser_error *
cling_rd_parser_expected_error(
    struct utillib_token_scanner *input,
    struct utillib_symbol const *expected,
    struct utillib_symbol const *actual,
    struct utillib_symbol const *context);

struct cling_rd_parser_error *
cling_rd_parser_noargs_error(
    struct utillib_token_scanner *input,
    char const * name);

#endif /* CLING_RD_PARSER_IMPH_H */

