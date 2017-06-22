/* token.h */
#ifndef TOKEN_H
#define TOKEN_H 1
#include "char_buffer.h"
#include "state.h"
#include "token_type.h"

#define MAX_TOKEN_LEN  BUFSIZ
typedef enum integer_flag
{
  INT_FLAG_UNSIGNED=1,
  INT_FLAG_LONG=2
} integer_flag ;

typedef struct token 
{
  enum token_type type;
  union 
  {
    char string[MAX_TOKEN_LEN];
    union
    {
      int integer;
      unsigned int uint;
      long long_int;
      unsigned long ulong;
      double dreal;
      float freal;
    } number;

    union 
    {
      integer_flag int_flag;
    } property;

    char character;
  } value ;

  int len;
  position begin;
  position end;

} token;


void init_token(struct token*,tokenizer_state,char_buffer*);
void append_token(struct token*,tokenizer_state,char_buffer*);
void skip_token(struct token*,tokenizer_state,char_buffer*);
void accept_token(struct token*,tokenizer_state,char_buffer*);
char *format_token(struct token*);

#endif

