/* tknzr_driver.c */
#include "token_buffer.h"
#include "char_buffer.h"
#include "token.h"
#include "tokenizer.h"

char_buffer cbuffer;
token_buffer buf;

int
unbuffer_print_token_stream (void)
{
  token tk;
  while (true)
    {
      memset (&tk, 0, sizeof (token));
      switch (get_next_token (&cbuffer, &tk))
	{
	case 0:
	  puts (format_token (&tk));
	  continue;
	case EOF:
	  return 0;
	default:
	  tknzr_error_handle (&cbuffer);
	  return 1;
	}
    }
}

int
print_token_stream (void)
{
  /* token *tk; */
  token *tk;
  while (true)
    {
      switch (get_token (&buf, &tk))
	{
	case 0:
	  puts (format_token (tk));
	  continue;
	case EOF:
	  return 0;
	case 1:
	  tknzr_error_handle (&cbuffer);
	  return 1;
	}
    }
  return 0;
}

void
print_buffer (void)
{
  for (int i = 0; i < cbuffer.end; ++i)
    {
      putchar (cbuffer.buf[i]);
    }
}



int
main (int ac, char **av)
{
  if (ac != 2)
    {
      printf ("Usage: tokenizer <file>\n");
      exit (0);
    }

  char *filename = av[1];
  if (init_char_buffer_from_file (&cbuffer, filename) < 0)
    {
      perror (filename);
      exit (1);
    }

  init_tokenizer ();
  exit (unbuffer_print_token_stream ());

}
