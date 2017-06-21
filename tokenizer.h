/* tokenizer.h */
#ifndef TOKENIZER_H
#define TOKENIZER_H 1
#include <sys/types.h>
#include <stdbool.h>
#include<stdlib.h>
#include <assert.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "transfer.h"
#include "token.h"

typedef enum tkz_error
{
  E_UNEXPECTED_CHAR=1,
  E_PREMATURE_END,
  E_NO_MORE_PUT_CHAR,

} tkz_error ;

void tokenizer_error (int error, char_buffer *buffer, token *tk, tokenizer_state last_state);
int get_next_token (token *tk, char_buffer *buffer, tokenizer_state *errstate);

#endif

