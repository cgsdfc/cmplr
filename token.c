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

/** NOTICE: do not change this */
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

  /* BINARY OPERATORS */
  [TKT_BINARY_OP_ADD]="TKT_BINARY_OP_ADD", // +
  [TKT_BINARY_OP_SUB]="TKT_BINARY_OP_SUB", // -
  [TKT_BINARY_OP_DIV]="TKT_BINARY_OP_DIV", // /
  [TKT_BINARY_OP_MUL]="TKT_BINARY_OP_MUL", // *
  [TKT_BINARY_OP_MOD  ]="TKT_BINARY_OP_MOD  ", // %
  [TKT_BINARY_OP_MOD_ASSIGN  ]="TKT_BINARY_OP_MOD_ASSIGN  ", // %=
  [TKT_BINARY_OP_ADD_ASSIGN]="TKT_BINARY_OP_ADD_ASSIGN", // +=
  [TKT_BINARY_OP_SUB_ASSIGN]="TKT_BINARY_OP_SUB_ASSIGN", // -=
  [TKT_BINARY_OP_DIV_ASSIGN]="TKT_BINARY_OP_DIV_ASSIGN", // /=
  [TKT_BINARY_OP_MUL_ASSIGN]="TKT_BINARY_OP_MUL_ASSIGN", // *=


  [TKT_BINARY_OP_ASSIGN]="TKT_BINARY_OP_ASSIGN", // =

  [TKT_BINARY_OP_BIT_AND_ASSIGN]="TKT_BINARY_OP_BIT_AND_ASSIGN", // &=
  [TKT_BINARY_OP_BIT_OR_ASSIGN]="TKT_BINARY_OP_BIT_OR_ASSIGN", // |=
  [TKT_BINARY_OP_BIT_XOR_ASSIGN]="TKT_BINARY_OP_BIT_XOR_ASSIGN", // ^=
  [TKT_BINARY_OP_BIT_LEFT_SHIFT_ASSIGN]="TKT_BINARY_OP_BIT_LEFT_SHIFT_ASSIGN", // <<=
  [TKT_BINARY_OP_BIT_RIGHT_SHIFT_ASSIGN]="TKT_BINARY_OP_BIT_RIGHT_SHIFT_ASSIGN", // >>=


  [TKT_BINARY_OP_BIT_AND]="TKT_BINARY_OP_BIT_AND", // &
  [TKT_BINARY_OP_BIT_OR]="TKT_BINARY_OP_BIT_OR", // |
  [TKT_BINARY_OP_BIT_XOR]="TKT_BINARY_OP_BIT_XOR", // ^
  [TKT_BINARY_OP_BIT_LEFT_SHIFT]="TKT_BINARY_OP_BIT_LEFT_SHIFT",// >>
  [TKT_BINARY_OP_BIT_RIGHT_SHIFT]="TKT_BINARY_OP_BIT_RIGHT_SHIFT", // <<

  [TKT_BINARY_OP_LOGICAL_AND]="TKT_BINARY_OP_LOGICAL_AND", // && 
  [TKT_BINARY_OP_LOGICAL_OR ]="TKT_BINARY_OP_LOGICAL_OR ", // ||
  [TKT_BINARY_OP_CMP_LESS  ]="TKT_BINARY_OP_CMP_LESS  ", // <
  [TKT_BINARY_OP_CMP_LESS_EQUAL  ]="TKT_BINARY_OP_CMP_LESS_EQUAL  ", // <=
  [TKT_BINARY_OP_CMP_GREATER  ]="TKT_BINARY_OP_CMP_GREATER  ", // >
  [TKT_BINARY_OP_CMP_GREATER_EQUAL ]="TKT_BINARY_OP_CMP_GREATER_EQUAL ", // >=
  [TKT_BINARY_OP_CMP_EQUAL ]="TKT_BINARY_OP_CMP_EQUAL ", // == 
  [TKT_BINARY_OP_CMP_NOT_EQUAL  ]="TKT_BINARY_OP_CMP_NOT_EQUAL  ", // !=
  [TKT_BINARY_OP_MEMBER_DOT  ]="TKT_BINARY_OP_MEMBER_DOT  ",// a.c
  [TKT_BINARY_OP_MEMBER_ARROW]="TKT_BINARY_OP_MEMBER_ARROW", // a->c

  /* UNARY OPERATORS */
  [TKT_UNARY_OP_LOGICAL_NOT  ]="TKT_UNARY_OP_LOGICAL_NOT  ", // !
  [TKT_UNARY_OP_BIT_NOT  ]="TKT_UNARY_OP_BIT_NOT  ",//~a
  [TKT_UNARY_OP_PLUS_PLUS  ]="TKT_UNARY_OP_PLUS_PLUS  ",//++a
  [TKT_UNARY_OP_MINUS_MINUS  ]="TKT_UNARY_OP_MINUS_MINUS  ",//--a
  [TKT_UNARY_OP_NEGATIVE  ]="TKT_UNARY_OP_NEGATIVE  ",//-a
  [TKT_UNARY_OP_ADDRESS  ]="TKT_UNARY_OP_ADDRESS  ",// &a
  [TKT_UNARY_OP_DEREFERENCE  ]="TKT_UNARY_OP_DEREFERENCE  ", // *p

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

};

// TODO: map accepted operator state to operator type's
const static token_type state2operator []=
{
  [TK_SLASH_END]=TKT_BINARY_OP_DIV,
  [TK_SLASH_EQUAL]=TKT_BINARY_OP_DIV_ASSIGN,

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
  void acc_one_char(token *tk, char);
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

    case TK_ONE_CHAR_END:
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
