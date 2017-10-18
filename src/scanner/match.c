#include "match.h"
#include "scanner.h"
#include <ctype.h>
#include <string.h>

/* if c is [_a-zA-Z] */
static int isidbegin(int c) { return isalpha(c) || c == '_'; }
/* if c is [_a-zA-z0-9] */
static int isidmiddle(int c) { return isalnum(c) || c == '_'; }

/* search linearly in a str-val tab to match the identifier */
/* against a fix string */
/* usually used for reserved words. may perfect hasing provides */
/* better efficiency */
static void const *linear_search_string(scanner_str_entry_t const *entries,
                                        char const *str) {
  scanner_str_entry_t const *e;
  for (e = entries; UTILLIB_PAIR_FIRST(e) != NULL; ++e) {
    if (0 == strcmp(UTILLIB_PAIR_FIRST(e), str)) {
      return e;
    }
  }
  return NULL;
}

scanner_str_entry_t const *
scanner_search_string(scanner_str_entry_t const *entries, char const *str) {
  return linear_search_string(entries, str);
}

/* match an identifier = idbegin idmiddle * */
/* also searches for fix string */
int scanner_match_identifier(scanner_base_t *self) {
  int c = scanner_getc(self);
  int r = SCANNER_UNMATCHED;
  if (isidbegin(c)) {
    r = SCANNER_MATCHED;
    do {
      c = scanner_getc(self);
    } while (isidmiddle(c));
  }
  scanner_ungetc(self, c);
  return r;
}

// impl of matching string with different delimiters
// DO NOT check for escaped sequence strictly.
// i.e. any escaped sequences will be treated as \x
// where x is an arbitrary single character.
static int match_string_impl(scanner_base_t *self, int left, int right) {
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
        } else if (c == '\n' || c == EOF) {
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

// match <xxxx>
int scanner_match_string_angle(scanner_base_t *self) {
  return match_string_impl(self, '<', '>');
}

// match 'xxxx'
int scanner_match_string_single(scanner_base_t *self) {
  static const int single_quote = '\'';
  return match_string_impl(self, single_quote, single_quote);
}

// match "xxxx"
int scanner_match_string_double(scanner_base_t *self) {
  static const int double_quote = '\"';
  return match_string_impl(self, double_quote, double_quote);
}

// match and skip a cpp style comment
// \/\/.*\n
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

// match and skip a c style comment
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

// this a not a matcher, but a skipper
// that can be passed as scanner_getc_func_t
// to skip all space
int scanner_skip_space(scanner_input_buf *self) {
  int c;
  do {
    c = scanner_input_buf_getc(self);
  } while (isspace(c));
  return c;
}

int scanner_match_any_char(scanner_base_t *self) {
  self->sc_val = scanner_getc(self);
  return SCANNER_MATCHED;
}
