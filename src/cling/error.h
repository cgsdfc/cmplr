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
#ifndef CLING_ERROR_H
#define CLING_ERROR_H
#include <utillib/enum.h>
#include <utillib/scanner.h>
#define CLING_ERROR_EMAX 4

UTILLIB_ENUM_BEGIN(cling_error_kind)
UTILLIB_ENUM_ELEM_INIT(CL_EAFMAIN, 1)
UTILLIB_ENUM_ELEM(CL_EEXPECT)
UTILLIB_ENUM_ELEM(CL_ENOARGS)
UTILLIB_ENUM_ELEM(CL_EUNEXPECTED)
UTILLIB_ENUM_ELEM(CL_EREDEFINED)
UTILLIB_ENUM_ELEM(CL_EVAGUE)
UTILLIB_ENUM_ELEM(CL_EEXPR)
UTILLIB_ENUM_ELEM(CL_EPREMATURE)
UTILLIB_ENUM_END(cling_error_kind);

struct cling_error {
  int kind;
  size_t row;
  size_t col;
  char const *einfo[CLING_ERROR_EMAX];
};

/**
 * \function cling_error
 * Creates an error recording the kind of
 * the error, the location in the source file.
 */
struct cling_error *
cling_error_create(int kind, struct utillib_token_scanner *input);

/**
 * \function cling_error_destroy
 * Destroy this error.
 */
void cling_error_destroy(struct cling_error *self);

struct cling_error *cling_expected_error(
    struct utillib_token_scanner *input,
    size_t expected,
    size_t actual, size_t context);

struct cling_error *
cling_noargs_error(struct utillib_token_scanner *input,
                             char const *name);

struct cling_error *
cling_unexpected_error(struct utillib_token_scanner *input,
                                 size_t unexpected,
                                 size_t context);
struct cling_error *
cling_redined_error(struct utillib_token_scanner *input,
    char const * name,
    size_t context);

struct cling_error *
cling_vague_error(struct utillib_token_scanner *input,
    char const * errmsg);

void cling_error_print(struct cling_error const *self);

struct cling_error *
cling_premature_error(struct utillib_token_scanner *input,
    size_t context);

struct cling_error *
cling_expr_error(struct utillib_token_scanner *input);


#endif /* CLING_ERROR_H */

