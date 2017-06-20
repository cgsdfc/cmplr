#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "state.h"
#include "char_buffer.h"

const char *keywords_tab [] = 
{
  [TKT_KW_FOR]="for",
  [TKT_KW_WHILE]="while",
  [TKT_KW_IF]="if",
  [TKT_KW_ELSE]="else", 
  [TKT_KW_SWITCH]="switch",
  [TKT_KW_CASE]="case",
  [TKT_KW_GOTO]="goto",
  [TKT_KW_RETURN]="return",


  [TKT_KW_AUTO]="auto",
  [TKT_KW_DOUBLE]="double",
  [TKT_KW_STRUCT]="struct",
  [TKT_KW_BREAK]="break",
  [TKT_KW_LONG]="long",
  [TKT_KW_ENUM]="enum",
  [TKT_KW_REGISTER]="register",
  [TKT_KW_TYPEDEF]="typedef",
  [TKT_KW_CHAR]="char",
  [TKT_KW_EXTERN]="extern",
  [TKT_KW_UNION ]="union",
  [TKT_KW_CONST ]="const",
  [TKT_KW_FLOAT ]="float",
  [TKT_KW_SHORT ]="short",
  [TKT_KW_UNSIGNED ]="unsigned",
  [TKT_KW_CONTINUE ]="continue",
  [TKT_KW_SIGNED ]="signed",
  [TKT_KW_VOID ]="void",
  [TKT_KW_DEFAULT ]="default",
  [TKT_KW_SIZEOF ]="sizeof",
  [TKT_KW_VOLATILE ]="volatile",
  [TKT_KW_DO ]="do",
  [TKT_KW_STATIC ]="static",
  [TKT_KW_INT]="int",


  [TKT_UNKNOWN]="TKT_UNKNOWN"

};

const char *token_tab [] = 
{
  /* KEYWORDS */
  [TKT_KW_IF]="TKT_KW_IF",
  [TKT_KW_FOR]="TKT_KW_FOR",
  [TKT_KW_ELSE]="TKT_KW_ELSE",
  [TKT_KW_GOTO]="TKT_KW_GOTO",
  [TKT_KW_CASE]="TKT_KW_CASE",
  [TKT_KW_SWITCH]="TKT_KW_SWITCH",
  [TKT_KW_RETURN]="TKT_KW_RETURN",
  [TKT_IDENTIFIER]="TKT_IDENTIFIER",
  [TKT_KW_WHILE]="TKT_KW_WHILE",
  [TKT_KW_AUTO]="TKT_KW_AUTO",
  [TKT_KW_DOUBLE]="TKT_KW_DOUBLE",
  [TKT_KW_STRUCT]="TKT_KW_STRUCT",
  [TKT_KW_BREAK]="TKT_KW_BREAK",
  [TKT_KW_LONG]="TKT_KW_LONG",
  [TKT_KW_ENUM]="TKT_KW_ENUM",
  [TKT_KW_REGISTER]="TKT_KW_REGISTER",
  [TKT_KW_TYPEDEF]="TKT_KW_TYPEDEF",
  [TKT_KW_CHAR]="TKT_KW_CHAR",
  [TKT_KW_EXTERN]="TKT_KW_EXTERN",
  [TKT_KW_UNION]="TKT_KW_UNION",
  [TKT_KW_CONST]="TKT_KW_CONST",
  [TKT_KW_FLOAT]="TKT_KW_FLOAT",
  [TKT_KW_SHORT]="TKT_KW_SHORT",
  [TKT_KW_UNSIGNED]="TKT_KW_UNSIGNED",
  [TKT_KW_CONTINUE]="TKT_KW_CONTINUE",
  [TKT_KW_SIGNED]="TKT_KW_SIGNED",
  [TKT_KW_VOID]="TKT_KW_VOID",
  [TKT_KW_DEFAULT]="TKT_KW_DEFAULT",
  [TKT_KW_SIZEOF]="TKT_KW_SIZEOF",
  [TKT_KW_VOLATILE]="TKT_KW_VOLATILE",
  [TKT_KW_DO]="TKT_KW_DO",
  [TKT_KW_STATIC]="TKT_KW_STATIC",
  [TKT_KW_INT]="TKT_KW_INT",

  /* PUNCTUATION */
  [TKT_LEFT_PARENTHESIS]="TKT_LEFT_PARENTHESIS",
  [TKT_RIGHT_PARENTHESIS]="TKT_RIGHT_PARENTHESIS",
  [TKT_LEFT_BRACKET]="TKT_LEFT_BRACKET",
  [TKT_RIGHT_BRACKET]="TKT_RIGHT_BRACKET",
  [TKT_LEFT_BRACE]="TKT_LEFT_BRACE",
  [TKT_RIGHT_BRACE]="TKT_RIGHT_BRACE",
  [TKT_COLON]="TKT_COLON",
  [TKT_SEMICOLON]="TKT_SEMICOLON",
  [TKT_COMMA]="TKT_COMMA",
  [TKT_PERIOD]="TKT_PERIOD",
  [TKT_QUESTION]="TKT_QUESTION",

  /* OPERATORS */
  [TKT_TILDE]="TKT_TILDE",
  [TKT_EXCLAIM]="TKT_EXCLAIM",
  [TKT_PERCENT]="TKT_PERCENT",
  [TKT_CARET]="TKT_CARET",
  [TKT_AMPERSAND]="TKT_AMPERSAND",
  [TKT_STAR]="TKT_STAR",
  [TKT_POSITIVE]="TKT_POSITIVE",
  [TKT_NEGATIVE]="TKT_NEGATIVE",
  [TKT_LESS]="TKT_LESS",
  [TKT_GREATER]="TKT_GREATER",
  [TKT_EQUAL]="TKT_EQUAL",
  [TKT_SLASH]="TKT_SLASH",
  [TKT_VERTICAL_BAR]="TKT_VERTICAL_BAR",

  /* ITERALS */
  [TKT_INTEGER_ITERAL]="TKT_INTEGER_ITERAL",
  [TKT_UNKNOWN]="TKT_UNKNOWN"

};

const static token_type char2type [] =
{
  
  /* PUNCTUATIONS */
  ['(']=TKT_LEFT_PARENTHESIS,
  [')']=TKT_RIGHT_PARENTHESIS,
  ['[']=TKT_LEFT_BRACKET,
  [']']=TKT_RIGHT_BRACKET,
  ['{']=TKT_LEFT_BRACE,
  ['}']=TKT_RIGHT_BRACE,
  [':']=TKT_COLON,
  [';']=TKT_SEMICOLON,
  [',']=TKT_COMMA,
  ['.']=TKT_PERIOD,
  ['?']=TKT_QUESTION,

  /* OPERATORS */
  ['~']=TKT_TILDE,
  ['!']=TKT_EXCLAIM,
  ['%']=TKT_PERCENT,
  ['^']=TKT_CARET,
  ['&']=TKT_AMPERSAND,
  ['*']=TKT_STAR,
  ['+']=TKT_POSITIVE,
  ['-']=TKT_NEGATIVE,
  ['<']=TKT_LESS,
  ['>']=TKT_GREATER,
  ['=']=TKT_EQUAL,
  ['/']=TKT_SLASH,
  ['|']=TKT_VERTICAL_BAR


};

typedef enum pos_enum
{
  POS_BEGIN, POS_END
} pos_enum;

void _set_token_pos (token *tk, char_buffer *buffer, pos_enum begin)
{
  switch (begin) {
    case POS_BEGIN:
      memcpy (&tk->begin, &buffer->pos, sizeof (position));
      return;
    case POS_END:
      memcpy (&tk->end, &buffer->pos, sizeof (position));
      return;
    default:
      return;
  }
      
}

void _terminate_token (token *tk)
{
  tk->value.string [tk->len]=0;
}

void _catchar_token (token *tk, char ch)
{
  int len = tk->len;
  tk->value.string [len] = ch;
  tk->len=len+1;

}

void _clear_token (token *tk)
{
  memset (tk, 0, sizeof *tk);

}

void init_token(token *tk, int state, char_buffer *buffer)
{
  _clear_token(tk);
  _set_token_pos(tk, buffer, POS_BEGIN);
  _catchar_token(tk, prev_char(buffer));

  switch (state) {
    case TK_INT_DEC_BEGIN:
      break;
    case TK_IDENTIFIER_BEGIN:
      break;
    default:
      break;
  }

}

void append_token(token *tk, int state, char_buffer *buffer)
{
  _catchar_token(tk,prev_char (buffer));
  switch (state) {
    case TK_INT_DEC_BEGIN:
      break;
    case TK_IDENTIFIER_BEGIN:
      break;
    default:
      break;
  }

}

void skip_token(token *tk, int state, char_buffer *buffer)
{
  /* do nothing */
}

void acc_identifier(token *tk)
{

  for (int i=0;i<N_KEYWORDS;++i)
  {
    if (keywords_tab[i] && 0==(strcmp(keywords_tab [i], tk->value.string)))
    {
      tk->type = i;
      return;
    }
  }
  tk->type = TKT_IDENTIFIER;

}
  
/** note: peek_char(buffer) will be one char pass the 
 * char that caused the *ACCEPT*
 * so use prev_char (buffer) to get the char
 */
void accept_token(token *tk, int state, char_buffer *buffer)
{

  void acc_integer(token *tk);
  void acc_one_char(token *tk, char);
  switch (state) {
    case TK_IDENTIFIER_END:
      _terminate_token(tk);
      acc_identifier(tk);
      put_char(buffer);
      _set_token_pos(tk,buffer,POS_END);
      return;

    case TK_ONE_CHAR_END:
      _set_token_pos(tk,buffer,POS_BEGIN);
      _set_token_pos(tk,buffer,POS_END);
      acc_one_char (tk, prev_char (buffer));
      return ;

    case TK_INT_END:
      _terminate_token(tk);
      acc_integer(tk);
      put_char(buffer);
      return;

    default:
      break;
  }

}


void acc_one_char (token *tk, char character)
{
  _catchar_token(tk,character);
  _terminate_token(tk);
  tk->type=char2type[character];
}

void acc_integer (token *tk)
{
  char *value = tk->value.string;
  tk->type = TKT_INTEGER_ITERAL;
  switch (tk->value.property.int_flag) {
    case INT_FLAG_UNSIGNED:
      tk->value.number.uint= (unsigned int) atoi (value);
      return;
    case INT_FLAG_LONG:
      tk->value.number.long_int=atol(value);
      return;
    case ( INT_FLAG_LONG | INT_FLAG_UNSIGNED ):
      tk->value.number.ulong=(unsigned long) atol(value);
      return;
  }

}


char *format_token (token *tk) 
{
  static char buf [ BUFSIZ ];
  snprintf ( buf , BUFSIZ, "<%s> <%s> <%d,%d> <%d,%d>", 
      token_tab [ tk->type ], tk->value.string,
      tk->begin.lineno, tk->begin.column, tk->end.lineno, tk->end.column);

  return buf;

}
