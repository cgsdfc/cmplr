#include "cprd_transfer.h"
#include "operator.h"
#include "char_literal.h"

typedef tokenizer_state node;

static transfer_table_t table;
static int entry_counters[MAX_TRANSFER_ENTRIES];

void check_table (void) 
{
  void check_fields(void);
  void check_init_table(void);

  check_fields();
  check_init_table();

  printf ("check_table passed\n");

}

transfer_entry *seek_entry (int state_from, int state_to)
{
  assert (state_to >=0 && state_to < MAX_TRANSFER_ENTRIES);
  assert (state_from >=0 && state_from < MAX_TRANSFER_ENTRIES);

  transfer_entry *entry;
  for (int i=0;i<entry_counters[state_from];++i)
  {
    entry = &table[state_from][i];
    if (TFE_STATE(entry->entry) == state_to)
      return entry;
  }
  return NULL;
}


void check_fields(void)
{
  entry_t entry;
  entry_t flag, action, state;

  entry = TFE_MAKE_ENTRY(TFE_ACT_INIT, 0,0); 
  action = TFE_ACTION(entry);
  assert (action == TFE_ACT_INIT);

  entry = TFE_MAKE_ENTRY(0,TK_INT_DEC_BEGIN,0);
  state=TFE_STATE(entry);
  assert (state == TK_INT_DEC_BEGIN);

  entry = TFE_MAKE_ENTRY(0,0,TFE_FLAG_REVERSED);
  assert (TFE_IS_REVERSED(entry));

  entry = TFE_MAKE_ENTRY(TFE_ACT_INIT,TK_IDENTIFIER_BEGIN,0);
  assert (TFE_STATE(entry)==TK_IDENTIFIER_BEGIN);
  assert (TFE_ACTION(entry)==TFE_ACT_INIT);
  assert (TFE_IS_ACCEPTED(entry)==0);
  assert (TFE_IS_REVERSED(entry)==0);

  entry=TFE_MAKE_ENTRY(TFE_ACT_APPEND,TK_IDENTIFIER_BEGIN,0);
  assert (TFE_ACTION(entry)==TFE_ACT_APPEND);

  entry=TFE_MAKE_ENTRY(TFE_ACT_SKIP,TK_INIT,0);
  assert(TFE_ACTION(entry)==TFE_ACT_SKIP);

  entry=TFE_MAKE_ENTRY(TFE_ACT_ACCEPT,TK_IDENTIFIER_END,TFE_FLAG_ACCEPTED);
  assert(TFE_ACTION(entry)==TFE_ACT_ACCEPT);
  assert(TFE_IS_ACCEPTED(entry));
  assert(TFE_STATE(entry)==TK_IDENTIFIER_END);

  printf ("check_fields passed\n");

}


bool can_transfer(transfer_entry *entry, int character) 
{
  assert(entry != NULL);

  bool is_in_class = strchr (entry->char_class, character);
  return (is_in_class && ! TFE_IS_REVERSED(entry->entry)) || 
    (!is_in_class && TFE_IS_REVERSED(entry->entry));

}


void add_transfer(tokenizer_state from,
    tokenizer_state to, 
    entry_flag flags, entry_action act,
    char *char_class)
{
  assert (char_class!=NULL);
  int len=entry_counters[from];
  transfer_entry *entry=&table[from][len];

  /** action state flags */
  entry->entry = TFE_MAKE_ENTRY(act, to, flags);
  entry->char_class = char_class;

  entry_counters[from]++;

}

void add_initial(node to, char *char_class)
{
  add_transfer (TK_INIT, to, 0,TFE_ACT_INIT,char_class);
}


void add_intermedia_rev (node from, node to, char *char_class ) 
{
  add_transfer(from, to, TFE_FLAG_REVERSED, TFE_ACT_APPEND, char_class);
}

void add_intermedia (node from, node to, char* char_class) 
{
  add_transfer(from, to, 0, TFE_ACT_APPEND, char_class);
}

void add_selfloop (node from, char *char_class)
{
  add_intermedia(from,from,char_class);
}

void add_selfloop_rev (node from, char *char_class)
{
  add_intermedia_rev(from,from,char_class);
}

void add_accepted_rev(node from,  node to, char * char_class) 
{
  add_transfer(from, to, TFE_FLAG_REVERSED|TFE_FLAG_ACCEPTED, TFE_ACT_ACCEPT, char_class);
}
void add_accepted(node from, node to , char *char_class) 
{
  add_transfer(from, to, TFE_FLAG_ACCEPTED, TFE_ACT_ACCEPT, char_class);
}

tokenizer_state do_transfer(tokenizer_state state,
    int ch, transfer_entry **entry)
{
  assert (state >=0 && state < MAX_TRANSFER_ENTRIES);
  tokenizer_state nstate;

  for (int i=0;i<entry_counters[state];++i)
  {
    *entry = &table[state][i];
    nstate = TFE_STATE(table[state][i].entry);
    assert (nstate >=0 && nstate < MAX_TRANSFER_ENTRIES);
    if (can_transfer (&table[state][i], ch) ) 
    {
      return nstate;
    }
  }
  return TK_NULL;

}

/** for each char in `char_class`, check `from` can transfer to `to` */
void check_can_transfer (tokenizer_state from, tokenizer_state to, char *char_class)
{
  for (int i=0;i<strlen(char_class);++i) 
  {
    int letter = char_class[i];
    assert (can_transfer(seek_entry(from, to), letter));

  }

}

void check_init_table (void)
{
  void init_table(void);
  bool can_transfer(transfer_entry *entry, int character);
  init_table ();

  /* TK_INIT -> TK_IDENTIFIER_BEGIN */
  check_can_transfer(TK_INIT,TK_IDENTIFIER_BEGIN,CHAR_CLASS_IDENTIFIER_BEGIN);

  /* check TK_INIT -> TK_INIT */ 
  check_can_transfer(TK_INIT,TK_INIT,CHAR_CLASS_SPACES);

  /* check TK_IDENTIFIER_BEGIN -> TK_IDENTIFIER_END */
  check_can_transfer(TK_IDENTIFIER_BEGIN,TK_IDENTIFIER_END,CHAR_CLASS_SEPARATOR);

  /* check TK_IDENTIFIER_BEGIN -> TK_IDENTIFIER_BEGIN */
  check_can_transfer(TK_INIT,TK_IDENTIFIER_BEGIN,CHAR_CLASS_IDENTIFIER_BEGIN);

  /* check TK_INIT -> TK_PUNCTUATION_END */
  check_can_transfer(TK_INIT,TK_PUNCTUATION_END,CHAR_CLASS_PUNCTUATION);

  printf ("check_init_table passed\n");

}

void init_identifier(void)
{
  add_initial(TK_IDENTIFIER_BEGIN,CHAR_CLASS_IDENTIFIER_BEGIN);
  add_selfloop (TK_IDENTIFIER_BEGIN, CHAR_CLASS_IDENTIFIER_PART);
  add_accepted_rev(TK_IDENTIFIER_BEGIN, TK_IDENTIFIER_END, CHAR_CLASS_IDENTIFIER_PART);
}

void init_dec_integer(void)
{
  add_initial(TK_INT_DEC_BEGIN, CHAR_CLASS_DEC_BEGIN);
  add_selfloop(TK_INT_DEC_BEGIN, CHAR_CLASS_DEC_PART);
  add_intermedia(TK_INT_DEC_BEGIN,TK_INT_LONG,"lL");
  add_intermedia(TK_INT_DEC_BEGIN,TK_INT_UNSIGNED,"uU");
  add_intermedia(TK_INT_UNSIGNED,TK_INT_LONG,"lL");
  add_intermedia(TK_INT_LONG,TK_INT_UNSIGNED,"uU");
  add_accepted(TK_INT_LONG,TK_INT_END,CHAR_CLASS_SEPARATOR);
  add_accepted(TK_INT_UNSIGNED,TK_INT_END,CHAR_CLASS_SEPARATOR);
  add_accepted(TK_INT_DEC_BEGIN,TK_INT_END,CHAR_CLASS_SEPARATOR);

}

void init_integer_literal(void)
{
  init_dec_integer();
}

void init_punctuation(void)
{
  add_accepted(TK_INIT, TK_PUNCTUATION_END, CHAR_CLASS_PUNCTUATION);
}

void init_single_line_coment(void)
{
  /* single line coment */
  add_transfer(TK_SLASH,TK_SINGLE_LINE_COMENT_BEGIN,0,TFE_ACT_SKIP,"/");
  add_transfer(TK_SINGLE_LINE_COMENT_BEGIN,TK_SINGLE_LINE_COMENT_BEGIN, 
      TFE_FLAG_REVERSED,TFE_ACT_SKIP, "\n\r");
  add_transfer(TK_SINGLE_LINE_COMENT_BEGIN, TK_INIT,0,TFE_ACT_SKIP,"\n\r");


}

void init_multi_line_coment(void)
{
  /* multi_line coment */
  add_transfer(TK_SLASH,TK_MULTI_LINE_COMENT_BEGIN,0,TFE_ACT_SKIP,"*");
  add_transfer(TK_MULTI_LINE_COMENT_BEGIN, TK_MULTI_LINE_COMENT_BEGIN, TFE_FLAG_REVERSED,
      TFE_ACT_SKIP, "/*");
  add_transfer(TK_MULTI_LINE_COMENT_BEGIN, TK_MULTI_LINE_COMENT_END, 0, TFE_ACT_SKIP,
      "*");
  add_transfer(TK_MULTI_LINE_COMENT_END,TK_MULTI_LINE_COMENT_BEGIN,TFE_FLAG_REVERSED, TFE_ACT_SKIP,"/");
  add_transfer(TK_MULTI_LINE_COMENT_END, TK_INIT,0,TFE_ACT_SKIP,"/");

  /* bad multi_line comment */
  add_transfer(TK_MULTI_LINE_COMENT_BEGIN, TK_BAD_MULTI_LINE_COMENT, 0, TFE_ACT_SKIP,"/");
  add_transfer(TK_BAD_MULTI_LINE_COMENT, TK_NULL, 0,TFE_ACT_SKIP,"*");
  add_transfer(TK_BAD_MULTI_LINE_COMENT, TK_MULTI_LINE_COMENT_BEGIN, TFE_FLAG_REVERSED,TFE_ACT_SKIP,"*");

}

void init_operator_div(void)
{
  add_initial(TK_SLASH,"/");
  add_accepted_rev(TK_SLASH, TK_SLASH_END, "/*=");
  add_accepted(TK_SLASH, TK_SLASH_EQUAL,"=");
}

void init_table (void)
{
  void add_transfer (tokenizer_state , tokenizer_state , entry_flag, entry_action, char*);
  /* skip spaces */
  add_transfer(TK_INIT, TK_INIT, 0, TFE_ACT_SKIP, CHAR_CLASS_SPACES);

  /* identifier */
  init_identifier();

  /* punctuation */
  init_punctuation();

  /* integer -- dec, oct, hex, long or not , unsigned or not */
  init_integer_literal();

  /* slash begin token -- single line coment, multi_line coment, operator div and  operator div_assign */

  /* div and div_assign */
  /* init_operator_div(); */

  /* single line coment */
  init_single_line_coment();

  /* multi_line coment */
  init_multi_line_coment();

  /* operators */
  init_operator();

  init_char_literal();


}


int get_next_token (token *tk, char_buffer *buffer, tokenizer_state *errstate)
{
  if (peek_char(buffer) == EOF) {
    return EOF;
  }

  transfer_entry *entry;
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

    accepted = TFE_IS_ACCEPTED(entry->entry);
    state = nstate;
    switch (TFE_ACTION(entry->entry))
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


