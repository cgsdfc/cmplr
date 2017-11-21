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
#include "json_scanner.h"
#include <assert.h>
#include <ctype.h>
#include <string.h>

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
  if (ch != '.' && ch != 'e' && ch != 'E')
    return JSON_SYM_LONG;

  if (ch == '.') {
    json_scanner_collect_char(scanner, buffer);
    ch=utillib_string_scanner_lookahead(scanner);
    if (!isdigit(ch))
      return -JSON_ENODIGIT;
    json_scanner_collect_digit(scanner, buffer);
  } 
  if (ch != 'e' && ch != 'E')
    return JSON_SYM_REAL;

  json_scanner_collect_char(scanner, buffer);
  ch=utillib_string_scanner_lookahead(scanner);
  /* Again, handle the optional sign */
  if (ch == '-' || ch == '+') {
    json_scanner_collect_char(scanner, buffer);
    ch=utillib_string_scanner_lookahead(scanner);
    if (!isdigit(ch)) 
      return -JSON_ENODIGIT;
  }
  json_scanner_collect_digit(scanner, buffer);
  return JSON_SYM_REAL;
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
      while (isalpha(utillib_string_scanner_lookahead(scanner))) {
        json_scanner_collect_char(scanner, buffer);
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
  self->code=json_scanner_read(&self->scanner, &self->buffer);
}

size_t utillib_json_scanner_lookahead(struct utillib_json_scanner *self) {
  return self->code;
}

void utillib_json_scanner_shiftaway(struct utillib_json_scanner *self) {
  utillib_string_clear(&self->buffer);
  int code=json_scanner_read(&self->scanner, &self->buffer);
  self->code = code >= 0 ? code : UT_SYM_ERR;
}

void const *utillib_json_scanner_semantic(struct utillib_json_scanner *self) {
  double real;
  long longv;
  char const *str;
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
      str=strdup(utillib_string_c_str(&self->buffer));
      return utillib_json_string_create(&str);
    case JSON_SYM_LONG:
      longv=strtol(utillib_string_c_str(&self->buffer), NULL);
      return utillib_json_long_create(&longv);
    case JSON_SYM_REAL:
      real=strtod(utillib_string_c_str(&self->buffer), NULL);
      return utillib_json_real_create(&real);
    default:
      return NULL;
  }
}

void utillib_json_scanner_destroy(struct utillib_json_scanner *self) {
  utillib_string_destroy(&self->buffer);
}
