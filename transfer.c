/* transfer.c */
#include "transfer.h"

static transfer_table_t table;
static int entry_counters[MAX_TRANSFER_ENTRIES];
static void add_transfer(tokenizer_state from,
    tokenizer_state state, 
    entry_flag flags, entry_action act,
    char_class_enum cclass);


void add_initial(node to, char_class_enum char_class)
{
  add_transfer (TK_INIT, to, 0,TFE_ACT_INIT,char_class);
}


void add_intermedia_rev (node from, node to, char_class_enum char_class ) 
{
  add_transfer(from, to, TFE_FLAG_REVERSED, TFE_ACT_APPEND, char_class);
}

void add_intermedia (node from, node to, char_class_enum char_class) 
{
  add_transfer(from, to, 0, TFE_ACT_APPEND, char_class);
}

void add_selfloop (node from, char_class_enum char_class)
{
  add_intermedia(from,from,char_class);
}

void add_selfloop_rev (node from, char_class_enum char_class)
{
  add_intermedia_rev(from,from,char_class);
}

void add_accepted_rev(node from,  node to, char_class_enum  char_class) 
{
  add_transfer(from, to, TFE_FLAG_REVERSED|TFE_FLAG_ACCEPTED, TFE_ACT_ACCEPT, char_class);
}
void add_accepted(node from, node to , char_class_enum char_class) 
{
  add_transfer(from, to, TFE_FLAG_ACCEPTED, TFE_ACT_ACCEPT, char_class);
}


void check_tokenizer (void) 
{
  void check_fields(void);
  void check_table(void);

  check_fields();
  check_table();

  puts ("check_table passed");

}

transfer_entry seek_entry (int state_from, int state_to)
{
  assert (state_from >=0 && state_from < MAX_TRANSFER_ENTRIES);

  transfer_entry entry;
  for (int i=0;i<entry_counters[state_from];++i)
  {
    entry = table[state_from][i];
    if (TFE_STATE(entry) == state_to)
      return entry;
  }
  return 0;
}


void check_fields(void)
{
#ifndef NDEBUG
  entry_t entry;
  entry_t flag, action, state, char_class;

  /* check all the action fields */
  for (int i=TFE_ACT_INIT;i<TFE_ACT_SKIP+1;++i)
  {
    entry = TFE_MAKE_ENTRY(i, 0, 0,0); 
    action = TFE_ACTION(entry);
    assert (action == i );
  }
  puts ("check action passed");

  /* check all the char_class fields */
  for (int i=CHAR_CLASS_EMPTY;i<_CHAR_CLASS_NULL;++i)
  {
    entry = TFE_MAKE_ENTRY(0, i,0,0);
    char_class=TFE_CHAR_CLASS (entry);
    assert (char_class== i);
  }
  puts ("check char_class passed");


  /* check all the state fields */
  for (int i=TK_INIT;i<TK_NULL+1;++i)
  {
    entry = TFE_MAKE_ENTRY(0, 0,i,0);
    state=TFE_STATE(entry);
    assert (state == i);
  }

  puts ("check state passed");

  /* use a big loop to test all possible */
  for (int i=TFE_ACT_INIT;i<TFE_ACT_SKIP+1;++i)
  {
    for (int j=CHAR_CLASS_EMPTY;j<_CHAR_CLASS_NULL;++j)
    {
      for (int k=TK_INIT;k<TK_NULL+1;++k)
      {
        entry=TFE_MAKE_ENTRY(i,j,k,0);
        assert (j==TFE_CHAR_CLASS(entry));
        assert (i==TFE_ACTION(entry));
        assert (k==TFE_STATE(entry));

      }
    }
  }

 
  puts ("check_fields passed");
#endif

}

static
char *get_char_class(char_class_enum char_class)
{
  char *ch=char_class2string[char_class];
  assert (ch != NULL);
  return ch;
}

bool can_transfer(entry_t entry,  int character) 
{
  char_class_enum cclass = TFE_CHAR_CLASS(entry);
  char *char_class=get_char_class(cclass);

  bool is_in_class = strchr (char_class, character);
  return (is_in_class && ! TFE_IS_REVERSED(entry)) || 
    (!is_in_class && TFE_IS_REVERSED(entry));

}

static
void add_transfer(tokenizer_state from,
    tokenizer_state state, 
    entry_flag flags, entry_action act,
    char_class_enum cclass)
{
  int len=entry_counters[from];

  /* the order is : */
  /* (action | char_class | state | flags) */
  transfer_entry entry = TFE_MAKE_ENTRY(act,cclass,state,flags);
  table[from][len]=entry;
  entry_counters[from]++;

}

tokenizer_state do_transfer(tokenizer_state state,
    int ch, transfer_entry *entry)
{
  assert (state >=0 && state < MAX_TRANSFER_ENTRIES);
  tokenizer_state nstate;
  char_class_enum char_class;

  for (int i=0;i<entry_counters[state];++i)
  {
    *entry = table[state][i];
    nstate = TFE_STATE(table[state][i]);
    if (can_transfer (table[state][i], ch)) 
    {
      return nstate;
    }
  }
  return TK_NULL;

}

/** for each char in `char_class`, check `from` can transfer to `to` */
void check_can_transfer (tokenizer_state from, 
    tokenizer_state to, char_class_enum cc)
{
  char *char_class=char_class2string[cc];
  for (int i=0;i<strlen(char_class);++i) 
  {
    int letter = char_class[i];
    assert (can_transfer(seek_entry(from, to), letter));

  }

}

void clear_table(void)
{
  memset(table, 0, sizeof table);
  memset(entry_counters, 0, sizeof entry_counters);
}


void check_table (void)
{
  // TODO: check more throughly
#ifndef NDEBUG
  void init_table(void);
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
  clear_table();
#endif

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
  add_intermedia(TK_INT_DEC_BEGIN,TK_INT_LONG, CHAR_CLASS_LONG_SUFIX);
  add_intermedia(TK_INT_DEC_BEGIN,TK_INT_UNSIGNED, CHAR_CLASS_UNSIGNED_SUFIX);
  add_intermedia(TK_INT_UNSIGNED,TK_INT_LONG,CHAR_CLASS_LONG_SUFIX);
  add_intermedia(TK_INT_LONG,TK_INT_UNSIGNED, CHAR_CLASS_UNSIGNED_SUFIX);
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
  add_transfer(TK_SLASH,TK_SINGLE_LINE_COMENT_BEGIN,0,TFE_ACT_SKIP,CAHR_CLASS_SLASH);
  add_transfer(TK_SINGLE_LINE_COMENT_BEGIN,TK_SINGLE_LINE_COMENT_BEGIN, 
      TFE_FLAG_REVERSED,TFE_ACT_SKIP, CHAR_CLASS_NEWLINE);
  add_transfer(TK_SINGLE_LINE_COMENT_BEGIN, TK_INIT,0,TFE_ACT_SKIP,CHAR_CLASS_NEWLINE);


}

// TODO: change it , donot detect bad multi_line 
// just let the parser get fucked
void init_multi_line_coment(void)
{
  /* multi_line coment */
  add_transfer(TK_SLASH,TK_MULTI_LINE_COMENT_BEGIN,0,TFE_ACT_SKIP,CHAR_CLASS_STAR);
  add_transfer(TK_MULTI_LINE_COMENT_BEGIN, TK_MULTI_LINE_COMENT_BEGIN, TFE_FLAG_REVERSED,
      TFE_ACT_SKIP, CHAR_CLASS_STAR);
  add_transfer(TK_MULTI_LINE_COMENT_BEGIN, TK_MULTI_LINE_COMENT_END, 0, TFE_ACT_SKIP,
      CHAR_CLASS_STAR);
  add_transfer(TK_MULTI_LINE_COMENT_END,TK_MULTI_LINE_COMENT_BEGIN,TFE_FLAG_REVERSED, TFE_ACT_SKIP,CAHR_CLASS_SLASH);
  add_transfer(TK_MULTI_LINE_COMENT_END, TK_INIT,0,TFE_ACT_SKIP,CAHR_CLASS_SLASH);

}


void init_table (void)
{
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

  /* character */ 
  init_char_literal();

  /* string */
  init_string_literal();

}

