/* tokenizer.c */
#include <stdbool.h>
#include<stdlib.h>
#include <assert.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "tokenizer.h"
#include "token_defs.h"

static int tknzr_errno;
// TODO: give more sensible error msg 
// based on errstate
const char *tokenizer_err_tab [] =
{

  [E_PREMATURE_END]="premature end of input",
  [E_UNEXPECTED_CHAR]="unexpected character"

};

token *
depatch_init(token_len_type len_type , position *pos,char ch)
{
  switch (len_type) {
    case   TFE_BRIEF :
      return init_breif(pos);
      break;

    case TFE_FIXLEN:
      return init_fixlen(pos,ch);
      break;

    case TFE_VARLEN:
      return init_varlen(pos,ch);

    default:
      return NULL;

  }
}

int depatch_append(token_len_type len_type, token *tk,  char ch)
{
  int r=0;

  switch (len_type) {
    case TFE_VARLEN :
      if (append_varlen(tk,ch)!=0)
        return r;
      return 0;

    case TFE_FIXLEN :
      if (append_fixlen(tk,ch)!=0)
        return r;
      return 0;

    default:
      return r;
  }
}

int depatch_accept(token_len_type len_type, 
    token *tk,
    char ch,
    node state,
    bool append)
{
  int r=0;

  switch (len_type) {
    case TFE_VARLEN:
      if ((r=accept_varlen(tk,ch,state, append))!=0)
        return r;
      return 0;

    case TFE_FIXLEN:
      if ((r=accept_fixlen(tk,ch,state, append))!=0)
        return r;
      return 0;

    case TFE_BRIEF:
      if ((r=accept_brief(tk,ch,state,append))!=0)
        return r;
      return 0;
  }
}

int get_next_token (token **ptoken,
    char_buffer *buffer,
    node *errstate)
{
  if (peek_char(buffer) == EOF) {
    return EOF;
  }

  transfer_entry entry;
  bool is_accepted=false;
  bool is_reversed=false;
  tokenizer_state state=TK_INIT;
  tokenizer_state nstate = TK_NULL;
  token_len_type len_type;
  entry_action action;
  int ch;
  int r;
  token *tk;

  while (!is_accepted) 
  {
    ch = get_char (buffer);
    if(ch == EOF) 
    {
      r=(state == TK_INIT)?EOF:E_PREMATURE_END;
      goto error;
    }

    nstate = do_transfer(state, ch, &entry);
    if (nstate == TK_NULL)
    {
      r=E_UNEXPECTED_CHAR;
      goto error;
    }

    is_accepted = TFE_IS_ACCEPTED(entry);
    len_type = TFE_LEN_TYPE(entry);
    is_reversed = TFE_IS_REVERSED(entry);
    state = nstate;
    action=TFE_ACTION(entry);

    switch (action)
    {
      case TFE_ACT_ACCEPT:
        /* if !is_reversed, that means the last char of a token is */
        /*   also part of it. */
        if ((r=depatch_accept(len_type, tk, ch, state, !is_reversed))!=0)
          goto error;

        if (is_reversed)
          put_char (buffer);

        *ptoken=tk;
        return 0;

      case TFE_ACT_APPEND:
        if ((r=depatch_append(len_type,tk,ch))!=0)
          goto error;
        break;

      case TFE_ACT_INIT:
        tk=depatch_init(len_type, &buffer->pos,ch);
        break;

      case TFE_ACT_SKIP:
        break;
    }
  } 
error:
  *errstate=state;
  return r;

}


void tokenizer_error (int error, char_buffer *buffer, token *tk, tokenizer_state last_state)
{
  char *errline = peek_line (buffer, get_lineno(buffer));
  fprintf(stderr, "state %s, line %d, column %d: error: %s\n\n", token_state_tab[last_state],
      buffer->pos.lineno, buffer->pos.column, tokenizer_err_tab[error]);
  fprintf(stderr, "-> %s", errline);

  switch (error) {
    case E_PREMATURE_END:
      break;

    case E_UNEXPECTED_CHAR:
      fprintf(stderr,"`%c\' is unexpected\n", peek_char (buffer));
      break;
    default:
      break;
  }

}

int print_token_stream (char_buffer *buffer)
{
  token *tk;
  int r=0;
  char *token_string;
  tokenizer_state errstate;

  check_tknzr_table();
  init_tknzr_table ();
  while ((r = get_next_token(&tk, buffer, &errstate)) != EOF)
  {
    switch (r)
    {
      case 0:
        token_string = format_token (tk);
        puts(token_string);
        break;

      default:
        goto error;
    }
  }
  return 0;

error:
  tokenizer_error(r,buffer, tk, errstate);
  return 1;

}

int pss (char *string)
{
  assert (string);
  char_buffer cb;
  if (init_char_buffer_from_string(&cb, string)<0)
    return -1;

  if (print_token_stream(&cb)!=0)
    return -1;

  clear_buffer(&cb);
  return 0;
}

int main(int ac,char**av){ 

  if (ac != 2)
  {
    printf("Usage: %s input \n", av[0]);
    exit(0);
  }


  FILE *input;
  token tk;
  int r=0;
  char *token_string;
  char_buffer buffer;
  tokenizer_state errstate;

  if (init_char_buffer_from_file (&buffer, av[1])<0)
  {
    perror (av[1]);
    exit(1);
  }

  exit (print_token_stream(&buffer));



}



