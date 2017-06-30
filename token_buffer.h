#ifndef TOKEN_BUFFER_H
#define TOKEN_BUFFER_H 1
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "char_buffer.h"
#include "block_buffer.h"
#include "token.h"

#define N_TKB_BLKSZ 100
#define N_TKB_INIT_NBLK 10

typedef struct token_buffer
{
  char_buffer *cb;
  block_buffer *bb;
  block_pos curtk;

} token_buffer;

int init_token_buffer(token_buffer *buf,char_buffer *cb);
int next_token(token_buffer *buf,token **tk);
int prev_token(token_buffer *buf,token **tk);
int put_token(token_buffer *buf);
int get_token(token_buffer *buf,token **tk);
#endif

