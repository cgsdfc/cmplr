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
#include "symbols.h"
#include <ctype.h>
#include <utillib/print.h> /* utillib_error_printf */
#define CLING_KW_SIZE 14

UTILLIB_ETAB_BEGIN(cling_scanner_error_kind)
UTILLIB_ETAB_ELEM_INIT(CL_EBADNEQ, "expected `=' after `!' to form `!=' token")
UTILLIB_ETAB_ELEM_INIT(CL_EUNTCHAR, "unterminated character constant")
UTILLIB_ETAB_ELEM_INIT(CL_EUNTSTR, "unterminated string constant")
UTILLIB_ETAB_ELEM_INIT(CL_ESTRCHAR, "unrecogized character in string constant")
UTILLIB_ETAB_ELEM_INIT(CL_ECHRCHAR,
                       "unrecogized character in character constant")
UTILLIB_ETAB_ELEM_INIT(CL_EUNKNOWN, "unrecogized character")
UTILLIB_ETAB_END(cling_scanner_error_kind)

static const struct utillib_keyword_pair cling_keywords[] = {
    {"case", SYM_KW_CASE},     {"char", SYM_KW_CHAR},
    {"const", SYM_KW_CONST},   {"default", SYM_KW_DEFAULT},
    {"else", SYM_KW_ELSE},     {"for", SYM_KW_FOR},
    {"if", SYM_KW_IF},         {"int", SYM_KW_INT},
    {"main", SYM_KW_MAIN},     {"printf", SYM_KW_PRINTF},
    {"return", SYM_KW_RETURN}, {"scanf", SYM_KW_SCANF},
    {"switch", SYW_KW_SWITCH}, {"void", SYM_KW_VOID},
};

static const struct utillib_token_scanner_callback cling_scanner_callback = {
    .read_handler = cling_scanner_read_handler,
    .error_handler = cling_scanner_error_handler,
    .semantic_handler = cling_scanner_semantic_handler,
};

void cling_scanner_init(struct utillib_token_scanner *self, FILE *file) {
  utillib_token_scanner_init(self, file, &cling_scanner_callback);
}

int cling_scanner_read_char(struct utillib_char_scanner *chars,
                            struct utillib_string *buffer) {
  char ch = utillib_char_scanner_lookahead(chars);
  if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '_' ||
      isalnum(ch)) {
    utillib_string_append_char(buffer, ch);
    utillib_char_scanner_shiftaway(chars);
    if (utillib_char_scanner_lookahead(chars) != '\'') {
      return -CL_EUNTCHAR;
    }
    utillib_char_scanner_shiftaway(chars);
    return SYM_CHAR;
  }
  return -CL_ECHRCHAR;
}

int cling_scanner_read_string(struct utillib_char_scanner *chars,
                              struct utillib_string *buffer) {
  char ch;
  for (; (ch = utillib_char_scanner_lookahead(chars)) != '\"';
       utillib_char_scanner_shiftaway(chars)) {
    if (utillib_char_scanner_reacheof(chars))
      return -CL_EUNTSTR;
    if (ch == 32 || ch == 33 || 35 <= ch && ch <= 126) {
      utillib_string_append_char(buffer, ch);
      continue;
    }
    return -CL_ESTRCHAR;
  }
  utillib_char_scanner_shiftaway(chars);
  return SYM_STRING;
}

int cling_scanner_read_handler(struct utillib_char_scanner *chars,
                               struct utillib_string *buffer) {
  utillib_token_scanner_skipspace(chars);
  if (utillib_char_scanner_reacheof(chars))
    return UT_SYM_EOF;
  char ch = utillib_char_scanner_lookahead(chars);
  int code = UT_SYM_NULL;
  switch (ch) {
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
  case ':':
    code = SYM_COLON;
    break;
  default:
    break;
  }
  if (code != UT_SYM_NULL) {
    utillib_char_scanner_shiftaway(chars);
    return code;
  }
  switch (ch) {
  case '=':
    utillib_char_scanner_shiftaway(chars);
    if (utillib_char_scanner_lookahead(chars) == '=') {
      utillib_char_scanner_shiftaway(chars);
      return SYM_DEQ;
    }
    return SYM_EQ;
  case '<':
    utillib_char_scanner_shiftaway(chars);
    if (utillib_char_scanner_lookahead(chars) == '=') {
      utillib_char_scanner_shiftaway(chars);
      return SYM_LE;
    }
    return SYM_LT;
  case '>':
    utillib_char_scanner_shiftaway(chars);
    if (utillib_char_scanner_lookahead(chars) == '=') {
      utillib_char_scanner_shiftaway(chars);
      return SYM_GE;
    }
    return SYM_GT;
  case '!':
    utillib_char_scanner_shiftaway(chars);
    if (utillib_char_scanner_lookahead(chars) == '=') {
      utillib_char_scanner_shiftaway(chars);
      return SYM_NE;
    }
    return -CL_EBADNEQ;
  }

  char const *keyword;
  if (utillib_token_scanner_isidbegin(ch)) {
    utillib_token_scanner_collect_identifier(chars, buffer);
    keyword = utillib_string_c_str(buffer);
    int code = utillib_keyword_bsearch(keyword, cling_keywords, CLING_KW_SIZE);
    if (code != UT_SYM_NULL)
      return code;
    return SYM_IDEN;
  }
  if (ch == '\"') {
    utillib_char_scanner_shiftaway(chars);
    return cling_scanner_read_string(chars, buffer);
  }
  if (ch == '\'') {
    utillib_char_scanner_shiftaway(chars);
    return cling_scanner_read_char(chars, buffer);
  }
  /* Maybe number is over simplified */
  if (isdigit(ch)) {
    utillib_token_scanner_collect_digit(chars, buffer);
    return SYM_UINT;
  }
  return -CL_EUNKNOWN;
}

int cling_scanner_error_handler(
    struct utillib_char_scanner *chars,
    struct utillib_token_scanner_error const *error) {
  char const *errmsg = cling_scanner_error_kind_tostring(error->kind);
  utillib_error_printf("ERROR in line %lu, column %lu: %s\n", chars->row,
                       chars->col, errmsg);
  char victim = error->victim;

  switch (error->kind) {
  case CL_EUNKNOWN:
    utillib_error_printf("character `%c' does not make sense\n", victim);
    break;
  case CL_EBADNEQ:
    utillib_error_printf("but got `%c'\n", victim);
    break;
  case CL_ECHRCHAR:
  case CL_ESTRCHAR:
    utillib_error_printf("character `%c' is not allowed\n", victim);
    break;
  }
  return 1;
}

void const *cling_scanner_semantic_handler(size_t value, char const *str) {
  size_t uint;
  char ch;
  switch (value) {
  case SYM_UINT:
    sscanf(str, "%lu", &uint);
    return uint;
  case SYM_IDEN:
  case SYM_STRING:
    return strdup(str);
  case SYM_CHAR:
    ch = str[0];
    return ch;
  default:
    return NULL;
  }
}

void cling_scanner_destroy(struct utillib_token_scanner *self)
{
  utillib_token_scanner_destroy(self);
}


