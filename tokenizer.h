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
#include "tokenizer.h"

#include "char_buffer.h"
#include "token.h"

#define MAX_TRANSFER_ENTRIES 100

#define CHAR_CLASS_LOWER_CASE "abcdefghijklmnopqrstuvwxyz"
#define CHAR_CLASS_UPPER_CASE "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define CHAR_CLASS_TEN_DIGITS "0123456789" 
#define CHAR_CLASS_IDENTIFIER_BEGIN "_" CHAR_CLASS_LOWER_CASE CHAR_CLASS_UPPER_CASE
#define CHAR_CLASS_IDENTIFIER_PART CHAR_CLASS_IDENTIFIER_BEGIN CHAR_CLASS_TEN_DIGITS
#define CHAR_CLASS_SPACES " \t\n\r\f\v"
#define CHAR_CLASS_PUNCTUATION "(){}[]:;,.?"
#define CHAR_CLASS_OPERATOR "~!%^&*-+<>=/|"
#define CHAR_CLASS_BACKSLASH "\\"

#define CHAR_CLASS_SEPARATOR \
  CHAR_CLASS_SPACES CHAR_CLASS_PUNCTUATION CHAR_CLASS_OPERATOR

#define CHAR_CLASS_ONE_CHAR \
  CHAR_CLASS_PUNCTUATION 



typedef enum tkz_error
{
  E_UNEXPECTED_CHAR=1,
  E_PREMATURE_END,
  E_NO_MORE_PUT_CHAR,

} tkz_error ;

void init_table (void);
void check_table (void);
void tokenizer_error (int error, char_buffer *buffer, token *tk, tokenizer_state last_state);
int get_next_token (token *tk, char_buffer *buffer, tokenizer_state *errstate);

#endif

