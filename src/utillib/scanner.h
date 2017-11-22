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
#ifndef UTILLIB_SCANNER_H
#define UTILLIB_SCANNER_H
#include "config.h" /* for bool */
#include "string.h"
#include <stddef.h> /* size_t */
#include <stdio.h>

/**
 * \file utillib/scanner.h
 * Common scanner facility.
 * Simplifies scanner writing.
 */

/**
 * \struct utillib_keyword_pair
 * Aids searching for keywords of
 * a language.
 * Define an array of it in sorted
 * order and search it.
 */
struct utillib_keyword_pair {
  char const *key;
  size_t value;
};

/**
 * \struct utillib_scanner_op
 * Function table that abstracts a scanner
 * for what a parser may concern about.
 * Any scanner that is meant to play with
 * the parser of utillib should fill in
 * the blank of this table.
 */
struct utillib_scanner_op {
  size_t (*lookahead)(void *);
  void (*shiftaway)(void *);
  bool (*reacheof)(void *);
  void const *(*semantic)(void *);
};

/**
 * \struct utillib_symbol_scanner
 * A scanner that scans upon a
 * const size_t array that ends
 * with `UT_SYM_EOF'.
 * Its main purpose here is for
 * somehow mocking the scanner part
 * for a parser to be tested.
 */

struct utillib_symbol_scanner {
  size_t const *symbols;
  struct utillib_symbol const *table;
};

/**
 * \struct utillib_char_scanner
 * Scans a single char from `file' each time
 * and maintains row and column counting, which
 * is needed for anything related to source location,
 * i.e., error reporting.
 * Its purpose is for implementation of other scanner
 * and thus does not have its `utillib_scanner_op'
 * function table.
 */

struct utillib_char_scanner {
  int ch;
  FILE *file;
  size_t col;
  size_t row;
};

/**
 * \struct utillib_token_scanner_error
 * This is error that the client of
 * token_scanner will know when it hit.
 */
struct utillib_token_scanner_error {
  int kind;
  char victim;
  size_t row;
  size_t col;
};

/**
 * This callback should be implemented
 * as returning non-negative value for
 * successful token and return `-ERRORCODE`
 * for error token.
 * Please use the lookahead and shiftaway of
 * the `chars' properly.
 * Optionally you can store these chars into
 * `buffer' using `utillib_string_append_char'.
 * There is also a `utillib_token_scanner_collect'
 * provided to combine `append_char' and `shiftaway'
 * in one action, but please use it with care!
 *
 */

/**
 * This callback informs the client a scanner error.
 * The `chars' of the scanner is also passed in
 * so client can attempt kind of recovery here.
 * If the error is unreverable, the client can
 * tell the scanner to stop at it by returning
 * a non-zero value. Then the scanner will return
 * `UT_SYM_ERR'.
 * Or else client can tell the scanner to skip this
 * error by returning 0.
 */

struct utillib_token_scanner_callback {
  int (*read_handler)(struct utillib_char_scanner *chars,
                      struct utillib_string *buffer);
  int (*error_handler)(struct utillib_char_scanner *chars,
                       struct utillib_token_scanner_error const *error);
  void const *(*semantic_handler)(size_t value, char const *str);
};

/**
 * \struct utillib_token_scanner
 * A scanner that aims to form token from
 * chars and to be configurable by client.
 * The main reason I put such a generic struct
 * here regardless of speed or the size of the
 * struct is that I want all the scanning logic
 * to be written in one callback and the rest
 * of scanning is set hereby.
 */

struct utillib_token_scanner {
  struct utillib_char_scanner chars;
  struct utillib_string buffer;
  struct utillib_token_scanner_callback const *callback;
  size_t code;
};

/**
 * \struct utillib_string_scanner
 * Does scanning on a `char const*'.
 */
struct utillib_string_scanner {
  char const *str;
  size_t pos;
};

/**
 * \function utillib_keyword_bsearch
 * Does binary search in the key-value table
 * and returns the value if it was found, or else
 * `UT_SYM_NULL' if not.
 * \param key Normally keywords are itself identifiers
 * so this string should be valid identifier.
 * \param table Where a bunch of `{"keyword", VALUE}'s
 * are defined.
 * \param nmemb The size of `table'.
 */

size_t utillib_keyword_bsearch(char const *key,
                               struct utillib_keyword_pair const *table,
                               size_t nmemb);

/*
 * utillib_symbol_scanner
 */

extern const struct utillib_scanner_op utillib_symbol_scanner_op;

void utillib_symbol_scanner_init(struct utillib_symbol_scanner *self,
                                 size_t const *symbols,
                                 struct utillib_symbol const *table);
/**
 * \function utillib_symbol_scanner_semantic
 * Returns pointer to a `struct utillib_symbol' corresponding
 * to the lookahead value as semantic of this token.
 */
void const *
utillib_symbol_scanner_semantic(struct utillib_symbol_scanner *self);
size_t utillib_symbol_scanner_lookahead(struct utillib_symbol_scanner *self);
void utillib_symbol_scanner_shiftaway(struct utillib_symbol_scanner *self);
bool utillib_symbol_scanner_reacheof(struct utillib_symbol_scanner *self);

/*
 * utillib_char_scanner
 */
void utillib_char_scanner_init(struct utillib_char_scanner *self, FILE *file);
size_t utillib_char_scanner_lookahead(struct utillib_char_scanner *self);
void utillib_char_scanner_shiftaway(struct utillib_char_scanner *self);
bool utillib_char_scanner_reacheof(struct utillib_char_scanner *self);
void utillib_char_scanner_destroy(struct utillib_char_scanner *self);

/*
 * utillib_token_scanner
 */
extern const struct utillib_scanner_op utillib_token_scanner_op;

void utillib_token_scanner_init(
    struct utillib_token_scanner *self, FILE *file,
    struct utillib_token_scanner_callback const *callback);
void utillib_token_scanner_destroy(struct utillib_token_scanner *self);
size_t utillib_token_scanner_lookahead(struct utillib_token_scanner *self);
void const *utillib_token_scanner_semantic(struct utillib_token_scanner *self);
void utillib_token_scanner_shiftaway(struct utillib_token_scanner *self);
bool utillib_token_scanner_reacheof(struct utillib_token_scanner *self);

/*
 * Helpers of utillib_token_scanner
 */
void utillib_token_scanner_skipspace(struct utillib_char_scanner *chars);
bool utillib_token_scanner_isidbegin(int ch);
void utillib_token_scanner_collect_identifier(
    struct utillib_char_scanner *chars, struct utillib_string *buffer);
void utillib_token_scanner_collect_digit(struct utillib_char_scanner *chars,
                                         struct utillib_string *buffer);

/*
 * utillib_string_scanner
 */
void utillib_string_scanner_init(struct utillib_string_scanner *self,
                                 char const *str);
size_t utillib_string_scanner_lookahead(struct utillib_string_scanner *self);
void utillib_string_scanner_shiftaway(struct utillib_string_scanner *self);
bool utillib_string_scanner_reacheof(struct utillib_string_scanner *self);

#endif /* UTILLIB_SCANNER_H */
