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
#include <stdlib.h> // bsearch
#include <string.h> // strcmp

/* void *bsearch(const void *key, const void *base, */
/*               size_t nmemb, size_t size, */
/*               int (*compar)(const void *, const void *)); */
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

const struct utillib_scanner_op utillib_symbol_scanner_op = {
    .lookahead = (void *)utillib_symbol_scanner_lookahead,
    .shiftaway = (void *)utillib_symbol_scanner_shiftaway,
    .reacheof = (void *)utillib_symbol_scanner_reacheof,
    .semantic = (void *)utillib_symbol_scanner_semantic,
};

#define symbol_scanner_deref(self) (*(self)->symbols)
#define symbol_scanner_eof(self) (UT_SYM_EOF == symbol_scanner_deref(self))

void utillib_symbol_scanner_init(struct utillib_symbol_scanner *self,
                                 size_t const *symbols,
                                 struct utillib_symbol const *table) {
  self->symbols = symbols;
  self->table = table;
}

size_t utillib_symbol_scanner_lookahead(struct utillib_symbol_scanner *self) {
  return symbol_scanner_deref(self);
}

void utillib_symbol_scanner_shiftaway(struct utillib_symbol_scanner *self) {
  if (symbol_scanner_eof(self))
    return;
  ++self->symbols;
}

bool utillib_symbol_scanner_reacheof(struct utillib_symbol_scanner *self) {
  return symbol_scanner_eof(self);
}

void const *
utillib_symbol_scanner_semantic(struct utillib_symbol_scanner *self) {
  return &self->table[symbol_scanner_deref(self)];
}

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

bool utillib_char_scanner_reacheof(struct utillib_char_scanner *self) {
  return self->ch == EOF;
}
