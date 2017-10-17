#include "scanner.h"
#include <assert.h>

UTILLIB_ETAB_BEGIN(scanner_retval_t)
UTILLIB_ETAB_ELEM_INIT(SCANNER_ANY_CHAR, "any char")
UTILLIB_ETAB_ELEM_INIT(SCANNER_MATCHED, "matched token")
UTILLIB_ETAB_ELEM_INIT(SCANNER_EOF, "end of input")
UTILLIB_ETAB_ELEM_INIT(SCANNER_UNMATCHED, "unmatched token")
UTILLIB_ETAB_ELEM_INIT(SCANNER_ERROR, "scanner error")
UTILLIB_ETAB_END(scanner_retval_t)

// not appending internal str
int scanner_ungetchar(scanner_base_t *self, int c) {
  return utillib_char_buf_ungetc(self->sc_char_buf, c, self->sc_cb_ft);
}

// not erasing internal str
int scanner_getchar(scanner_base_t *self) {
  int c = utillib_char_buf_getc(self->sc_char_buf, self->sc_cb_ft);
  return c;
}

static void scanner_str_clear(scanner_base_t *self) {
  utillib_string_clear(&(self->sc_str));
}

void scanner_init(scanner_base_t *self, void *char_buf,
                 utillib_char_buf_ft const *cb_ft,
                 scanner_match_entry_t const *match,
                 scanner_str_entry_t const *tab, int flags) {
  utillib_string_init(&(self->sc_str));
  self->sc_char_buf = char_buf;
  self->sc_cb_ft = cb_ft;
  self->sc_match = match;
  self->sc_flags = flags;
  if (flags & SCANNER_MATCH_STR_AS_ID) {
    self->sc_tab = tab;
  }
  return 0;
}

int scanner_getc(scanner_base_t *self) {
  int c = scanner_getchar(self);
  utillib_string_append(&(self->sc_str), c);
  return c;
}

int scanner_ungetc(scanner_base_t *self, int c) {
  int ch = scanner_ungetchar(self, c);
  utillib_string_erase_last(&(self->sc_str));
  return ch;
}

void scanner_ungets(scanner_base_t *self) {
  // unget the currently collected char
  char const *s = utillib_string_c_str(&(self->sc_str));
  while (*s) {
    scanner_ungetc(self, *s);
    ++s;
  }
}

static int scanner_try_match(scanner_base_t *self) {
  scanner_match_entry_t const *match;
  scanner_str_clear(self);
  int c;
  for (match = self->sc_match; match->scm_func != NULL || match->scm_val != 0;
       ++match) {
    int r;
    switch (r = match->scm_func(self)) {
    case SCANNER_MATCHED:
      self->sc_text = utillib_string_c_str(&(self->sc_str));
      if (self->sc_flags & SCANNER_MATCH_STR_AS_ID)
        ; /* done in scanner_linear_search_string */
      else {
        self->sc_val = match->scm_val;
      }
      return r;
    case SCANNER_UNMATCHED:
      continue;
    case SCANNER_ERROR:
      self->sc_val = 0;
      return SCANNER_ERROR;
    }
  }
  c = scanner_getc(self);
  if (c == EOF) {
    return SCANNER_EOF;
  }
  self->sc_val = c;
  if (self->sc_flags & SCANNER_MATCH_ANY_CHAR) {
    return SCANNER_ANY_CHAR;
  }
  return SCANNER_ERROR;
}

char const *scanner_get_text(scanner_base_t *self) { return self->sc_text; }
int scanner_get_val(scanner_base_t *self) { return self->sc_val; }

int scanner_yylex(scanner_base_t *self) { return scanner_try_match(self); }

void scanner_skip_to(scanner_base_t *self, int c) {
  int ch;
  do {
    ch = scanner_getchar(self);
  } while (ch != c);
}

void scanner_destroy(scanner_base_t *self) {
  // client should destroy the self->sc_char_buf 
  // itself
  utillib_string_destroy(&(self->sc_str));
}
