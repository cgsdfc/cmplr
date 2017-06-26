/* tokenizer.c */
#include "tknzr_table.h"
#include "tknzr_error.h"
#include "tknzr_state.h"

static bool do_check=false;
char *filename;
char_buffer cbuffer;

  token *
depatch_init(token_len_type len_type , position *pos,char ch)
{
  switch (len_type) {
    case TFE_BRIEF :
      return init_breif(pos,ch);
      break;

    case TFE_FIXLEN:
      return init_fixlen(pos,ch);
      break;

    case TFE_VARLEN:
      return init_varlen(pos,ch);

    default:
      return init_breif(pos,ch);

  }
}

int depatch_append(token_len_type len_type, token *tk,  char ch)
{
  int r=0;

  switch (len_type) {
    case TFE_VARLEN :
      if (append_varlen(tk,ch)<0)
        return -1;
      return 0;

    case TFE_FIXLEN :
      if (append_fixlen(tk,ch)<0)
        return -1;
      return 0;
  }
}

int depatch_accept(token_len_type len_type, 
    token *tk,
    char ch,
    tknzr_state state,
    bool append)
{
  int r=0;

  switch (len_type) {
    case TFE_VARLEN:
      if ((r=accept_varlen(tk,ch,state, append))<0)
        return -1;
      return 0;

    case TFE_FIXLEN:
      if ((r=accept_fixlen(tk,ch,state, append))<0)
        return -1;
      return 0;

    default:
    case TFE_BRIEF:
      if ((r=accept_brief(tk,ch,state,append))<0)
        return -1;
      return 0;
  }
}


int get_next_token (token **ptoken,
    char_buffer *buffer,
    tknzr_state *errstate)
{
  
  entry_t entry;
  bool is_accepted=false;
  bool is_reversed=false;
  tknzr_state state=TK_INIT;
  token_len_type len_type;
  entry_action action;
  char ch;
  int r;
  token *tk=NULL;

  while (!is_accepted) 
  {
    ch = get_char (buffer);
    if(ch == EOF) 
    {
      tknzr_error_set(state == TK_INIT ? TERR_END_OF_INPUT:
          TERR_UNEXPECTED_END);
      return -1;
    }

    state = st_do_transfer(tknzr_table, state, ch, &entry);
    if (state == TK_NULL)
    {
      tknzr_error_set(TERR_UNEXPECTED_CHAR);
      return -1;
    }

    is_accepted = TFE_IS_ACCEPTED(entry);
    len_type = TFE_LEN_TYPE(entry);
    is_reversed = TFE_IS_REVERSED(entry);
    action=TFE_ACTION(entry);

    switch (action)
    {
      case TFE_ACT_ACCEPT:
        if ((r=depatch_accept(len_type, tk, ch, state, !is_reversed))!=0)
          return -1;

        if (is_reversed)
          put_char (buffer);

        *ptoken=tk;
        return 0;

      case TFE_ACT_APPEND:
        if ((r=depatch_append(len_type,tk,ch))!=0)
          return -1;
        break;

      case TFE_ACT_INIT:
        if((tk=depatch_init(len_type, &buffer->pos,ch))==NULL)
          return -1;
        break;

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
  tknzr_state errstate;

  if(do_check)
  {
    check_tknzr_table();
  }
  init_tknzr_table ();
  while (true)
  {
    get_next_token(&tk, &cbuffer, &errstate);
    switch (r)
    {
      case 0:
        token_string = format_token (tk);
        puts(token_string);
        break;
      case -1:
        if (tknzr_error_get() == TERR_END_OF_INPUT)
        {
          return 0;
        }
        else {
          tknzr_error_handle(errstate);
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



