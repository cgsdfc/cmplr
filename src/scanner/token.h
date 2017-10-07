#ifndef SCANNER_TOKEN_H
#define SCANNER_TOKEN_H 
#include <stddef.h>
#include "utillib/position.h"

typedef struct scanner_token 
{
  char const * str;
  int type;
  utillib_position pos;
} scanner_token;

scanner_token * scanner_make_token(char *const, int /* type */, utillib_position *);
void scanner_destroy_token(scanner_token * );

#endif

