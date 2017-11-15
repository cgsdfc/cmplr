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
#define _GNU_SOURCE
#include "scanner.h"
#include "symbols.h"
#include <ctype.h>
#include <stdlib.h> // strtoul free
#include <string.h>
#include <utillib/print.h>

static struct pascal_scanner_error *
scanner_error_create(int kind, char badch, size_t row, size_t col);

UTILLIB_ETAB_BEGIN(pascal_scanner_error_kind)
UTILLIB_ETAB_ELEM_INIT(PASCAL_EBADCHAR, "unrecogized character")
UTILLIB_ETAB_ELEM_INIT(PASCAL_EBADCEQ, "incomplete `:=' token")
UTILLIB_ETAB_END(pascal_scanner_error_kind);

/**
 * Pascal keyword (restricted).
 * Sorted for bsearch.
 */
const struct utillib_keyword_pair pascal_keywords[] = {
    {"begin", SYM_KW_BEGIN}, {"call", SYM_KW_CALL},
    {"const", SYM_KW_CONST}, {"do", SYM_KW_DO},
    {"end", SYM_KW_END},     {"if", SYM_KW_IF},
    {"odd", SYM_KW_ODD},     {"procedure", SYM_KW_PROC},
    {"read", SYM_KW_READ},   {"then", SYM_KW_THEN},
    {"var", SYM_KW_VAR},     {"while", SYM_KW_WHILE},
    {"write", SYM_KW_WRITE},
};
const size_t pascal_keywords_size =
    sizeof pascal_keywords / sizeof pascal_keywords[0];

/**
 * \variable pascal_scanner_op
 * Function table for parser to use.
 * Notes the `reacheof' is not implemented since
 * EOF can be told from value returned by `lookahead'.
 */
struct utillib_scanner_op const pascal_scanner_op = {
    .lookahead = (void *)pascal_scanner_lookahead,
    .shiftaway = (void *)pascal_scanner_shiftaway,
    .semantic = (void *)pascal_scanner_semantic,
};

static bool is_idbegin(int ch) { return ch == '_' || isalpha(ch); }

static bool is_idmiddle(int ch) { return ch == '_' || isalnum(ch); }

/**
 * \function scanner_collect_char
 * Collects the lookahead char into buffer
 * and shifts away that char.
 */
static void scanner_collect_char(struct utillib_string *buffer,
                                 struct utillib_char_scanner *scanner) {
  char ch = utillib_char_scanner_lookahead(scanner);
  utillib_string_append_char(buffer, ch);
  utillib_char_scanner_shiftaway(scanner);
}

/**
 * \function scanner_gettoken
 * Tries to read from input the next token.
 * \param scanner Lookahead and shift single char from it.
 * \param buffer Where the chars of tokens like `iden' or 'uint'
 * are collected.
 * \return `UT_SYM_EOF' if `scanner' reaches EOF.
 * The positive symbol value (token code) if it successfully read
 * a token.
 * A negative `enum pascal_error_kind' number if it failed.
 *
 */
static int scanner_gettoken(struct utillib_char_scanner *scanner,
                            struct utillib_string *buffer) {
  int ch;
  int code = UT_SYM_NULL;
  while (isspace(ch = utillib_char_scanner_lookahead(scanner))) {
    utillib_char_scanner_shiftaway(scanner);
  }
  if (utillib_char_scanner_reacheof(scanner))
    return UT_SYM_EOF;

  switch (ch) {
  case '(':
    code = SYM_LP;
    break;
  case ')':
    code = SYM_RP;
    break;
  case ';':
    code = SYM_SEMI;
    break;
  case ',':
    code = SYM_COMMA;
    break;
  case '-':
    code = SYM_MINUS;
    break;
  case '+':
    code = SYM_ADD;
    break;
  case '/':
    code = SYM_DIV;
    break;
  case '*':
    code = SYM_MUL;
    break;
  case '=':
    code = SYM_EQ;
    break;
  }
  if (code != UT_SYM_NULL) {
    utillib_char_scanner_shiftaway(scanner);
    return code;
  }
  switch (ch) {
  case ':':
    utillib_char_scanner_shiftaway(scanner);
    if ('=' == utillib_char_scanner_lookahead(scanner)) {
      utillib_char_scanner_shiftaway(scanner);
      return SYM_CEQ;
    } else {
      return -PASCAL_EBADCEQ;
    }
  case '<':
    utillib_char_scanner_shiftaway(scanner);
    switch (utillib_char_scanner_lookahead(scanner)) {
    case '>':
      code = SYM_NE;
      break;
    case '=':
      code = SYM_LE;
      break;
    default:
      code = SYM_LT;
      break;
    }
    utillib_char_scanner_shiftaway(scanner);
    return code;
  case '>':
    utillib_char_scanner_shiftaway(scanner);
    if (utillib_char_scanner_lookahead(scanner) == '=') {
      utillib_char_scanner_shiftaway(scanner);
      return SYM_GE;
    }
    return SYM_GT;
  }
  if (is_idbegin(ch)) {
    scanner_collect_char(buffer, scanner);
    while (is_idmiddle(utillib_char_scanner_lookahead(scanner))) {
      scanner_collect_char(buffer, scanner);
    }
    char const *iden = utillib_string_c_str(buffer);
    size_t code =
        utillib_keyword_bsearch(iden, pascal_keywords, pascal_keywords_size);
    return code == UT_SYM_NULL ? SYM_IDEN : code;
  }
  if (isdigit(ch)) {
    scanner_collect_char(buffer, scanner);
    while (isdigit(utillib_char_scanner_lookahead(scanner))) {
      scanner_collect_char(buffer, scanner);
    }
    return SYM_UINT;
  }
  return -PASCAL_EBADCHAR;
}

/**
 * \function scanner_read_input
 * Since `scanner_gettoken' only handles the first thing
 * it encounters, it cannot do things like storing and
 * skipping errors and hence this function.
 * It reads the first non-errored token (including EOF)
 * from input and creating and storing `pascal_scanner_error',
 * skipping error char while going.
 */

static void scanner_read_input(struct pascal_scanner *self) {
  struct utillib_string *buffer = &self->buffer;
  struct utillib_char_scanner *scanner = &self->scanner;

  while (true) {
    int code = scanner_gettoken(scanner, buffer);
    if (code >= 0) { /* UT_SYM_EOF == 0 */
      self->code = code;
      return;
    }
    char badch = utillib_char_scanner_lookahead(scanner);
    utillib_char_scanner_shiftaway(scanner);
    utillib_vector_push_back(
        &self->errors,
        scanner_error_create(-code, badch, scanner->row, scanner->col));
  }
}

/*
 * pascal_scanner_error
 */

static struct pascal_scanner_error *
scanner_error_create(int kind, char badch, size_t row, size_t col) {
  struct pascal_scanner_error *self = malloc(sizeof *self);
  self->badch = badch;
  self->kind = kind;
  self->row = row;
  self->col = col;
  return self;
}

static void scanner_error_destroy(struct pascal_scanner_error *self) {
  free(self);
}

/*
 * Public interface
 */

void pascal_scanner_init(struct pascal_scanner *self, FILE *file) {
  utillib_string_init(&self->buffer);
  utillib_char_scanner_init(&self->scanner, file);
  utillib_vector_init(&self->errors);
  scanner_read_input(self);
}

void pascal_scanner_destroy(struct pascal_scanner *self) {
  utillib_string_destroy(&self->buffer);
  utillib_vector_destroy_owning(&self->errors, (void *)scanner_error_destroy);
}

/*
 * Utillib.Scanner interface
 */

size_t pascal_scanner_lookahead(struct pascal_scanner *self) {
  return self->code;
}

/**
 * \function pascal_scanner_shiftaway
 * Clears the buffer and retrieves the next token
 * from input.
 */
void pascal_scanner_shiftaway(struct pascal_scanner *self) {
  utillib_string_clear(&self->buffer);
  scanner_read_input(self);
}

void const *pascal_scanner_semantic(struct pascal_scanner *self) {
  char const *str = utillib_string_c_str(&self->buffer);
  if (self->code == SYM_IDEN) {
    return strdup(str);
  }
  if (self->code == SYM_UINT) {
    size_t *uint_val = malloc(sizeof *uint_val);
    *uint_val = strtoul(str, NULL, 10);
    return uint_val;
  }
  return NULL;
}

void pascal_scanner_error_print(struct pascal_scanner_error const *self) {
  utillib_error_printf("In %lu:%lu: ERROR: ", self->row, self->col);
  switch (self->kind) {
  case PASCAL_EBADCHAR:
    utillib_error_printf("%s: `%c'\n",
                         pascal_scanner_error_kind_tostring(self->kind),
                         self->badch);
    return;
  case PASCAL_EBADCEQ:
    utillib_error_printf("%s: expected `=', got `%c'\n",
                         pascal_scanner_error_kind_tostring(self->kind),
                         self->badch);
    return;
  }
}

/**
 * \function pascal_scanner_check
 * Checks the size of `self->errors'.
 * If non-zero, prints all these errors.
 * \return Size of errors.
 */
size_t pascal_scanner_check(struct pascal_scanner *self) {
  size_t size = utillib_vector_size(&self->errors);
  if (0 == size)
    return 0;
  UTILLIB_VECTOR_FOREACH(struct pascal_scanner_error const *, err,
                         &self->errors) {
    pascal_scanner_error_print(err);
    utillib_error_printf("\n");
  }
  return size;
}

void pascal_scanner_getpos(struct pascal_scanner const *self,
    size_t *row, size_t *col)
{
  *row=self->scanner.row;
  *col=self->scanner.col;
}

