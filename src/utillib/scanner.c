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
#include "scanner.h"
#include "symbol.h"
#include <ctype.h>
#include <stdlib.h> // bsearch
#include <string.h> // strcmp

static int keyword_pair_cmp(struct utillib_keyword_pair const *lhs,
                            struct utillib_keyword_pair const *rhs) {
  return strcmp(lhs->key, rhs->key);
}

size_t utillib_keyword_bsearch(char const *key,
                               struct utillib_keyword_pair const *table,
                               size_t nmemb) {
  struct utillib_keyword_pair input;
  input.key = key;
  struct utillib_keyword_pair const *pair =
      bsearch(&input, table, nmemb, sizeof table[0], (void *)keyword_pair_cmp);
  if (!pair)
    return UT_SYM_NULL;
  return pair->value;
}

/*
 * utillib_symbol_scanner
 */
const struct utillib_scanner_op utillib_symbol_scanner_op = {
    .lookahead = (void *)utillib_symbol_scanner_lookahead,
    .shiftaway = (void *)utillib_symbol_scanner_shiftaway,
};

#define symbol_scanner_deref(self) (*(self)->symbols)
#define symbol_scanner_eof(self) (UT_SYM_EOF == symbol_scanner_deref(self))

void utillib_symbol_scanner_init(struct utillib_symbol_scanner *self,
                                 size_t const *symbols,
                                 struct utillib_symbol const *table) {
  self->symbols = symbols;
  self->table = table;
}

inline size_t
utillib_symbol_scanner_lookahead(struct utillib_symbol_scanner *self) {
  return symbol_scanner_deref(self);
}

void utillib_symbol_scanner_shiftaway(struct utillib_symbol_scanner *self) {
  if (symbol_scanner_eof(self))
    return;
  ++self->symbols;
}

inline bool
utillib_symbol_scanner_reacheof(struct utillib_symbol_scanner *self) {
  return symbol_scanner_eof(self);
}

inline void
utillib_symbol_scanner_semantic(struct utillib_symbol_scanner *self,
                                struct utillib_symbol const **value) {
  *value = &self->table[symbol_scanner_deref(self)];
}

/*
 * utillib_char_scanner
 */
void utillib_char_scanner_init(struct utillib_char_scanner *self, FILE *file) {
  self->file = file;
  self->col = 0;
  self->row = 0;
  self->ch = fgetc(file);
}

size_t utillib_char_scanner_lookahead(struct utillib_char_scanner *self) {
  return self->ch;
}

void utillib_char_scanner_shiftaway(struct utillib_char_scanner *self) {
  self->ch = fgetc(self->file);
  if (self->ch == '\n') {
    self->col = 0;
    ++self->row;
  } else {
    ++self->col;
  }
}

inline bool utillib_char_scanner_reacheof(struct utillib_char_scanner *self) {
  return self->ch == EOF;
}

void utillib_char_scanner_destroy(struct utillib_char_scanner *self) {
  if (self->file != stdin)
    fclose(self->file);
}

/*
 * utillib_token_scanner
 */
const struct utillib_scanner_op utillib_token_scanner_op = {
    .lookahead = utillib_symbol_scanner_lookahead,
    .shiftaway = utillib_symbol_scanner_shiftaway,
};

static void token_scanner_error_init(struct utillib_token_scanner_error *self,
                                     int kind, char victim, size_t row,
                                     size_t col) {
  self->kind = kind;
  self->victim = victim;
  self->row = row;
  self->col = col;
}

static void token_scanner_read_input(struct utillib_token_scanner *self) {
  struct utillib_string *buffer = &self->buffer;
  struct utillib_char_scanner *chars = &self->chars;
  struct utillib_token_scanner_callback const *callback = self->callback;
  struct utillib_token_scanner_error error;

  if (utillib_char_scanner_reacheof(chars)) {
    self->code = UT_SYM_EOF;
    return;
  }

  while (true) {
    int code = callback->read_handler(chars, buffer);
    if (code >= 0) { /* UT_SYM_EOF == 0 */
      self->code = code;
      return;
    }
    char victim = utillib_char_scanner_lookahead(chars);
    token_scanner_error_init(&error, -code, victim, chars->row, chars->col);
    int recovery = callback->error_handler(chars, &error);
    if (0 == recovery)
      continue;
    self->code = UT_SYM_ERR;
    return;
  }
}

void utillib_token_scanner_init(
    struct utillib_token_scanner *self, FILE *file,
    struct utillib_token_scanner_callback const *callback) {
  utillib_char_scanner_init(&self->chars, file);
  utillib_string_init(&self->buffer);
  self->callback = callback;
  token_scanner_read_input(self);
}

void utillib_token_scanner_destroy(struct utillib_token_scanner *self) {
  utillib_string_destroy(&self->buffer);
  utillib_char_scanner_destroy(&self->chars);
}

inline size_t
utillib_token_scanner_lookahead(struct utillib_token_scanner const *self) {
  return self->code;
}

inline char const *
utillib_token_scanner_semantic(struct utillib_token_scanner const *self) {
  return utillib_string_c_str(&self->buffer);
}

void utillib_token_scanner_shiftaway(struct utillib_token_scanner *self) {
  utillib_string_clear(&self->buffer);
  token_scanner_read_input(self);
}

inline bool utillib_token_scanner_reacheof(struct utillib_token_scanner const *self) {
  return self->code == UT_SYM_EOF;
}

/*
 * utillib_string_scanner
 */
void utillib_string_scanner_init(struct utillib_string_scanner *self,
                                 char const *str) {
  self->str = str;
  self->pos = 0;
}

inline size_t
utillib_string_scanner_lookahead(struct utillib_string_scanner *self) {
  return *self->str;
}

void utillib_string_scanner_shiftaway(struct utillib_string_scanner *self) {
  if (*self->str == '\0')
    return;
  ++self->str;
  ++self->pos;
}

inline bool
utillib_string_scanner_reacheof(struct utillib_string_scanner *self) {
  return *self->str == '\0';
}

/*
 * Helpers of utillib_token_scanner
 */
static void token_scanner_collect_char(struct utillib_string *buffer,
                                       struct utillib_char_scanner *chars) {
  char ch = utillib_char_scanner_lookahead(chars);
  utillib_string_append_char(buffer, ch);
  utillib_char_scanner_shiftaway(chars);
}

void utillib_token_scanner_skipspace(struct utillib_char_scanner *chars) {
  while (isspace(utillib_char_scanner_lookahead(chars))) {
    utillib_char_scanner_shiftaway(chars);
  }
}

static bool is_idmiddle(int ch) { return ch == '_' || isalnum(ch); }
bool utillib_token_scanner_isidbegin(int ch) {
  return ch == '_' || isalpha(ch);
}

void utillib_token_scanner_collect_identifier(
    struct utillib_char_scanner *chars, struct utillib_string *buffer) {
  token_scanner_collect_char(buffer, chars);
  while (is_idmiddle(utillib_char_scanner_lookahead(chars))) {
    token_scanner_collect_char(buffer, chars);
  }
}

void utillib_token_scanner_collect_digit(struct utillib_char_scanner *chars,
                                         struct utillib_string *buffer) {
  token_scanner_collect_char(buffer, chars);
  while (isdigit(utillib_char_scanner_lookahead(chars))) {
    token_scanner_collect_char(buffer, chars);
  }
}
