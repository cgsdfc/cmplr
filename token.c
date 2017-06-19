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
  [TKT_UNKNOWN]="TKT_UNKNOWN"

};

void init_token(token *tk, int state, char_buffer *buffer)
{
  tk->len=0;
  memcpy (&tk->begin, &buffer->pos, sizeof (position));
  memset (&tk->end, 0, sizeof (position));
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

void accept_token(token *tk, int state, char_buffer *buffer)
{
  char *string=tk->value.string;
  string [tk->len]=0;
  put_char (buffer);
  memcpy (&tk->end, &buffer->pos, sizeof (position));

  if (state == TK_IDENTIFIER_END)
  {
    for (int i=0;i<N_KEYWORDS;++i)
    {
      if (keywords_tab[i] && 0==(strcmp(keywords_tab [i], string)))
      {
        tk->type = i;
        return;
      }
    }
    tk->type = TKT_IDENTIFIER;
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
