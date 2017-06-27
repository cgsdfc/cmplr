#include "token.h"

token *alloc_token (position *begin)
{
// TODO use token_buffer instead
  token *tk=malloc(sizeof(token));
  if (!tk) {
    tknzr_error_set(TERR_NOMEM);
    return NULL;
  }

  memset(tk,0,sizeof (token));
  memcpy(&tk->begin, begin, sizeof (position));
  tk->type=TKT_UNKNOWN;
  return tk;

}

token *init_varlen(position *begin, char ch)
{
  token *tk=alloc_token(begin);
  tk->string=malloc(sizeof (char)*( TOKEN_VARLEN_INIT_LEN + 1));
  tk->max=TOKEN_VARLEN_INIT_LEN;
  if (!tk->string) { return NULL; }
  append_varlen((token*)tk,ch);
  return tk;
}


int append_varlen(token *tk, char ch)
{
  char *newspace;
  if (tk->len == tk->max)
  {
    tk->max=2 *  (tk->len);
    newspace=malloc(sizeof(char) * tk->max);
    if (!newspace)
    {
      tknzr_error_set(TERR_NOMEM);
      return -1;
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

token *accept_punctuation (position *begin, char ch)
{
  token *tk=alloc_token(begin);
  tk->type=char2punctuation(ch);
  return tk;
}

token *accept_operator (position *begin, tknzr_state state)
{
  token *tk=alloc_token(begin);
  if (!tk) { return NULL; }
  (tk)->type=state2oper(state);
  return tk;
}


int accept_varlen(token *tk,char ch,tknzr_state state)
{

  tk->string[tk->len]=0;
  switch (state) {
    case TK_IDENTIFIER_END:
      for (int i=FIRST_KEYWORD;i<=LAST_KEYWORD;++i)
      {
        const char *kw=keywords_tab[i];
        if (kw != NULL && strcmp (tk->string, kw)==0)
        {
          tk->type=i;
          return 0;
        }
      }
      tk->type=TKT_IDENTIFIER;
      return 0;

    case TK_CHAR_LITERAL_END:
      if(append_varlen(tk,ch) < 0)
        return -1;
      tk->type=TKT_CHARACTER_LITERAL;
      return 0;
    case TK_INT_END:
      tk->type=TKT_INTEGER_LITERAL;
      return 0;
    case TK_FLOAT_END:
      tk->type=TKT_FLOAT_LITERAL;
      break;
    case TK_STRING_LITERAL_END:
      if(append_varlen(tk,ch) < 0)
        return -1;
      tk->type=TKT_STRING_LITERAL;
      return 0;
  }
  return 0;
}


char *format_token (token *tk) 
{
  assert (tk);

  static char buf [ BUFSIZ ];
  const char *type_string= token_tab[TOKEN_TYPE(tk)];
  position *begin= &tk->begin;
  char *string=TOKEN_STRING(tk);

  snprintf ( buf , BUFSIZ, "<%s> <%s> <%d>", 
      type_string, string, begin->lineno);

  return buf;
}

void fini_token(token *tk)
{
  char *string;
  if (tk->string)
  {
    free(tk->string);
  } 
  free(tk);
}

