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
#ifndef PASCAL_SCANNER_H
#define PASCAL_SCANNER_H
#include <stdio.h> // FILE*
#include <utillib/enum.h>
#include <utillib/scanner.h>
#include <utillib/string.h>
#include <utillib/vector.h>

extern const struct utillib_keyword_pair pascal_keywords[];
extern const size_t pascal_keywords_size;

/**
 * \enum
 * Error code for possible scanner error.
 * \value PASCAL_EBADCHAR An unrecognized char.
 * \value PASCAL_EBADCEQ `:' but without '='.
 * Bad comma equal token.
 */
UTILLIB_ENUM_BEGIN(pascal_scanner_error_kind)
UTILLIB_ENUM_ELEM_INIT(PASCAL_EBADCHAR, 1)
UTILLIB_ENUM_ELEM(PASCAL_EBADCEQ)
UTILLIB_ENUM_END(pascal_scanner_error_kind);

extern struct utillib_scanner_op const pascal_scanner_op;

/**
 * \struct pascal_scanner_error
 * Records a piece of scanner error of the above varieties.
 * The `badch' field is for the unrecognized char.
 */
struct pascal_scanner_error {
  int kind;
  int badch;
  size_t row;
  size_t col;
};

/**
 * \struct
 * pascal_scanner
 * Scanner that can recognize the token corresponding
 * to the terminal symbols defined in "symbols.h".
 * `UT_SYM_EOF' can also be recognized.
 */
struct pascal_scanner {
  struct utillib_char_scanner scanner;
  struct utillib_string buffer;
  struct utillib_vector errors;
  size_t code;
};

void pascal_scanner_init(struct pascal_scanner *self, FILE *file);
size_t pascal_scanner_lookahead(struct pascal_scanner *self);
void pascal_scanner_shiftaway(struct pascal_scanner *self);
void const *pascal_scanner_semantic(struct pascal_scanner *self);
void pascal_scanner_destroy(struct pascal_scanner *self);
void pascal_scanner_error_print(struct pascal_scanner_error const *self);
size_t pascal_scanner_check(struct pascal_scanner *self);
void pascal_scanner_getpos(struct pascal_scanner const *self,
    size_t *row, size_t *col);

#endif /* PASCAL_SCANNER_H */
