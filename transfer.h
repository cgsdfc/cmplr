/* transfer.h */
#ifndef TRANSFER_H
#define TRANSFER_H 1
#include <stdbool.h>
#include<stdlib.h>
#include <assert.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "state.h"
#include "char_class.h"
#include "char_buffer.h"

/** entry: lower 2 bit is entry_flag
 * middle 15 bit is state
 * upper 15 bit is char_class
 * entry_t needs to have at least 32 bit
 */ 
typedef unsigned int transfer_entry;
typedef transfer_entry entry_t;


/* transfer_entry flags */
typedef enum entry_flag 
{
  TFE_FLAG_ACCEPTED=0x1,
  TFE_FLAG_REVERSED=0X2

} entry_flag;

/* transfer actions */
typedef enum entry_action
{
  TFE_ACT_INIT = 1,
  TFE_ACT_APPEND = 2,
  TFE_ACT_ACCEPT = 3,
  TFE_ACT_SKIP=4

} entry_action;

/* bit shift of fields */
#define TFE_STATE_SHIFT ( 4 )
#define TFE_CHAR_CLASS_SHIFT ( 8 +TFE_STATE_SHIFT  )
#define TFE_ACTION_SHIFT (8 + TFE_CHAR_CLASS_SHIFT )
#define TFE_STATE_MASK ( ~(0XFFFFF << TFE_CHAR_CLASS_SHIFT) )
#define TFE_CHAR_CLASS_MASK ( ~(0xF << TFE_ACTION_SHIFT) )


/* fields accessor */
#define TFE_IS_ACCEPTED(e) ((e) & TFE_FLAG_ACCEPTED) 

#define TFE_IS_REVERSED(e) ((e) & TFE_FLAG_REVERSED) 

#define TFE_ACTION(e) ((e) >> TFE_ACTION_SHIFT)

#define TFE_CHAR_CLASS(e) (((e)&TFE_CHAR_CLASS_MASK) >> TFE_CHAR_CLASS_SHIFT)

#define TFE_STATE(e) (((e)& TFE_STATE_MASK ) >> TFE_STATE_SHIFT)

#define TFE_MAKE_ENTRY(a,c,s,f)\
  ( (entry_t) ((a)<<TFE_ACTION_SHIFT | (c) << TFE_CHAR_CLASS_SHIFT |  (s) << TFE_STATE_SHIFT | (f)) )



/* needed by operator.c string_literal.c char_literal.c */
void add_initial(node , char_class_enum );
void add_intermedia_rev (node , node , char_class_enum  ); 
void add_intermedia (node , node , char_class_enum ); 
void add_selfloop (node , char_class_enum );;
void add_selfloop_rev (node , char_class_enum );;
void add_accepted_rev(node ,  node , char_class_enum  ); 
void add_accepted(node , node  , char_class_enum ); 


tokenizer_state do_transfer (tokenizer_state state, int ch, transfer_entry *entry);

void add_transfer(tokenizer_state from,
    tokenizer_state state, 
    entry_flag flags, entry_action act,
    char_class_enum cclass);
bool can_transfer(entry_t entry,  int character) ;
entry_t seek_entry(tokenizer_state , tokenizer_state);

#endif

