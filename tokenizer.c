/* tokenizer.c */
#include <stdbool.h>
#include<stdlib.h>
#include <assert.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "tokenizer.h"



const char *tokenizer_err_tab [] =
{
  [E_PREMATURE_END]="premature end of input",
  [E_UNEXPECTED_CHAR]="unexpected character"

};


int get_next_token (token *tk, char_buffer *buffer, tokenizer_state *errstate)
{
  if (peek_char(buffer) == EOF) {
    return EOF;
  }

  transfer_entry entry;
  bool accepted=false;
  tokenizer_state state=TK_INIT;
  tokenizer_state nstate = TK_NULL;
  int _char;

  while (!accepted) 
  {
    *errstate=state;
    _char = get_char (buffer);
    if(_char == EOF) {
      if (state == TK_INIT)
        return EOF;
      else
        return E_PREMATURE_END;
    }

    nstate = do_transfer(state, _char, &entry);
    if (nstate == TK_NULL)
    {
      put_char(buffer);
      return E_UNEXPECTED_CHAR;
    }

    accepted = TFE_IS_ACCEPTED(entry);
    state = nstate;
    switch (TFE_ACTION(entry))
    {
      case TFE_ACT_ACCEPT:
        accept_token(tk,nstate,buffer);
        break;

      case TFE_ACT_APPEND:
        append_token(tk,nstate,buffer);
        break;

      case TFE_ACT_INIT:
        init_token(tk,nstate,buffer);
        break;
      case TFE_ACT_SKIP:
        skip_token(tk,nstate,buffer);
        break;
    }
  } 

  return 0;
}


void tokenizer_error (int error, char_buffer *buffer, token *tk, tokenizer_state last_state)
{
  char *errline = peek_line (buffer, get_lineno(buffer));
  fprintf(stderr, "state %s, line %d, column %d: error: %s\n\n", token_state_tab[last_state],
      buffer->pos.lineno, buffer->pos.column, tokenizer_err_tab[error]);
  fprintf(stderr, "-> %s", errline);

  switch (error) {
    case E_PREMATURE_END:
      tk->value.string[tk->len]=0;
      fprintf(stderr, "token `%s\' is unfinished\n", tk->value.string);
      break;

    case E_UNEXPECTED_CHAR:
      fprintf(stderr,"`%c\' is unexpected\n", peek_char (buffer));
      break;
    default:
      break;
  }

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
    perror (av[0]);
    exit(1);
  }

  init_table();
  while ((r = get_next_token(&tk, &buffer, &errstate)) != EOF)
  {
    switch (r)
    {
      case 0:
        token_string = format_token (&tk);
        puts(token_string);
        break;

      default:
        goto error;
    }
  }

  return 0;

error:
  tokenizer_error(r,&buffer, &tk, errstate);
  exit(1);
}



