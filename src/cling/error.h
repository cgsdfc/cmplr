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
UTILLIB_ENUM_ELEM(CL_EUNEXPECTED)
UTILLIB_ENUM_ELEM(CL_EREDEFINED)
UTILLIB_ENUM_ELEM(CL_EINCTYPE)
UTILLIB_ENUM_ELEM(CL_EUNDEFINED)
UTILLIB_ENUM_ELEM(CL_ENOTLVALUE)
UTILLIB_ENUM_END(cling_error_kind);

struct cling_error {
  int kind;
  char const *context;
  size_t row;
  size_t col;
  char const *einfo[CLING_ERROR_EMAX];
};

/**
 * \function cling_error_destroy
 * Destroy this error.
 */
void cling_error_destroy(struct cling_error *self);

struct cling_error *cling_expected_error(struct utillib_token_scanner *input,
                                         size_t expected, size_t context);

struct cling_error *cling_unexpected_error(struct utillib_token_scanner *input,
                                           size_t context);

struct cling_error *cling_redefined_error(struct utillib_token_scanner *input,
                                        char const *name, size_t context);

struct cling_error *cling_undefined_name_error(struct utillib_token_scanner *input,
    char const *name, size_t context);

struct cling_error *cling_not_lvalue_error(struct utillib_token_scanner *input,
    char const *name, size_t context);

struct cling_error * cling_incompatible_type_error(struct utillib_token_scanner *input, 
    int actual_type, int expected_type, size_t context);

void cling_error_print(struct cling_error const *self);

#endif /* CLING_ERROR_H */
