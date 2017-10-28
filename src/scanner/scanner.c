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
#include "input_buf.h"
#include <assert.h>
#include <utillib/unordered_map.h> // for utillib_unordered_map

UTILLIB_ETAB_BEGIN(scanner_retval_t)
UTILLIB_ETAB_ELEM_INIT(SCANNER_MATCHED, "matched token")
UTILLIB_ETAB_ELEM_INIT(SCANNER_EOF, "end of input")
UTILLIB_ETAB_ELEM_INIT(SCANNER_UNMATCHED, "unmatched token")
UTILLIB_ETAB_ELEM_INIT(SCANNER_ERROR, "scanner error")
UTILLIB_ETAB_END(scanner_retval_t)

// not appending internal str
int scanner_ungetchar(scanner_base_t *self, int c) {
  return scanner_input_buf_ungetc(self->sc_buf, c);
}

// not erasing internal str
int scanner_getchar(scanner_base_t *self) {
  return self->sc_getc(self->sc_buf);
}

static void str_clear(scanner_base_t *self) {
  utillib_string_clear(&self->sc_str);
}

/* scanner_t constructor */
void scanner_init(scanner_t *self, FILE *file, char const *filename,
                  scanner_match_entry_t const *match) {
  // default scanner_t
  scanner_input_buf_init(&self->sc_buf, file, filename);
  scanner_base_init(&self->sc_base, &self->sc_buf, scanner_input_buf_getc,
                    match);
}

/* scanner_base_t constructor */
void scanner_base_init(scanner_base_t *self, scanner_input_buf *buf,
                       scanner_getc_func_t *getc,
                       scanner_match_entry_t const *match) {
  utillib_string_init(&self->sc_str);
  self->sc_val = 0;
  self->sc_text = NULL;
  self->sc_buf = buf;
  self->sc_getc = getc;
  self->sc_match = match;
}

/* scanner_t destructor */
void scanner_destroy(scanner_t *self) {
  scanner_input_buf_destroy(&self->sc_buf);
  scanner_base_destroy(&self->sc_base);
}
/* scanner_base_t destructor */
void scanner_base_destroy(scanner_base_t *self) {
  utillib_string_destroy(&self->sc_str);
}
/* appending internal str */
int scanner_getc(scanner_base_t *self) {
  int c = scanner_getchar(self);
  utillib_string_append_char(&self->sc_str, c);
  return c;
}

/* erasing internal str */
int scanner_ungetc(scanner_base_t *self, int c) {
  int ch = scanner_ungetchar(self, c);
  utillib_string_erase_last(&self->sc_str);
  return ch;
}

/* unget the currently collected char */
void scanner_ungets(scanner_base_t *self) {
  char const *s = utillib_string_c_str(&self->sc_str);
  while (*s) {
    scanner_ungetc(self, *s);
    ++s;
  }
}

/**
 * \function try_match
 * Try to match the input characters with the function pointers
 * in self->sc_match.
 * \return If one match ever succeeded, it sets self->sc_st to
 * point to a read-only buffer, sets self->sc_val to the code and returns
 * SCANNER_MATCHED.
 * Specially, if the scanner_match_any_char returns SCANNER_CHAR, it will assume
 * the character has been put into the self->sc_val and does as about.
 * If none of the functions return SCANNER_MATCHED, it returns
 * SCANNER_UNMATCHED.
 * If one of the match functions returns SCANNER_ERROR, it reports
 * it as it.
*/

static int try_match(scanner_base_t *self) {
  scanner_match_entry_t const *match;
  for (match = self->sc_match; match->scm_func != NULL; ++match) {
    int r;
    switch (r = match->scm_func(self)) {
    case SCANNER_CHAR:
      self->sc_text = utillib_string_c_str(&self->sc_str);
      return SCANNER_MATCHED;
    case SCANNER_MATCHED:
      self->sc_text = utillib_string_c_str(&self->sc_str);
      self->sc_val = match->scm_val;
      return r;
    case SCANNER_UNMATCHED:
      continue;
    case SCANNER_ERROR:
      self->sc_val = match->scm_val;
      return r;
    }
  }
  // since match_func will not consume any char
  // if it does not match(even the trouble char)
  // so we eat that char here;
  scanner_getchar(self);
  return SCANNER_ERROR;
}
int scanner_yylex(scanner_base_t *self) {
  int c = scanner_getchar(self);
  if (c == EOF) {
    return SCANNER_EOF;
  }
  str_clear(self);
  scanner_ungetchar(self, c);
  return try_match(self);
}

char const *scanner_get_text(scanner_base_t *self) { return self->sc_text; }
int scanner_get_val(scanner_base_t *self) { return self->sc_val; }
int scanner_skip_to(scanner_base_t *self, int c) {
  int ch;
  do {
    ch = scanner_getchar(self);
  } while (ch != c);
  return ch;
}

int scanner_read_all(scanner_base_t *self, FILE *file) {
  while (true) {
    utillib_error *err;
    switch (scanner_yylex(self)) {
    case SCANNER_EOF:
      return 0;
    case SCANNER_MATCHED:
      fprintf(file, "`%s':%d\n", scanner_get_text(self), scanner_get_val(self));
      break;
    case SCANNER_ERROR:
      err = scanner_input_buf_make_error(
          self->sc_buf, ERROR_LV_ERROR,
          "scanner_read_all: error when matching, code=%d",
          scanner_get_errc(self));
      scanner_input_buf_pretty_perror(self->sc_buf, err);
      utillib_destroy_error(err);
      return 1;
    }
  }
}
