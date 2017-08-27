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
  int type;
  position begin;
  char *string;
  int len;
  int max;

} token;


int collect_char (token * tk, char ch);
int alloc_buffer (token * tk, position * begin);
int accept_char (token * tk, char ch);
int accept_operator (token * tk, char ch);
int accept_identifier (token * tk, char ch);
int accept_float (token * tk, char ch);
int accept_integer (token * tk, char ch);
int accept_punctuation (token * tk, char ch);
int accept_string (token * tk, char ch);
char *format_token (token *);


#endif
