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
#include "error.h"
#include "option.h"
#include "rd_parser.h"
#include "symbols.h"
#include <assert.h>
#include <ctype.h>
#define CLING_KW_SIZE 14

static const struct utillib_keyword_pair cling_keywords[] = {
    {"case", SYM_KW_CASE},     {"char", SYM_KW_CHAR},
    {"const", SYM_KW_CONST},   {"default", SYM_KW_DEFAULT},
    {"else", SYM_KW_ELSE},     {"for", SYM_KW_FOR},
    {"if", SYM_KW_IF},         {"int", SYM_KW_INT},
    {"main", SYM_KW_MAIN},     {"printf", SYM_KW_PRINTF},
    {"return", SYM_KW_RETURN}, {"scanf", SYM_KW_SCANF},
    {"switch", SYM_KW_SWITCH}, {"void", SYM_KW_VOID},
};

static int read_char(struct cling_scanner *self) {
  char ch;
  ch = utillib_char_scanner_lookahead(&self->input);
  if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '_' ||
      isalnum(ch)) {
    utillib_string_append_char(&self->buffer, ch);
    utillib_char_scanner_shiftaway(&self->input);
    if (utillib_char_scanner_lookahead(&self->input) != '\'') {
      return -CL_EUNTCHAR;
    }
    utillib_char_scanner_shiftaway(&self->input);
    return SYM_CHAR;
  }
  return -CL_ECHRCHAR;
}

static int read_string(struct cling_scanner *self) {
  char ch;
  for (; (ch = utillib_char_scanner_lookahead(&self->input)) != '\"';
       utillib_char_scanner_shiftaway(&self->input)) {
    if (utillib_char_scanner_reacheof(&self->input))
      return -CL_EUNTSTR;
    if (ch == 32 || ch == 33 || 35 <= ch && ch <= 126) {
      utillib_string_append_char(&self->buffer, ch);
      continue;
    }
    return -CL_ESTRCHAR;
  }
  utillib_char_scanner_shiftaway(&self->input);
  return SYM_STRING;
}

static int read_number(struct cling_scanner *self, char ch) {
  char next_char;
  switch (ch) {
  case '+':
  case '-':
    utillib_char_scanner_shiftaway(&self->input);
    if (self->context == SYM_EXPR)
      goto match_op;
    next_char = utillib_char_scanner_lookahead(&self->input);
    if (isdigit(next_char)) {
      utillib_string_append_char(&self->buffer, ch);
      goto match_integer;
    }
    goto match_op;
  default:
    assert(isdigit(ch));
    goto match_integer;
  }
match_integer:
  utillib_token_scanner_collect_digit(&self->input, &self->buffer);
  return SYM_INTEGER;

match_op:
  return ch == '+' ? SYM_ADD : SYM_MINUS;
}

static int read_dispatch(struct cling_scanner *self) {
  char ch=utillib_char_scanner_lookahead(&self->input);
  int code = UT_SYM_NULL;
  char const *keyword;
  int two_chars, one_char;
  switch (ch) {
  /*
   * Level 1 dispatch: single char.
   */
  case '[':
    code = SYM_LK;
    break;
  case ']':
    code = SYM_RK;
    break;
  case '{':
    code = SYM_LB;
    break;
  case '}':
    code = SYM_RB;
    break;
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
  case '/':
    code = SYM_DIV;
    break;
  case '*':
    code = SYM_MUL;
    break;
  case ':':
    code = SYM_COLON;
    break;
  default:
    break;
  }
  if (code != UT_SYM_NULL) {
    utillib_char_scanner_shiftaway(&self->input);
    return code;
  }
  switch (ch) {
  /*
   * Level 2 dispatch: two chars.
   */
  case '=':
    two_chars = SYM_DEQ;
    one_char = SYM_EQ;
    goto level2;
  case '<':
    two_chars = SYM_LE;
    one_char = SYM_LT;
    goto level2;
  case '>':
    two_chars = SYM_GE;
    one_char = SYM_GT;
    goto level2;
  case '!':
    two_chars = SYM_NE;
    one_char = -CL_EBADNEQ;
    goto level2;
  default:
    goto level3;
  }

level2:
  utillib_char_scanner_shiftaway(&self->input);
  if (utillib_char_scanner_lookahead(&self->input) == '=') {
    utillib_char_scanner_shiftaway(&self->input);
    return two_chars;
  }
  return one_char;

level3:
  /*
   * Level 3 : keyword and iden.
   */
  if (utillib_token_scanner_isidbegin(ch)) {
    utillib_token_scanner_collect_identifier(&self->input, &self->buffer);
    keyword = utillib_string_c_str(&self->buffer);
    code = utillib_keyword_bsearch(keyword, cling_keywords, CLING_KW_SIZE);
    if (code != UT_SYM_NULL)
      return code;
    return SYM_IDEN;
  }

  /*
   * Level 4 : char, string and number
   */
  if (ch == '\"') {
    utillib_char_scanner_shiftaway(&self->input);
    return read_string(self);
  }
  if (ch == '\'') {
    utillib_char_scanner_shiftaway(&self->input);
    return read_char(self);
  }
  if (isdigit(ch) || ch == '+' || ch == '-')
    return read_number(self, ch);
  return -CL_EUNKNOWN;
}

void cling_scanner_init(struct cling_scanner *self,
                        struct cling_option const *option,
                        struct cling_rd_parser *parser, FILE *file) {
  self->option = option;
  self->parser = parser;
  self->context = SYM_PROGRAM;
  utillib_char_scanner_init(&self->input, file);
  utillib_string_init(&self->buffer);
  self->lookahead=read_dispatch(self);
}

void cling_scanner_destroy(struct cling_scanner *self) {
  utillib_string_destroy(&self->buffer);
  utillib_char_scanner_destroy(&self->input);
}

inline void cling_scanner_context(struct cling_scanner *self, size_t context) {
  self->context = context;
}

inline size_t cling_scanner_lookahead(struct cling_scanner const *self) {
  return self->lookahead;
}

static void skipcomment(struct cling_scanner *self) {
  char ch;
  while ((ch = utillib_char_scanner_lookahead(&self->input)) == '#') {
    while ((ch = utillib_char_scanner_lookahead(&self->input)) != '\n')
      utillib_char_scanner_shiftaway(&self->input);
    utillib_token_scanner_skipspace(&self->input);
  }
}

void cling_scanner_shiftaway(struct cling_scanner *self) {
  int code;
  if (utillib_char_scanner_reacheof(&self->input))
    return;
  utillib_token_scanner_skipspace(&self->input);
  if (self->option->allow_comment)
    skipcomment(self);
  utillib_string_clear(&self->buffer);
  code = read_dispatch(self);
  if (code < 0) {
    rd_parser_error_push_back(
        self->parser,
        cling_badtoken_error(self, -code, utillib_string_c_str(&self->buffer),
                             self->context));
    code=UT_SYM_ERR;
  }
  self->lookahead = code;
}

inline char const *cling_scanner_semantic(struct cling_scanner const *self) {
  return utillib_string_c_str(&self->buffer);
}
