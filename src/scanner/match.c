#include "match.h"
#include "scanner.h"
#include "util.h"

// this file provides common match function
static int scanner_linear_search_string(scanner_base_t *self) {
  char const *str = utillib_string_c_str(&(self->sc_str));
  scanner_str_entry_t const *e;
  for (e = self->sc_tab; e->se_str != NULL; ++e) {
    if (0 == strcmp(e->se_str, str)) {
      self->sc_val = e->se_val;
      return SCANNER_MATCHED;
    }
  }
  return SCANNER_UNMATCHED;
}

int scanner_match_identifier(scanner_base_t *self) {
  int c = scanner_getc(self);
  int r = SCANNER_UNMATCHED;
  if (isidbegin(c)) {
    r = SCANNER_MATCHED;
    do {
      c = scanner_getc(self);
    } while (isidmiddle(c));
    if (self->sc_flags & SCANNER_MATCH_STR_AS_ID) {
      return scanner_linear_search_string(self);
    }
  }
  scanner_ungetc(self, c);
  return r;
}

int scanner_match_string(scanner_base_t *self, const char *str) {
  char const *s;
  int c;
  for (s = str, c = scanner_getc(self); *s && *s == (char)c;
       c = scanner_getc(self), ++s)
    ;
  if (*s) {
    scanner_ungets(self);
    return SCANNER_UNMATCHED;
  }
  return SCANNER_MATCHED;
}

int scanner_match_string_literal(scanner_base_t *self, int quote) {
  int state = 0;
  int c = scanner_getc(self);
  if (c == quote) {
    while (true) {
      switch (state) {
      case 0:
        c = scanner_getc(self);
        if (c == quote) {
          state = 1;
        } else if (c == '\\') {
          state = 2;
        } else if (c == '\n') {
          state = 3;
        }
        break;
      case 1:
        return SCANNER_MATCHED;
      case 2:
        scanner_getc(self);
        state = 0;
        break;
      case 3:
        scanner_ungetc(self, c);
        return SCANNER_ERROR;
      }
    }
  }
  scanner_ungetc(self, c);
  return SCANNER_UNMATCHED;
}

int scanner_match_string_single(scanner_base_t *self) {
  return scanner_match_string_literal(self, '\'');
}

int scanner_match_string_double(scanner_base_t *self) {
  return scanner_match_string_literal(self, '\"');
}
