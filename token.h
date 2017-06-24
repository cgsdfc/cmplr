/* token.h */
#ifndef TOKEN_H
#define TOKEN_H 1
#include "char_buffer.h"
#include "state.h"
#include "token_type.h"

#define TOKEN_LEN_TYPE(t) ((t)->_type) 
#define TOKEN_TYPE(t)    (((token*) t)->type)
#define TOKEN_FIXLEN_STRING(t) (((fixlen_token*) t)->string)
#define TOKEN_VARLEN_STRING(t) (((varlen_token*) t)->string)
#define TOKEN_FIXLEN_MAX_LEN 20
#define TOKEN_VARLEN_INIT_LEN 50


typedef struct token 
{
  enum token_type type;
  position begin;
  unsigned char _type:2;
} token;

typedef struct breif_token
{
  token _token;
  char ch;
} breif_token;

typedef struct varlen_token
{
  token _token;
  char *string;
  int len;
  int max;
} varlen_token;

typedef struct fixlen_token
{
  token _token;
  char string[TOKEN_FIXLEN_MAX_LEN];
  int len;
} fixlen_token;

/* init */
token *init_breif(position *begin, char ch);
token *init_fixlen(position *begin, char ch);
token *init_varlen(position *begin, char ch);

/* append */
int append_brief(token *tk, char ch);
int append_fixlen(token *tk, char ch);
int append_varlen(token *tk, char ch);

/* accept */
int accept_brief(token *tk, char ch,node state, bool append);
int accept_varlen(token *tk,char ch,node state, bool append);
int accept_fixlen(token *tk,char ch,node state,bool append);
char *format_token (token *tk);




#endif

