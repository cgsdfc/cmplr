/* transfer.c */
#include <stdbool.h>
#include<stdlib.h>
#include <assert.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "transfer.h"


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

  

void add_transfer(int this_state, int state, bool is_accepted, bool is_reversed, char *char_class, tkz_action act)
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
  for (int i=0;i<strlen (CHAR_CLASS_SEPARATOR); ++i)
  {
    int letter = CHAR_CLASS_SEPARATOR[i];
    assert (can_transfer (seek_entry(TK_IDENTIFIER_BEGIN, TK_IDENTIFIER_END), letter));
  }

  /* check TK_IDENTIFIER_BEGIN -> TK_IDENTIFIER_BEGIN */
  for (int i=0;i<strlen (CHAR_CLASS_IDENTIFIER_PART); ++i)
  {
    int letter =CHAR_CLASS_IDENTIFIER_PART[i]; 
    assert (can_transfer (seek_entry(TK_IDENTIFIER_BEGIN, TK_IDENTIFIER_BEGIN), letter));
  }

  /* check TK_INIT -> TK_ONE_CHAR_END */
  for (int i=0;i<strlen (CHAR_CLASS_ONE_CHAR); ++i)
  {
    int one_char = CHAR_CLASS_ONE_CHAR[i];
    assert (can_transfer (seek_entry(TK_INIT, TK_ONE_CHAR_END), one_char));
  }
  printf ("check_init_table passed\n");

}
void init_table (void)
{
  init_transfer();

  /* skip space */ 
  add_transfer(TK_INIT, TK_INIT, false, false, CHAR_CLASS_SPACES, skip_token);

  /* identifier */
  add_transfer(TK_INIT, TK_IDENTIFIER_BEGIN, false, false, CHAR_CLASS_IDENTIFIER_BEGIN, init_token);
  add_transfer(TK_IDENTIFIER_BEGIN, TK_IDENTIFIER_END, true, true, CHAR_CLASS_IDENTIFIER_PART, accept_token);
  add_transfer(TK_IDENTIFIER_BEGIN, TK_IDENTIFIER_BEGIN, false, false, CHAR_CLASS_IDENTIFIER_PART, append_token);

  /* one_char -- punctuation or operator */
  /* TODO: some operator need two chars, remove them from here */
  add_transfer(TK_INIT, TK_ONE_CHAR_END, true, false, CHAR_CLASS_ONE_CHAR, accept_token);

  /* integer -- dec, oct, hex, long or not , unsigned or not */
  add_transfer(TK_INIT, TK_INT_DEC_BEGIN, false, false, "123456789", init_token);
  add_transfer(TK_INT_DEC_BEGIN, TK_INT_DEC_BEGIN, false, false, CHAR_CLASS_TEN_DIGITS, append_token);
  add_transfer(TK_INT_DEC_BEGIN, TK_INT_END, true, false, CHAR_CLASS_SEPARATOR, accept_token);
  add_transfer(TK_INT_DEC_BEGIN, TK_INT_LONG, false, false, "lL", append_token);
  add_transfer(TK_INT_LONG, TK_INT_END, true, false, CHAR_CLASS_SEPARATOR, accept_token);
  add_transfer(TK_INT_LONG, TK_INT_UNSIGNED, false, false, "uU", append_token);
  add_transfer(TK_INT_UNSIGNED, TK_INT_END, true, false, CHAR_CLASS_SEPARATOR, accept_token);
  

}
int get_next_token(token *tk, char_buffer *buffer, tokenizer_state *errstate)
{
  if (peek_char (buffer) == EOF) {
    return EOF;
  }

  transfer_entry *entry;
  bool accepted=false;
  tokenizer_state state = TK_INIT;
  static bool last_accepted;

  while (!accepted) {
    *errstate = state;
    int _char = get_char (buffer);
    printf("state = %s char = %c\n", token_state_tab[state], _char);
    if (_char == EOF) {
      if (state == TK_INIT) 
        return EOF;
      else 
        return E_PREMATURE_END;
    }
    state = do_transfer (state ,_char,&entry);
    printf("state' = %s\n", token_state_tab[state]);
    if (state == TK_NULL) {
      put_char(buffer);
      return E_UNEXPECTED_CHAR;
    }
    accepted = entry->is_accepted;
    entry->tkz_act(tk,state, buffer);
  }
  return 0;

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

