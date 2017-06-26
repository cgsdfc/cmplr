/* operator.h */
#ifndef OPERATOR_H
#define OPERATOR_H
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "state_table.h"
#include "tknzr_state.h"
#include "token_type.h"
#include "char_class.h"

#define OP_MAX_STATE 7
#define N_OPERATOR_KINDS 4
#define OP_MAX_PER_KIND 7

typedef void (*op_init)(tknzr_state *,  char_class , char_class);
/** the `reject` field is quite tricky:
 * for every kind of operators, 
 * there are more than one rejection
 * transferation in their state diagrams,
 * but since these rejections are neither
 * an op char i.e, '+', an CLASS_CHAR_EQUAL,
 * or the cat of them, i.e. "+=", SO, the
 * `reject` field just reflects the last
 * rejection.
 * other rejections are hard coded inside
 * add_xstates functions.
 */

/** represents one kind of operators */
typedef struct op_struct
{
  // a name telling its variety
  char *kind; 

 // number of operators under this kind
  int op_count;

  // number of states in the diagram of this kind
  int op_states; 

  // their states
  tknzr_state states[OP_MAX_PER_KIND][OP_MAX_STATE];

  // the op char of them
  char_class op[OP_MAX_PER_KIND]; 

  // as memtioned above, the last rejection of them
  char_class reject[OP_MAX_PER_KIND]; 

  // a function adding their diagrams into the state machine
  op_init init; 

} op_struct;


void add_1state(tknzr_state *state, char_class op, char_class rej);
void add_3states(tknzr_state *state, char_class op, char_class rej);
void add_4states(tknzr_state *state, char_class op, char_class rej);
void add_6states(tknzr_state *state, char_class op, char_class rej);


void add_arrow_operator(void);
void init_operator(void);
void check_operator(void);
token_type state2oper(tknzr_state state);

extern op_struct operators[];

#endif

