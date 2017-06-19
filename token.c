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
  [TKT_UNKNOWN]="unknown"

};

const char *token_tab [] = 
{
  [TKT_KW_IF]="TKT_KW_IF",
  [TKT_KW_FOR]="TKT_KW_FOR",
  [TKT_KW_ELSE]="TKT_KW_ELSE",
  [TKT_KW_GOTO]="TKT_KW_GOTO",
  [TKT_KW_SWITCH]="TKT_KW_SWITCH",
  [TKT_KW_RETURN]="TKT_KW_RETURN",
  [TKT_IDENTIFIER]="TKT_IDENTIFIER",
  [TKT_KW_WHILE]="TKT_KW_WHILE",

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


void init_token(token *tk, int state, char_buffer *buffer)
{
  tk->len=0;
  memcpy (&tk->begin, &buffer->pos, sizeof (position));
  append_token(tk,state,buffer);

}

void append_token(token *tk, int state, char_buffer *buffer)
{
  int len = tk->len;
  tk->value.string [len] = prev_char (buffer);
  tk->len=len+1;

}

void skip_token(token *tk, int state, char_buffer *buffer)
{
  /* do nothing */
}

void acc_identifier(token *tk)
{
  char *string=tk->value.string;
  tk->value.string [tk->len]=0;

  for (int i=0;i<N_KEYWORDS;++i)
  {
    if (keywords_tab[i] && 0==(strcmp(keywords_tab [i], string)))
    {
      tk->type = i;
      return;
    }
  }
  tk->type = TKT_IDENTIFIER;

}

void acc_one_char (token *tk, char character)
{
  tk->type=char2type[character];
  tk->value.string [tk->len]=0;
}

/** note: peek_char(buffer) will be one char pass the 
 * char that caused the *ACCEPT*
 * so use prev_char (buffer) to get the char
 */
void accept_token(token *tk, int state, char_buffer *buffer)
{
  memcpy (&tk->end, &buffer->pos, sizeof (position));
  char _char =peek_char (buffer);

  switch (state) {
    case TK_IDENTIFIER_END:
      acc_identifier(tk);
      put_char(buffer);
      return;

    case TK_ONE_CHAR_END:
      init_token (tk, TK_ONE_CHAR_END, buffer);
      acc_one_char (tk, prev_char (buffer));
      return ;

    default:
      break;
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
