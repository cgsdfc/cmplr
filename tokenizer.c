/* tokenizer.c */
#include <stdbool.h>
#include<stdlib.h>
#include <assert.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "tokenizer.h"




void check_all(void)
{
  void check_char_buffer(void);
  check_char_buffer();
  
  check_table();
}

const char *tokenizer_err_tab [] =
{
  [E_PREMATURE_END]="premature end of input",
  [E_UNEXPECTED_CHAR]="unexpected character"

};

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
  check_all();

  if (ac != 2)
  {
    printf("Usage: %s input \n", av[0]);
    goto error;
  }


  FILE *input;
  token tk;
  int r=0;
  char *token_string;
  char_buffer buffer;
  tokenizer_state errstate;

  if (init_char_buffer (&buffer, av[1])<0)
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



