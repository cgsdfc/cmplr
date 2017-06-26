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
  TERR_NOMEM,
  TERR_NOT_BRIEF,
  TERR_NOT_VARLEN,
  TERR_NOT_FIXLEN,
  TERR_TOKEN_TOO_LONG,
  TERR_END_OF_INPUT,
  TERR_UNEXPECTED_CHAR,
  TERR_UNEXPECTED_END,
  _TERR_NULL,
} tknzr_error;

tknzr_error  tknzr_error_get(void);
void tknzr_error_set(tknzr_error err);
void tknzr_error_clear(void);

#endif

