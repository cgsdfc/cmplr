/* tokenizer.c */
#include "tknzr_table.h"
#include "tknzr_error.h"
#include "tknzr_state.h"

static bool do_check=false;
char *filename;
char_buffer cbuffer;
void depatch_errno(tknzr_state state);

int get_next_token (token **ptoken,
    char_buffer *buf)
{
  if (peek_char(buf) == EOF)
  {
    tknzr_error_set(TERR_END_OF_INPUT);
    return -1;
  }
  
  entry_t entry=0;
  bool is_reversed=false;
  bool is_varlen=false;
  tknzr_state state=TK_INIT;
  tknzr_state prev_state;
  char ch;
  int r;
  token *tk=NULL;

  while (!TFE_IS_ACCEPTED(entry))
  {
    ch = get_char (buf);
    if(ch == EOF) 
    {
      depatch_errno(state);
      return -1;
    }
    
    prev_state=state;
    state = st_do_transfer(tknzr_table, state, ch, &entry);
    if (state == TK_NULL)
    {
      depatch_errno(prev_state);
      return -1;
    }

    switch (TFE_ACTION(entry))
    {
      case TFE_ACT_ACCEPT:
        if (is_varlen_accept(state))
        {
          if (accept_varlen(tk,ch,state)<0)
            return -1;
          if (is_punctuation_char(ch) || 
              is_operator_char(ch))
          {
            put_char(buf);
          }
        }
        else if(is_operator_accept(state)) 
        {
          tk=accept_operator(&buf->pos,state);
        }
        if (is_punctuation_accept(ch))
        {
          tk=accept_punctuation (&buf->pos,ch);
        }

        if (tk == NULL) {
          return -1;
        }
        *ptoken=tk;
        if (TFE_IS_REVERSED(entry))
        {
          put_char (buf);
        }
        return 0;

      case TFE_ACT_APPEND:
        if (is_varlen && append_varlen(tk,ch)<0)
        {
          return -1;
        }
        break;

      case TFE_ACT_INIT:
        is_varlen=is_varlen_init(state);
        if (is_varlen && !( tk=init_varlen(&buf->pos,ch)))
        {
          return -1;
        }
        break;

        // TODO when comment is no longer 
        // handle by tokenizer, this is no need
      case TFE_ACT_SKIP:
        if(tk!=NULL)
        {
          fini_token(tk);
          tk=NULL;
        }

        break;
    }
  } 

}



int print_token_stream (void)
{
  token *tk;
  int r=0;
  char *token_string;
  int nall=0;

  if(do_check)
  {
    check_tknzr_table();
  }
  init_tknzr_table ();
  while (true)
  {
    r=get_next_token(&tk, &cbuffer);
    switch (r)
    {
      case 0:
        token_string = format_token (tk);
        puts(token_string);
        nall++;
        break;
      case -1:
        if (tknzr_error_get() == TERR_END_OF_INPUT)
        {
          printf("read %d tokens in total\n", nall);
          return 0;
        }
        else {
          tknzr_error_handle();
          return -1;
        }
    }
  }
}

int main(int ac,char**av){ 

  if (ac != 2)
  {
    puts(__BASE_FILE__);
    printf("Usage: %s input \n", av[0]);
    exit(0);
  }


  if (init_char_buffer_from_file (&cbuffer, av[1])<0)
  {
    perror (av[1]);
    exit(1);
  }

  filename=av[1];

  exit (print_token_stream());



}



