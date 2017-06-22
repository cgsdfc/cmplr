#ifndef ESCAPED_H
#define ESCAPED_H 1
#include "transfer.h"

typedef enum escape_states
{

  /* the '\\' is read */
  ES_BEGIN=0,  

  /* 'x' is read */
  ES_HEX_BEGIN,

  /* 2 hex digs are read */
  ES_HEX_END, 

  /* one oct dig is read */
  ES_OCT_BEGIN,

  /* 2 oct dig is read */
  ES_OCT_END, 
  
  /* '0' is read */
  ES_ZERO,

  /* 1 letter or 2 hex digs or 3 oct digs are read */
  ES_END,
 
  /* this state is meant for finishing escaped but */
  /*   the quote is yet to met */
  ES_PART
} escape_states;

void add_escape_hex_literal(node *state, char_class_enum quote);
void add_escape_oct_literal(node *state, char_class_enum quote);
void add_escape_single(node *state, char_class_enum quote);
void add_escape_all (node *state, char_class_enum quote);

#endif

