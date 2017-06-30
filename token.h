/* token.h */
#ifndef TOKEN_H
#define TOKEN_H 1
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "char_buffer.h"
#include "token_type.h"
#include "tknzr_error.h"
#define TYPE_DESCRIPTOR_MAX_LEN 6
#define TOKEN_TYPE(t)    (((token*) t)->type)
#define TOKEN_STRING(t) (((token*) t)->string != NULL ? (t)->string : "")
#define TOKEN_VARLEN_INIT_LEN 50

typedef struct token 
{
  // index into global type description
  // array
  int type_id;
  token_type type;
  position begin;
  char *string;
  int len;
  int max;

} token;


#endif

