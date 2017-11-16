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
#include "scanner.h"
#include <ctype.h>

UTILLIB_SYMBOL_BEGIN(utillib_json_symbols)
  UTILLIB_SYMBOL_NON_TERMINAL(UT_JSON_SYM_VAL)
  UTILLIB_SYMBOL_NON_TERMINAL(UT_JSON_SYM_OBJ)
  UTILLIB_SYMBOL_NON_TERMINAL(UT_JSON_SYM_ARR)
  UTILLIB_SYMBOL_NON_TERMINAL(UT_JSON_SYM_STRVAL)
  UTILLIB_SYMBOL_NON_TERMINAL(UT_JSON_SYM_STRVAL_LS)
  UTILLIB_SYMBOL_NON_TERMINAL(UT_JSON_SYM_VAL_)
  UTILLIB_SYMBOL_NON_TERMINAL(UT_JSON_SYM_VAL_LS)

  UTILLIB_SYMBOL_TERMINAL(UT_JSON_SYM_STR)
  UTILLIB_SYMBOL_TERMINAL(UT_JSON_SYM_NUM)
  UTILLIB_SYMBOL_TERMINAL(UT_JSON_SYM_KW_TRUE)
  UTILLIB_SYMBOL_TERMINAL(UT_JSON_SYM_KW_FALSE)
  UTILLIB_SYMBOL_TERMINAL(UT_JSON_SYM_KW_NULL)
  UTILLIB_SYMBOL_TERMINAL(UT_JSON_SYM_LB)
  UTILLIB_SYMBOL_TERMINAL(UT_JSON_SYM_RB)
  UTILLIB_SYMBOL_TERMINAL(UT_JSON_SYM_LK)
  UTILLIB_SYMBOL_TERMINAL(UT_JSON_SYM_RK)
  UTILLIB_SYMBOL_TERMINAL(UT_JSON_SYM_COMMA)
  UTILLIB_SYMBOL_TERMINAL(UT_JSON_SYM_COLON)
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
 * Notes that `UT_JSON_SYM_VAL_' := epsilon
 * and `UT_JSON_SYM_STRVAL' := epsilon.
 * That is object and array can be empty.
 */

UTILLIB_RULE_BEGIN(utillib_json_rules)
  UTILLIB_RULE_ELEM(UT_JSON_SYM_VAL,UT_JSON_SYM_STR)
  UTILLIB_RULE_ELEM(UT_JSON_SYM_VAL,UT_JSON_SYM_NUM)
  UTILLIB_RULE_ELEM(UT_JSON_SYM_VAL,UT_JSON_SYM_OBJ)
  UTILLIB_RULE_ELEM(UT_JSON_SYM_VAL,UT_JSON_SYM_ARR)
  UTILLIB_RULE_ELEM(UT_JSON_SYM_VAL,UT_JSON_SYM_KW_TRUE)
  UTILLIB_RULE_ELEM(UT_JSON_SYM_VAL, UT_JSON_SYM_KW_FALSE)
  UTILLIB_RULE_ELEM(UT_JSON_SYM_VAL, UT_JSON_SYM_KW_NULL)
  UTILLIB_RULE_ELEM(UT_JSON_SYM_OBJ, UT_JSON_SYM_LB, UT_JSON_SYM_STRVAL,UT_JSON_SYM_STRVAL_LS , UT_JSON_SYM_RB)
  UTILLIB_RULE_ELEM(UT_JSON_SYM_ARR, UT_JSON_SYM_LK, UT_JSON_SYM_VAL_, UT_JSON_SYM_VAL_LS, UT_JSON_SYM_RK)
  UTILLIB_RULE_ELEM(UT_JSON_SYM_VAL_, UT_JSON_SYM_VAL)
  UTILLIB_RULE_ELEM(UT_JSON_SYM_VAL_, UT_SYM_EPS)
  UTILLIB_RULE_ELEM(UT_JSON_SYM_VAL_LS, UT_JSON_SYM_COMMA, UT_JSON_SYM_VAL, UT_JSON_SYM_VAL_LS)
  UTILLIB_RULE_ELEM(UT_JSON_SYM_STRVAL, UT_JSON_SYM_STR, UT_JSON_SYM_COLON, UT_JSON_SYM_VAL)
  UTILLIB_RULE_ELEM(UT_JSON_SYM_STRVAL, UT_SYM_EPS)
  UTILLIB_RULE_ELEM(UT_JSON_SYM_STRVAL_LS, UT_JSON_SYM_COMMA, UT_JSON_SYM_STRVAL, UT_JSON_SYM_STRVAL_LS)
  UTILLIB_RULE_ELEM(UT_JSON_SYM_VAL_LS, UT_SYM_EPS)
  UTILLIB_RULE_ELEM(UT_JSON_SYM_STRVAL_LS, UT_SYM_EPS)
UTILLIB_RULE_END(utillib_json_rules);

const struct utillib_scanner_op utillib_json_scanner_op={
  .lookahead=utillib_json_scanner_lookahead,
  .shiftaway=utillib_json_scanner_shiftaway,
  .semantic=utillib_json_scanner_semantic,
};

static int json_scanner_read_str(char const ** input,  struct utillib_string * buffer)
{
  char specail;
  char const * str=*input;
  assert (*str == '\"' && "Should be called when a `\"' was seen");
  for (; *str != '\"'; ++str) {
    if (*str == '\0' || *str == '\n' || *str == '\r') {
      *input=str;
      return -UT_JSON_ESTRING;
    }

    if (*str == '\\') {
      ++str;
      switch (*str) {
        case '\"': ++str; specail='\"'; break;
        case '\\': ++str; specail='\\'; break;
        case '/': ++str; specail='/'; break;
        case 'b': ++str; specail='\b'; break;
        case 'f': ++str; specail='\f'; break;
        case 'n': ++str; specail='\n'; break;
        case 'r': ++str; specail='\r'; break;
        case 't': ++str; specail='\t'; break;
        default: *input=str; return -UT_JSON_EESCAPE;
      }
      utillib_string_append_char(buffer, specail);
      continue;
    } 
    utillib_string_append_char(buffer, *str);
  }
  ++str;
  *input=str;
  return UT_JSON_SYM_STR;
}

/**
 * \function json_scanner_read_number
 *
number
    int
    int frac
    int exp
    int frac exp 
int
    digit
    digit1-9 digits
    - digit
    - digit1-9 digits 
frac
    . digits
exp
    e digits
digits
    digit
    digit digits
e
    e
    e+
    e-
    E
    E+
    E-
*/

static int json_scanner_read_number(char const **input, struct utillib_string *buffer)
{
  if (**input == '.') {
    utillib_string_append(buffer, "0.");
    ++*input;
    goto fraction;
  }
  if (**input == '+' || **input == '-') {
    utillib_string_append(
fraction:




}

static int json_scanner_read(char const ** str, struct utillib_string * buffer)
{
  char const * keyword;
  while (isspace(**str))
    ++*str;
  if (**str == '\0') 
    return UT_SYM_EOF;
  switch (**str) {
  case '[': ++*str; return UT_JSON_SYM_LK;
  case ']': ++*str; return UT_JSON_SYM_RK;
  case '{': ++*str; return UT_JSON_SYM_LB;
  case '}': ++*str; return UT_JSON_SYM_RB;
  case ',': ++*str; return UT_JSON_SYM_COMMA;
  case ':': ++*str; return UT_JSON_SYM_COLON;
  case 't': 
  case 'f': 
  case 'n':
            utillib_string_append_char(buffer, **str);
            while(isalpha(**str)) {
              utillib_string_append_char(buffer, **str);
              ++*str;
            }
            keyword=utillib_string_c_str(buffer);
            if (strcmp(keyword, "true")==0)
              return UT_JSON_SYM_KW_TRUE;
            if (strcmp(keyword, "false")==0)
              return UT_JSON_SYM_KW_FALSE;
            if (strcmp(keyword, "null")==0)
              return UT_JSON_SYM_KW_NULL;
            return -UT_JSON_EUNKNOWN;
  default: break;
  }
  if (**str == '\"') {
    return json_scanner_read_str(str, buffer);
  }
  if (**str == '-' || **str=='+' || **str == '.' || isdigit(**str)) {
    return json_scanner_read_number(str, buffer);
  }
  return -UT_JSON_EUNKNOWN;
}

void utillib_json_scanner_init(struct utillib_json_scanner *self, char const *str)
{
  utillib_string_init(&self->buffer);
  self->str=str;
  self->code=0;
}

size_t utillib_json_scanner_lookahead(struct utillib_json_scanner *self)
{


}

void utillib_json_scanner_shiftaway(struct utillib_json_scanner *self)
{


}

void const * utillib_json_scanner_semantic(struct utillib_json_scanner *self)
{

}

void utillib_json_scanner_destroy(struct utillib_json_scanner *self)
{
  utillib_string_destroy(&self->buffer);
}



