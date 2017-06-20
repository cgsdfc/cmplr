#ifndef TRANSFER_H
#define TRANSFER_H 1
#include "tokenizer.h"

#define TFE_STATE(e) ((e)->state)
#define TFE_IS_ACCEPTED(e) ((e)->is_accepted)
#define TFE_IS_REVERSED(e) ((e)->is_reversed)


typedef void (* tkz_action) (struct token *, int, char_buffer *);

typedef struct transfer_entry 
{
  enum tokenizer_state state;
  bool is_accepted;
  bool is_reversed;
  char *char_class;
  tkz_action tkz_act;

} transfer_entry;
void add_transfer(int this_state, int state, bool is_accepted, bool is_reversed, char *char_class, tkz_action act);


#endif

