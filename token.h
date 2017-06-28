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
#include "tknzr_state.h"
#include "token_type.h"
#include "operator.h"
#include "punctuation.h"
#include "tknzr_error.h"

#define TOKEN_TYPE(t)    (((token*) t)->type)
#define TOKEN_STRING(t) (((token*) t)->string != NULL ? (t)->string : "")
#define TOKEN_VARLEN_INIT_LEN 50


typedef struct token 
{
  enum token_type type;
  position begin;
  char *string;
  int len;
  int max;

} token;

int accept_token(token *tk, position *, tknzr_state , char);
int accept_varlen(token *tk,char ch,tknzr_state state);
int append_varlen(token *tk, char ch);
int init_varlen(token *tk, position *,char ch);

token *alloc_token(void);

/* helpers */
char *format_token (token *tk);
void fini_token(token *tk);


#endif

