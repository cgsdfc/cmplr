#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "state.h"
#include "char_buffer.h"


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

void _set_token_type(token *tk, token_type type)
{
  tk->type=type;
}

void init_token(token *tk, tokenizer_state state, char_buffer *buffer)
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

void append_token(token *tk, tokenizer_state state, char_buffer *buffer)
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

void skip_token(token *tk, tokenizer_state state, char_buffer *buffer)
{
  /* do nothing */
#ifndef NDEBUG // when debug, print out the comment we collect so far
  switch (state) {
    case TK_MULTI_LINE_COMENT_BEGIN:
    case TK_SINGLE_LINE_COMENT_BEGIN:
    case TK_MULTI_LINE_COMENT_END:
      printf ("%c", prev_char (buffer));
      return;
  }
#endif
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
  
bool is_tokenizable_operator(tokenizer_state state)
{
  return _TK_OPERATOR_ACCEPT_BEGIN < state && state < _TK_OPERATOR_ACCEPT_END;
}

/** note: peek_char(buffer) will be one char pass the 
 * char that caused the *ACCEPT*
 * so use prev_char (buffer) to get the char
 */
void accept_token(token *tk, tokenizer_state state, char_buffer *buffer)
{

  char _char;
  void acc_integer(token *tk);
  if (is_tokenizable_operator (state))
  {
    _set_token_type (tk, state2operator[state]);
    _terminate_token(tk);
    return;
  }
  switch (state) {
    case TK_IDENTIFIER_END:
      _terminate_token(tk);
      acc_identifier(tk);
      put_char(buffer);
      _set_token_pos(tk,buffer,POS_END);
      return;

    case TK_PUNCTUATION_END:
      _char=prev_char(buffer);
      _clear_token(tk);
      _catchar_token(tk,_char);
      _terminate_token(tk);
      _set_token_pos(tk,buffer,POS_BEGIN);
      _set_token_pos(tk,buffer,POS_END);
      _set_token_type(tk, char2type[_char]);
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
