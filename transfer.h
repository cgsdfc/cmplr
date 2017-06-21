#ifndef TRANSFER_H
#define TRANSFER_H 1
#include "tokenizer.h"

typedef unsigned int entry_t;
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
#define TFE_STATE_SHIFT ( 2 )
#define TFE_ACTION_SHIFT ( 20 + TFE_STATE_SHIFT )
#define TFE_ACTION_MASK (~(0xF << TFE_ACTION_SHIFT))


/* fields accessor */
#define TFE_IS_ACCEPTED(e) ((e) & TFE_FLAG_ACCEPTED) 
#define TFE_IS_REVERSED(e) ((e) & TFE_FLAG_REVERSED) 
#define TFE_ACTION(e) ((e) >> TFE_ACTION_SHIFT)
#define TFE_STATE(e) (((e)& TFE_ACTION_MASK )>> TFE_STATE_SHIFT)
#define TFE_MAKE_ENTRY(a,s,f)\
  ( (entry_t) ((a)<<TFE_ACTION_SHIFT | (s) << TFE_STATE_SHIFT | (f)) )


typedef struct transfer_entry 
{
  entry_t entry;
  char *char_class;

} transfer_entry;

typedef transfer_entry transfer_table_t [MAX_TRANSFER_ENTRIES][MAX_TRANSFER_ENTRIES];
void add_transfer (tokenizer_state , tokenizer_state , entry_flag, entry_action, char*);
#endif

