/* state_table.h */
#ifndef STATE_TABLE_H 
#define STATE_TABLE_H 1
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/** this file contains declaration
 * for a quite general defininess finite state machine.
 * (DFA)
 * the state_table struct, the add* initializors
 * and the st_do_transfer, together form the machine.
 * currently the impl use bit fields for information
 * about a table entry and use use match-or-not for
 * transfer conditions*/

typedef unsigned int entry_t;

/* another name of state */
typedef int node;

/* this function should return 1 if ch satisfies */
/* the condition `cond`, 0 if not, and -1 if error */
/* happened */
typedef int (*find_func)(entry_t cond, entry_t meet);

typedef struct state_table
{
  char *name;
  int nrows;
  int ncols;
  int *count;
  find_func func;
  entry_t **diagram;
  entry_t null_st;

} state_table;



/* transfer_entry flags */
typedef enum entry_flag 
{
  TFE_FLAG_ACCEPTED=0x1,
  TFE_FLAG_REVERSED=0X2

} entry_flag;

/* transfer actions */
typedef enum entry_action
{
  TFE_ACT_SKIP=0,

  TFE_ACT_INIT,
  TFE_ACT_APPEND,
  TFE_ACT_ACCEPT,

  _TFE_ACT_NULL,

} entry_action;

/** how the bit fields of entry are used:
 * lower 2 bit is entry_flag
 * middle 15 bit is state
 * upper 15 bit is char_class
 * entry_t needs to have at least 32 bit
 */ 



/* bit shift of fields */
#define TFE_STATE_SHIFT ( 4 )

#define TFE_COND_SHIFT ( 8 +TFE_STATE_SHIFT  )

#define TFE_ACTION_SHIFT (8 + TFE_COND_SHIFT )

#define TFE_LEN_TYPE_SHIFT ( 4 + TFE_ACTION_SHIFT )

#define TFE_STATE_MASK ( ~(0XFFFFF << TFE_COND_SHIFT) )

#define TFE_COND_MASK ( ~(0xFFF << TFE_ACTION_SHIFT) )

#define TFE_ACTION_MASK ( ~(0xFF << TFE_LEN_TYPE_SHIFT ) )

#define TFE_SET_LEN_TYPE(e,a) do {\
  e &= 0XFFFFFF;\
  e |= (( a ) << TFE_LEN_TYPE_SHIFT);\
} while (0)


/* fields accessor */
#define TFE_IS_ACCEPTED(e) ((e) & TFE_FLAG_ACCEPTED) 

#define TFE_IS_REVERSED(e) ((e) & TFE_FLAG_REVERSED) 

#define TFE_IS_LOOK_HEAD(e) (TFE_IS_ACCEPTED(e) && TFE_IS_REVERSED(e))

#define TFE_ACTION(e) (((e) & TFE_ACTION_MASK) >> TFE_ACTION_SHIFT)

#define TFE_COND(e) (((e)&TFE_COND_MASK) >> TFE_COND_SHIFT)

#define TFE_STATE(e) (((e)& TFE_STATE_MASK ) >> TFE_STATE_SHIFT)

#define TFE_LEN_TYPE(e) ((e) >> TFE_LEN_TYPE_SHIFT)

#define TFE_MAKE_ENTRY(a,c,s,f)\
  ( (entry_t) ((a)<<TFE_ACTION_SHIFT | (c) << TFE_COND_SHIFT |  (s) << TFE_STATE_SHIFT | (f)) )

 int
init_state_table(state_table *table,
    char *name,
    int nrows,
    int ncols, 
    entry_t null_st,
    find_func func);

state_table *alloc_table(void);

node st_do_transfer(state_table *table,
    entry_t state,
    entry_t cc,
    entry_t* entry);

void st_add_initial(state_table *table, entry_t state,entry_t cond);
void st_add_intermedia(state_table *table, entry_t from, entry_t to, entry_t cond);
void st_add_accepted(state_table *table, entry_t from, entry_t to,  entry_t cond);
void st_add_selfloop(state_table *table, entry_t state, entry_t cond);
void st_add_intermedia_rev (state_table *table, entry_t from, entry_t to, entry_t cond);
void st_add_accepted_rev (state_table *table,entry_t from, entry_t to, entry_t cond);
void st_add_selfloop_rev (state_table *table, entry_t state, entry_t cond);

/* **use** global ptr */
void add_initial(node to, entry_t cond);
void add_intermedia_rev (node from, node to, entry_t cond );
void add_intermedia(node from, node to, entry_t cond );
void add_selfloop (node from, entry_t cond);
void add_selfloop_rev (node from, entry_t cond);
void add_accepted_rev(node from,  node to, entry_t cond);
void add_accepted(node from, node to , entry_t cond);
void add_skip(node from, node to, entry_t cond);
void add_skip_rev (node from, node to, entry_t cond);
void add_skip_loop(node from, entry_t cond);
void add_skip_rev_loop (node from, entry_t cond);

#endif

