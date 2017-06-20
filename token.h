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
   /* KEYWORDS */
  TKT_KW_FOR=0,
  TKT_KW_WHILE,
  TKT_KW_CASE,
  TKT_KW_RETURN,
  TKT_KW_SWITCH,
  TKT_KW_IF,
  TKT_KW_ELSE,
  TKT_KW_GOTO,

  TKT_KW_AUTO,
  TKT_KW_DOUBLE,
  TKT_KW_STRUCT,
  TKT_KW_BREAK,
  TKT_KW_LONG,
  TKT_KW_ENUM,
  TKT_KW_REGISTER,
  TKT_KW_TYPEDEF,
  TKT_KW_CHAR,
  TKT_KW_EXTERN,
  TKT_KW_UNION ,
  TKT_KW_CONST ,
  TKT_KW_FLOAT ,
  TKT_KW_SHORT ,
  TKT_KW_UNSIGNED ,
  TKT_KW_CONTINUE ,
  TKT_KW_SIGNED ,
  TKT_KW_VOID ,
  TKT_KW_DEFAULT ,
  TKT_KW_SIZEOF ,
  TKT_KW_VOLATILE ,
  TKT_KW_DO ,
  TKT_KW_STATIC ,
  TKT_KW_INT,

  /* PUNCTUATIONS */
  TKT_LEFT_PARENTHESIS,
  TKT_RIGHT_PARENTHESIS,
  TKT_LEFT_BRACKET,
  TKT_RIGHT_BRACKET,
  TKT_LEFT_BRACE,
  TKT_RIGHT_BRACE,
  TKT_COLON,
  TKT_SEMICOLON,
  TKT_COMMA,
  TKT_PERIOD,
  TKT_QUESTION,

  /* OPERATORS */
  TKT_TILDE,
  TKT_EXCLAIM,
  TKT_PERCENT,
  TKT_CARET,
  TKT_AMPERSAND,
  TKT_STAR,
  TKT_POSITIVE,
  TKT_NEGATIVE,
  TKT_LESS,
  TKT_GREATER,
  TKT_EQUAL,
  TKT_SLASH,
  TKT_VERTICAL_BAR,

  /* IDENTIFIERS */
  TKT_IDENTIFIER,

  /* ITERALS */
  TKT_INTEGER_ITERAL,
  TKT_UNKNOWN


} token_type;

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


void init_token(struct token*,int,char_buffer*);
void append_token(struct token*,int,char_buffer*);
void skip_token(struct token*,int,char_buffer*);
void accept_token(struct token*,int,char_buffer*);
char *format_token(struct token*);

#endif

