/* tknzr_driver.c */
#include "token_buffer.h"
#include "char_buffer.h"
#include "token.h"

char_buffer cbuffer;

int print_token_stream (void)
{
  token *tk;
  while (true)
  {
    switch (get_token(&tk))
    {
      case 0:
        puts(format_token(tk));
        continue;
      case EOF:
        return 0;
      case 1:
        return 1;
    }
  }
}



int main(int ac,char**av){ 
  if (ac!=2)
  {
    printf("Usage: tokenizer <file>\n");
    exit(0);
  }

  char *filename=av[1];
  if (init_char_buffer_from_file(&cbuffer, filename)<0)
  {
    perror(filename);
    exit(1);
  }

  exit(print_token_stream());

}

