/* tokenizer.h */
#ifndef TOKENIZER_H
#define TOKENIZER_H 1
#include <sys/types.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "token.h"

enum
{
  TKA_COLLECT_CHAR=1,
  TKA_ALLOC_BUF,
  TKA_ACC_IDFR,
  TKA_ACC_PUNC,
  TKA_ACC_OPER,
  TKA_ACC_CHAR,
  TKA_ACC_INT,
  TKA_ACC_FLOAT,
  TKA_ACC_STRING,
  TKA_PUTBACK,
};

int get_next_token(char_buffer *, token *);
int init_tokenizer(void);


#endif

