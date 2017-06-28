#ifndef TKNZR_ERROR_H 
#define TKNZR_ERROR_H 1

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
typedef enum tknzr_error
{
  TERR_SUCCESS=0,
  _TERR_FATAL,
  TERR_NO_INPUT_FILE=1,
  TERR_NOMEM,
  _TERR_ERROR,
  TERR_COMMENT_UNTERMINATED,
  TERR_DIGIT_NOT_OCTONARY,
  TERR_DIGIT_NOT_HEXADECIMAL,
  TERR_BAD_INTEGER_SUFFIX,
  TERR_INCOMPLETED_FLOAT,
  TERR_BAD_FLOAT_SUFFIX,
  TERR_STRING_UNTERMINATED,
  TERR_FLOAT_EXP_NO_DIGIT,
  TERR_CHAR_UNTERMINATED,
  TERR_EMPTY_CHAR_LITERAL,
  TERR_INVALID_IDENTIFIER,
  _TERR_WARNING,
  TERR_END_OF_INPUT,
  TERR_UNKNOWN,
  _TERR_NULL,
} tknzr_error;

tknzr_error  tknzr_error_get(void);
void tknzr_error_set(tknzr_error err);
void tknzr_error_clear(void);
void tknzr_error_handle(void);

#endif

