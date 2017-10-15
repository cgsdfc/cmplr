#include "match.h"
#include "scanner.h"
#include "util.h"
#include <ctype.h>
static int isidmiddle(int c) { return isalnum(c) || c == '_'; }

static int isidbegin(int c) { return isalpha(c) || c == '_'; }

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

static int scanner_match_string_impl(scanner_base_t *self, int left,
                                     int right) {
  int state = 0;
  int c = scanner_getc(self);
  if (c == left) {
    while (true) {
      switch (state) {
      case 0:
        c = scanner_getc(self);
        if (c == right) {
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

int scanner_match_string_angle(scanner_base_t *self) {
  return scanner_match_string_impl(self, '<', '>');
}

int scanner_match_string_single(scanner_base_t *self) {
  static const int single_quote = '\'';
  return scanner_match_string_impl(self, single_quote, single_quote);
}

int scanner_match_string_double(scanner_base_t *self) {
  static const int double_quote = '\"';
  return scanner_match_string_impl(self, double_quote, double_quote);
}
int scanner_skip_space(scanner_base_t *self) {
  int c;
  int cnt = 0;
  do {
    c = scanner_getchar(self);
    cnt++;
  } while (isspace(c));
  scanner_ungetchar(self, c);
  return cnt > 1 ? SCANNER_MATCHED : SCANNER_UNMATCHED;
}

int scanner_skip_cpp_comment(scanner_base_t *self) {
  int c;
  if ((c = scanner_getchar(self)) == '/') {
    if ((c = scanner_getchar(self)) == '/') {
      do {
        c = scanner_getchar(self);
      } while (c != '\n');
      return SCANNER_MATCHED;
    }
    scanner_ungetchar(self, c);
  }
  scanner_ungetchar(self, c);
  return SCANNER_UNMATCHED;
}

int scanner_skip_c_comment(scanner_base_t *self) {
  int c;
  int state = 0;
  if ((c = scanner_getchar(self)) == '/') {
    if ((c = scanner_getchar(self)) == '*') {
      while (true) {
        switch (state) {
        case 0:
          c = scanner_getchar(self);
          if (c == '*') {
            state = 1;
          } else if (c == '/') {
            state = 2;
          } else if (c == EOF) {
            state = 4;
          }
          break;
        case 1:
          c = scanner_getchar(self);
          if (c == '/') {
            state = 3;
          } else if (c != '*') {
            state = 0;
          }
          break;
        case 2:
          c = scanner_getchar(self);
          if (c == '*') {
            state = 4;
          } else {
            state = 1;
          }
          break;
        case 3:
          return SCANNER_MATCHED;
        case 4:
          return SCANNER_ERROR;
        }
      }
    }
    scanner_ungetchar(self, c);
  }
  scanner_ungetchar(self, c);
  return SCANNER_UNMATCHED;
}
