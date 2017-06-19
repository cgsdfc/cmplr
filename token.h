/* token.h */
#ifndef TOKEN_H
#define TOKEN_H 1
#include "char_buffer.h"
#include "state.h"

#define MAX_TOKEN_LEN 50
#define N_KEYWORDS ( sizeof keywords_tab / sizeof keywords_tab[0] )

extern const char *keywords_tab[];
extern const char *token_tab[];


typedef enum token_type 
{
  /* keywords must come first */
  TKT_KW_FOR=0,
  TKT_KW_WHILE,
  TKT_KW_CASE,
  TKT_KW_RETURN,
  TKT_KW_SWITCH,
  TKT_KW_IF,
  TKT_KW_ELSE,
  TKT_KW_GOTO,


  TKT_IDENTIFIER,
  TKT_UNKNOWN

} token_type;

typedef struct token 
{
  enum token_type type;
  union 
  {
    char string[MAX_TOKEN_LEN];
    int integer;
    double dreal;
    float freal;
  } value ;
  int len;
  position begin;
  position end;

} token;


void init_token(struct token*,int,char_buffer*);
void append_token(struct token*,int,char_buffer*);
void skip_token(struct token*,int,char_buffer*);
void accept_token(struct token*,int,char_buffer*);
char *format_token(struct token*);

#endif

