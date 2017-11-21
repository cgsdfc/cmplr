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
#include "json_parser_impl.h"
#include "json_parser.h"
#include "scanner.h"
#include <assert.h>
#include <ctype.h>
#include <string.h>

UTILLIB_SYMBOL_BEGIN(utillib_json_symbols)
UTILLIB_SYMBOL_NON_TERMINAL(JSON_SYM_VAL)
UTILLIB_SYMBOL_NON_TERMINAL(JSON_SYM_OBJ)
UTILLIB_SYMBOL_NON_TERMINAL(JSON_SYM_ARR)
UTILLIB_SYMBOL_NON_TERMINAL(JSON_SYM_STRVAL)
UTILLIB_SYMBOL_NON_TERMINAL(JSON_SYM_STRVAL_LS)
UTILLIB_SYMBOL_NON_TERMINAL(JSON_SYM_VAL_)
UTILLIB_SYMBOL_NON_TERMINAL(JSON_SYM_VAL_LS)

UTILLIB_SYMBOL_TERMINAL(JSON_SYM_STR)
UTILLIB_SYMBOL_TERMINAL(JSON_SYM_NUM)
UTILLIB_SYMBOL_TERMINAL(JSON_SYM_TRUE)
UTILLIB_SYMBOL_TERMINAL(JSON_SYM_FALSE)
UTILLIB_SYMBOL_TERMINAL(JSON_SYM_NULL)
UTILLIB_SYMBOL_TERMINAL(JSON_SYM_LB)
UTILLIB_SYMBOL_TERMINAL(JSON_SYM_RB)
UTILLIB_SYMBOL_TERMINAL(JSON_SYM_LK)
UTILLIB_SYMBOL_TERMINAL(JSON_SYM_RK)
UTILLIB_SYMBOL_TERMINAL(JSON_SYM_COMMA)
UTILLIB_SYMBOL_TERMINAL(JSON_SYM_COLON)
UTILLIB_SYMBOL_END(utillib_json_symbols);

/*
 * Rules for JSON:
 * value := object | array | number | true | false | null | string
 * object := '{' [strval] {, strval } '}'
 * array := '[' [value], {, value } ']'
 * strval := string ':' value
 *
 * Reference:
 * https://www.json.org/json-zh.html
 *
 * Notes that `JSON_SYM_VAL_' := epsilon
 * and `JSON_SYM_STRVAL' := epsilon.
 * That is object and array can be empty.
 */

UTILLIB_RULE_BEGIN(utillib_json_rules)
UTILLIB_RULE_ELEM(JSON_SYM_VAL, JSON_SYM_STR)
UTILLIB_RULE_ELEM(JSON_SYM_VAL, JSON_SYM_NUM)
UTILLIB_RULE_ELEM(JSON_SYM_VAL, JSON_SYM_OBJ)
UTILLIB_RULE_ELEM(JSON_SYM_VAL, JSON_SYM_ARR)
UTILLIB_RULE_ELEM(JSON_SYM_VAL, JSON_SYM_TRUE)
UTILLIB_RULE_ELEM(JSON_SYM_VAL, JSON_SYM_FALSE)
UTILLIB_RULE_ELEM(JSON_SYM_VAL, JSON_SYM_NULL)
UTILLIB_RULE_ELEM(JSON_SYM_OBJ, JSON_SYM_LB, JSON_SYM_STRVAL,
                  JSON_SYM_STRVAL_LS, JSON_SYM_RB)
UTILLIB_RULE_ELEM(JSON_SYM_ARR, JSON_SYM_LK, JSON_SYM_VAL_, JSON_SYM_VAL_LS,
                  JSON_SYM_RK)
UTILLIB_RULE_ELEM(JSON_SYM_VAL_, JSON_SYM_VAL)
UTILLIB_RULE_ELEM(JSON_SYM_VAL_, UT_SYM_EPS)
UTILLIB_RULE_ELEM(JSON_SYM_VAL_LS, JSON_SYM_COMMA, JSON_SYM_VAL,
                  JSON_SYM_VAL_LS)
UTILLIB_RULE_ELEM(JSON_SYM_STRVAL, JSON_SYM_STR, JSON_SYM_COLON, JSON_SYM_VAL)
UTILLIB_RULE_ELEM(JSON_SYM_STRVAL, UT_SYM_EPS)
UTILLIB_RULE_ELEM(JSON_SYM_STRVAL_LS, JSON_SYM_COMMA, JSON_SYM_STRVAL,
                  JSON_SYM_STRVAL_LS)
UTILLIB_RULE_ELEM(JSON_SYM_VAL_LS, UT_SYM_EPS)
UTILLIB_RULE_ELEM(JSON_SYM_STRVAL_LS, UT_SYM_EPS)
UTILLIB_RULE_END(utillib_json_rules);

const struct utillib_scanner_op utillib_json_scanner_op = {
    .lookahead = utillib_json_scanner_lookahead,
    .shiftaway = utillib_json_scanner_shiftaway,
    .semantic = utillib_json_scanner_semantic,
};

static int json_scanner_read_str(struct utillib_string_scanner *scanner,
                                 struct utillib_string *buffer) {
  char ch;
  for (ch=utillib_string_scanner_lookahead(scanner);
      ch != '\"'; utillib_string_scanner_shiftaway(scanner)) {
    if (ch == '\n' || ch == '\r' || '\0')
      return -JSON_ESTRING;
    if (ch != '\\') {
      utillib_string_append_char(buffer, ch);
      continue;
    }
    utillib_string_scanner_shiftaway(scanner);
    ch=utillib_string_scanner_lookahead(scanner);
    switch (ch) {
      case '\"':
        utillib_string_append_char(buffer, '\"');
        break;
      case '\\':
        utillib_string_append_char(buffer, '\\');
        break;
      case '/':
        utillib_string_append_char(buffer, '/');
        break;
      case 'b':
        utillib_string_append_char(buffer, '\b');
        break;
      case 'f':
        utillib_string_append_char(buffer, '\f');
        break;
      case 'n':
        utillib_string_append_char(buffer, '\n');
        break;
      case 'r':
        utillib_string_append_char(buffer, '\r');
        break;
      case 't':
        utillib_string_append_char(buffer, '\t');
        break;
      default:
        return -JSON_EESCAPE;
      }
  }
  return JSON_SYM_STR;
}

static void json_scanner_collect_char(
    struct utillib_string_scanner *scanner, 
    struct utillib_string *buffer)
{
  utillib_string_append_char(buffer,
      utillib_string_scanner_lookahead(scanner));
  utillib_string_scanner_shiftaway(scanner);
}

static void json_scanner_collect_digit(
    struct utillib_string_scanner *scanner, 
    struct utillib_string *buffer)
{
  int ch;
  while (isdigit(ch=utillib_string_scanner_lookahead(scanner))) {
    utillib_string_append_char(buffer, ch);
    utillib_string_scanner_shiftaway(scanner);
  }
}

/**
 * \function json_scanner_read_number
 *
 * number:
 *  int | int frac | int exp | int frac exp
 * int:
 *  digit | digit1-9 digits | - digit | - digit1-9 digits
 * frac:
 *  . digits
 * exp:
 *  e digits
 * digits:
 *  digit | digit digits
 * e: e | e+ | e- | E | E+ | E- |
 * 
 */

static int json_scanner_read_number(struct utillib_string_scanner *scanner, 
    struct utillib_string *buffer)
{
  char ch=utillib_string_scanner_lookahead(scanner);
  /* Handles the optional sign */
  if (ch == '-' || ch == '+') {
    json_scanner_collect_char(scanner, buffer);
    ch=utillib_string_scanner_lookahead(scanner);
    if (!isdigit(ch))
      return -JSON_ENODIGIT;
  }

  json_scanner_collect_digit(scanner, buffer);
  /* Handles the optional fraction or exponent */
  ch=utillib_string_scanner_lookahead(scanner);
  if (ch != '.' || ch != 'e' || ch != 'E')
    return JSON_SYM_NUM;
  bool seen_exp=false;
  bool seen_frac=false;
  while (true) {
    if (seen_exp)
      return JSON_SYM_NUM;
    if (seen_frac)
      return JSON_SYM_NUM;
    if (ch == '.') {
      json_scanner_collect_char(scanner, buffer);
      ch=utillib_string_scanner_lookahead(scanner);
      if (!isdigit(ch))
        return -JSON_EFRACTION;
      json_scanner_collect_digit(scanner, buffer);
      seen_frac=true;
    } else if (ch == 'e' || ch == 'E') {
      json_scanner_collect_char(scanner, buffer);
      ch=utillib_string_scanner_lookahead(scanner);
      if (isdigit(ch)) {
        json_scanner_collect_digit(scanner, buffer);
        seen_exp=true;
        continue;
      }
      /* Again, handle the optional sign */
      if (ch == '-' || ch == '+') {
        json_scanner_collect_char(scanner, buffer);
        ch=utillib_string_scanner_lookahead(scanner);
        if (!isdigit(ch)) 
          return -JSON_ENODIGIT;
        json_scanner_collect_digit(scanner, buffer);
        seen_exp=true;
      }
      return -JSON_EEXPONENT;
    }
  }
}

static int 
json_scanner_read(struct utillib_string_scanner *scanner, struct utillib_string *buffer) {
  while (isspace(utillib_string_scanner_lookahead(scanner)))
    utillib_string_scanner_shiftaway(scanner);
  if (utillib_string_scanner_reacheof(scanner))
    return UT_SYM_EOF;

  char const *keyword;
  size_t ch=utillib_string_scanner_lookahead(scanner);
  switch (ch) {
    case '[':
      utillib_string_scanner_shiftaway(scanner); 
      return JSON_SYM_LK;
    case ']':
      utillib_string_scanner_shiftaway(scanner); 
      return JSON_SYM_RK;
    case '{':
      utillib_string_scanner_shiftaway(scanner); 
      return JSON_SYM_LB;
    case '}':
      utillib_string_scanner_shiftaway(scanner); 
      return JSON_SYM_RB;
    case ',':
      utillib_string_scanner_shiftaway(scanner); 
      return JSON_SYM_COMMA;
    case ':':
      utillib_string_scanner_shiftaway(scanner); 
      return JSON_SYM_COLON;
    case 't':
    case 'f':
    case 'n':
      utillib_string_append_char(buffer,ch);
      while (isalpha(utillib_string_scanner_lookahead(scanner))) {
        ch=utillib_string_scanner_lookahead(scanner);
        utillib_string_append_char(buffer, ch);
      }
      keyword = utillib_string_c_str(buffer);
      if (strcmp(keyword, "true") == 0)
        return JSON_SYM_TRUE;
      if (strcmp(keyword, "false") == 0)
        return JSON_SYM_FALSE;
      if (strcmp(keyword, "null") == 0)
        return JSON_SYM_NULL;
      return -JSON_EUNKNOWN;
    case '\"':
      return json_scanner_read_str(scanner, buffer);
    default:
      break;
  }
  if (ch == '-' ||ch  == '+' || isdigit(ch)) {
    return json_scanner_read_number(scanner, buffer);
  }
  return -JSON_EUNKNOWN;
}

void utillib_json_scanner_init(struct utillib_json_scanner *self,
    char const *str) {
  utillib_string_init(&self->buffer);
  utillib_string_scanner_init(&self->scanner, str);
  self->code = 0;
}

size_t utillib_json_scanner_lookahead(struct utillib_json_scanner *self) {
  return self->code;
}

void utillib_json_scanner_shiftaway(struct utillib_json_scanner *self) {
  int code=json_scanner_read(&self->scanner, &self->buffer);
  self->code = code >= 0 ? code : UT_SYM_ERR;
}

void const *utillib_json_scanner_semantic(struct utillib_json_scanner *self) {
  switch (self->code) {
    case UT_SYM_EOF:
    case UT_SYM_ERR:
      return NULL;
    case JSON_SYM_FALSE:
      return &utillib_json_false;
    case JSON_SYM_TRUE:
      return &utillib_json_true;
    case JSON_SYM_NULL:
      return &utillib_json_null;
    case JSON_SYM_STR:
      return strdup(utillib_string_c_str(&self->buffer));
    case JSON_SYM_NUM:
      return strtod(utillib_string_c_str(&self->buffer), NULL);
    default:
      return NULL;
  }
}

void utillib_json_scanner_destroy(struct utillib_json_scanner *self) {
  utillib_string_destroy(&self->buffer);
}
