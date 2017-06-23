#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "token_defs.h"
#include "transfer.h"

bool is_oper_type (node state);
  static 
token *_init_token(position *begin, size_t len, token_len_type type)
{

  token *tk=malloc(len);
  assert (tk);
  memset(tk,0,len);
  memcpy(&tk->begin,begin,sizeof(position));
  tk->type=TKT_UNKNOWN;
  tk->_type=type;
  return tk;

}

void check_init_token(void)
{
  token *tk;
  position pos={0};
  position *begin=&pos;
  char tstring[TOKEN_FIXLEN_MAX_LEN -1 ]={'a'};
  char sstring[TOKEN_VARLEN_INIT_LEN ]={'b'};
  char *string;
  int len;

  puts("check_init_token begin");
  for (int i=TFE_BRIEF;i<_TFE_LEN_TYPE_END;++i)
  {
    switch(i) {
      case TFE_FIXLEN:
        tk=init_fixlen(begin,'1');
        len = ((fixlen_token*)tk)->len;
        string = ((fixlen_token*)tk)->string;
        assert (TOKEN_LEN_TYPE(tk) == TFE_FIXLEN);
        assert (len  == 0);
        strncpy (string, tstring, TOKEN_FIXLEN_MAX_LEN);
        puts(string);
        break;

      case TFE_VARLEN:
        tk=init_varlen(begin,'1');
        len = ((varlen_token*)tk)->len;
        string = ((varlen_token*)tk)->string;
        assert (TOKEN_LEN_TYPE(tk) == TFE_VARLEN);
        assert (len  == 0);
        strncpy (string, tstring, TOKEN_VARLEN_INIT_LEN);
        puts(string);
        break;

      case TFE_BRIEF:
        break;
    }
  }
  puts("check_init_token passed");
}


token *init_breif(position *begin)
{
  return _init_token(begin,sizeof(breif_token),TFE_BRIEF);
}

token *init_fixlen(position *begin, char ch)
{
  fixlen_token *ftk=_init_token(begin,sizeof(fixlen_token),TFE_FIXLEN);
  append_fixlen(ftk,ch);
  return ftk;
}

token *init_varlen(position *begin, char ch)
{
  varlen_token *tk= (varlen_token*) _init_token(begin,sizeof(varlen_token), TFE_VARLEN);
  tk->string=malloc(sizeof (char)*( TOKEN_VARLEN_INIT_LEN + 1));
  tk->max=TOKEN_VARLEN_INIT_LEN;
  assert(tk->string);
  append_varlen(tk,ch);
  return (token*) tk;

}

int append_fixlen(token *_tk, char ch)
{
  fixlen_token *tk= (fixlen_token*) _tk;

  if (tk->len == TOKEN_FIXLEN_MAX_LEN)
    return E_FIXLEN_TOO_LONG;
  tk->string[tk->len++]=ch;
  return 0;
}

void check_append_fixlen(void)
{
  token *tk;
  position pos;
  tk=init_fixlen(&pos,'0');
  int r;

  for (int i=0;i<TOKEN_FIXLEN_MAX_LEN-1;++i)
  {
    r=append_fixlen(tk,'a');
    assert(r==0);
  }
  puts(TOKEN_FIXLEN_STRING(tk));
  r=append_fixlen(tk,'a');
  assert (r!=0);
  puts("check_append_fixlen passed");
}


int append_varlen(token *_tk, char ch)
{
  char *newspace;
  varlen_token *tk=(varlen_token*)_tk;

  if (tk->len == tk->max)
  {
    tk->max=2 *  (tk->len);
    newspace=malloc(sizeof(char) * tk->max);
    if (!newspace)
    {
      perror("malloc");
      return E_NOMEM;
    }

    memcpy(newspace,tk->string,sizeof(char)*(tk->len));
    newspace[tk->len++]=ch;
    free(tk->string);
    tk->string=newspace;
    return 0;
  }
  tk->string[tk->len++]=ch;
  return 0;

}

// TODO: check_varlen_append
int accept_brief(token *tk,char ch, node state, bool append)
{
  if (is_oper_type(state))
  {
    (tk)->type=state2operator[state];
    return 0;
  }
  switch(state) {
    case TK_PUNCTUATION_END:
    case TK_PERIOD_END:
      tk->type=state2punctuation[state];
      return 0;

    default:
      return E_TOKEN_NOT_BREIF;
  }

}

int accept_varlen(token *tk,char ch,node state, bool append)
{
  int r=0;
  varlen_token *vtk=(varlen_token*)tk;

  if(append && (r=append_varlen(tk,ch)) != 0)
    return r;

  vtk->string[vtk->len]=0;
  switch (state) {
    case TK_IDENTIFIER_END:
      tk->type=TKT_IDENTIFIER;
      return 0;

    case TK_STRING_LITERAL_END:
      tk->type=TKT_STRING_LITERAL;
      return 0;
    default:
      return E_TOKEN_NOT_VARLEN;
  }

}

int accept_fixlen(token *tk,char ch,node state,bool append)
{
  int r=0;
  fixlen_token *ftk=(fixlen_token*)tk;

  if(append && (r=append_fixlen(tk,ch)) != 0)
    return r;

  ftk->string[ftk->len]=0;
  switch (state) {
    case TK_INT_END:
      tk->type=TKT_INTEGER_LITERAL;
      break;

    case TK_CHAR_LITERAL_END:
      tk->type=TKT_CHARACTER_LITERAL;
      return 0;

    case TK_FLOAT_END:
      tk->type=TKT_FLOAT_LITERAL;
      return 0;
    default:
      return E_TOKEN_NOT_FIXLEN;
  }

}


  
bool is_oper_type (node state)
{
  return _TK_OPERATOR_ACCEPT_BEGIN < state && state < _TK_OPERATOR_ACCEPT_END;
}


char *format_token (token *tk) 
{
  static char buf [ BUFSIZ ];
  breif_token *btk;
  fixlen_token *ftk;
  varlen_token *vtk;
  const char *type_string= token_tab [TOKEN_TYPE(tk)];
  position *begin= &tk->begin;
  char *string="";

  switch (TOKEN_LEN_TYPE(tk)) {
    case TFE_FIXLEN:
      string=((fixlen_token*)tk)->string;
      break;

    case TFE_VARLEN:
      string=((varlen_token*)tk)->string;
      break;
  }

  snprintf ( buf , BUFSIZ, "<%s> <%s> <%d>", 
      type_string, string, begin->lineno);

  return buf;

}

void fini_token(token *tk)
{
  char *string;
  switch(TOKEN_LEN_TYPE(tk)) 
  {
    case TFE_FIXLEN:
      string=TOKEN_FIXLEN_STRING(tk);
      free(string);
      break;

    case TFE_VARLEN:
      string=TOKEN_VARLEN_STRING(tk);
      free(string);
      break;
  }
  free(tk);
}

