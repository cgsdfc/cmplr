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
#include "tknz_table.h"
#include "token.h"

// TODO: clear TODOs, 
// impl token_buffer that can look
// ahead token, consum and discard
// tokens
typedef struct tokenizer
{
  char_buffer buffer;
  entry_t *table;
  int *counter;
  int len;

} tokenizer ;


typedef enum tkz_error
{
  E_UNEXPECTED_CHAR=1,
  E_PREMATURE_END,
  E_NO_MORE_PUT_CHAR,

} tkz_error ;

void tokenizer_error (int error, char_buffer *buffer, token *tk, tokenizer_state last_state);
int get_next_token (token *tk, char_buffer *buffer, tokenizer_state *errstate);

#endif

