/* tokenizer.c */
#include <stdbool.h>
#include<stdlib.h>
#include <assert.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "tokenizer.h"


struct token;
static transfer_entry transfer_table[MAX_TRANSFER_ENTRIES][MAX_TRANSFER_ENTRIES];

void init_transfer (void)
{
  for (int i=0;i<MAX_TRANSFER_ENTRIES;++i) 
  {
    for (int j=0;j<MAX_TRANSFER_ENTRIES;++j)
    {
      transfer_table[i][j].state = TK_NULL;
      transfer_table[i][j].tkz_act = append_token;
      transfer_table[i][j].char_class = "";
      transfer_table[i][j].is_accepted = false;
      transfer_table[i][j].is_reversed = false;
    }
  }

}

  
int add_transfer(int this_state, int state, bool is_accepted, bool is_reversed, char *char_class, tkz_action act)
{
  static int entry_counters[MAX_TRANSFER_ENTRIES]={0};
  int entry_counter = entry_counters[this_state];
  transfer_entry *this_entry = &transfer_table[this_state][entry_counter];

  this_entry->state = state;
  this_entry->is_accepted = is_accepted;
  this_entry->is_reversed = is_reversed;
  this_entry->char_class = char_class;
  this_entry->tkz_act = act;

  entry_counters[this_state]=entry_counter+1;
  return entry_counter;

}

bool can_transfer(transfer_entry *entry, int character) 
{
  // TODO: delete it
  if (entry->char_class == NULL) 
  {
    return false;
  }
  bool is_in_class = strchr (entry->char_class, character);
  return (is_in_class || entry->is_reversed);

}

tokenizer_state do_transfer (int state, int character, transfer_entry **entry)
{
  tokenizer_state next_state;

  for (int i=0;i<MAX_TRANSFER_ENTRIES;++i)
  {
    next_state = transfer_table[state][i].state;
    *entry = &transfer_table[state][i];
    if (can_transfer(&transfer_table[state][i], character))
      break;
  }

  return next_state;

}

transfer_entry *seek_entry (int state_from, int state_to)
{
  transfer_entry *entry;
  for (int i=0;i<MAX_TRANSFER_ENTRIES;++i)
  {
    entry = &transfer_table[state_from][i];
    if (entry->state == state_to) 
    {
      break;
    }
  }
  return entry;
}

void init_table (void)
{
  init_transfer();

  add_transfer(TK_INIT, TK_IDENTIFIER_BEGIN, false, false, CHAR_CLASS_IDENTIFIER_BEGIN, init_token);
  add_transfer(TK_INIT, TK_INIT, false, false, CHAR_CLASS_SPACES, skip_token);
  add_transfer(TK_IDENTIFIER_BEGIN, TK_IDENTIFIER_END, true, false, CHAR_CLASS_SPACES, accept_token);
  add_transfer(TK_IDENTIFIER_BEGIN, TK_IDENTIFIER_BEGIN, false, false, CHAR_CLASS_IDENTIFIER_PART, append_token);

}

void check_init_table (void)
{
  void init_table(void);
  bool can_transfer(transfer_entry *entry, int character);
  init_table ();

  /* TK_INIT -> TK_IDENTIFIER_BEGIN */
  for (int i=0;i<strlen (CHAR_CLASS_IDENTIFIER_BEGIN);++i) 
  {
    int letter = CHAR_CLASS_IDENTIFIER_BEGIN[i];
    assert (can_transfer (seek_entry(TK_INIT, TK_IDENTIFIER_BEGIN), letter));

  }

  /* check TK_INIT -> TK_INIT */ 
  for (int i=0;i<strlen (CHAR_CLASS_SPACES); ++i)
  {
    int space = CHAR_CLASS_SPACES[i];
    assert (can_transfer (seek_entry(TK_INIT, TK_INIT), space));
  }

  /* check TK_IDENTIFIER_BEGIN -> TK_IDENTIFIER_END */
  for (int i=0;i<strlen (CHAR_CLASS_SPACES); ++i)
  {
    int letter = CHAR_CLASS_SPACES[i]; 
    assert (can_transfer (seek_entry(TK_IDENTIFIER_BEGIN, TK_IDENTIFIER_END), letter));
  }

  /* check TK_IDENTIFIER_BEGIN -> TK_IDENTIFIER_BEGIN */
  for (int i=0;i<strlen (CHAR_CLASS_IDENTIFIER_PART); ++i)
  {
    int letter =CHAR_CLASS_IDENTIFIER_PART[i]; 
    assert (can_transfer (seek_entry(TK_IDENTIFIER_BEGIN, TK_IDENTIFIER_BEGIN), letter));
  }

  printf ("check_init_table passed\n");

}


int get_next_token(token *tk, char_buffer *buffer)
{
  if (peek_char (buffer) == EOF) {
    return EOF;
  }

  transfer_entry *entry;
  bool accepted=false;
  int state = TK_INIT;
  static bool last_accepted;

  while (!accepted) {
    int _char = get_char (buffer);
    if (_char == EOF) {
      if (state == TK_INIT) 
        return EOF;
      else
        return E_PREMATURE_END;
    }
    state = do_transfer (state ,_char,&entry);
    if (state == TK_NULL) {
      put_char(buffer);
      return E_UNEXPECTED_CHAR;
    }
    accepted = entry->is_accepted;
    entry->tkz_act(tk,state, buffer);
  }
  return 0;

}

void check_all(void)
{
  void check_char_buffer(void);
  check_char_buffer();
  
  check_init_table();
}

const char *tokenizer_err_tab [] =
{
  [E_PREMATURE_END]="premature end of input",
  [E_UNEXPECTED_CHAR]="unexpected character"

};

void tokenizer_error (int error, char_buffer *buffer, token *tk)
{
  char *errline = peek_line (buffer, get_lineno(buffer));
  fprintf(stderr, "line %d, column %d: error: %s\n\n",
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

  if (init_char_buffer (&buffer, av[1])<0)
  {
    perror (av[0]);
    goto error;
  }

  init_table();
  while ((r = get_next_token(&tk, &buffer)) != EOF)
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
  tokenizer_error(r,&buffer, &tk);
  exit(1);
}



