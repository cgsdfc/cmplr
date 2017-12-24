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

/*
 * XXX: Can't be so easy
 */
static int read_number(struct cling_scanner *self) {
  char ch;
  ch = utillib_char_scanner_lookahead(&self->input);
  utillib_token_scanner_collect_digit(&self->input, &self->buffer);
  return SYM_UINT;
}

static int read_dispatch(struct cling_scanner *self, char ch) {
  int code = UT_SYM_NULL;
  char const *keyword;
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
    utillib_char_scanner_shiftaway(&self->input);
    if (utillib_char_scanner_lookahead(&self->input) == '=') {
      utillib_char_scanner_shiftaway(&self->input);
      return SYM_DEQ;
    }
    return SYM_EQ;
  case '<':
    utillib_char_scanner_shiftaway(&self->input);
    if (utillib_char_scanner_lookahead(&self->input) == '=') {
      utillib_char_scanner_shiftaway(&self->input);
      return SYM_LE;
    }
    return SYM_LT;
  case '>':
    utillib_char_scanner_shiftaway(&self->input);
    if (utillib_char_scanner_lookahead(&self->input) == '=') {
      utillib_char_scanner_shiftaway(&self->input);
      return SYM_GE;
    }
    return SYM_GT;
  case '!':
    utillib_char_scanner_shiftaway(&self->input);
    if (utillib_char_scanner_lookahead(&self->input) == '=') {
      utillib_char_scanner_shiftaway(&self->input);
      return SYM_NE;
    }
    return -CL_EBADNEQ;
  }

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
    return read_number(self);
  return -CL_EUNKNOWN;
}

void cling_scanner_init(struct cling_scanner *self,
                        struct utillib_vector *elist, FILE *file) {
  /*
   * context will be set by each parse function.
   */
  self->elist = elist;
  utillib_char_scanner_init(&self->input, file);
  utillib_string_init(&self->buffer);
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

void cling_scanner_shiftaway(struct cling_scanner *self) {}

inline char const *cling_scanner_semantic(struct cling_scanner const *self) {
  return utillib_string_c_str(&self->buffer);
}
