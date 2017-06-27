#ifndef CHECKER_H 
#define CHECKER_H 1

#include "tknzr_state.h"
#include "tknzr_table.h"
#include "operator.h"
#include "token.h"
#include "char_class.h"
#include "escaped.h"
#include <limits.h>
/* this is the checker for all the */ 
/* functions, it provides checks */
/* TODO: split it */ 
void check_tknzr_table (void);
void  check_fields(void);
void check_can_transfer (tknzr_state from, 
    tknzr_state to, char_class cc);
void check_table (void);
void check_init_len_type(void);
void check_peek_line (char_buffer *buffer);
void check_char_buffer (void);
void check_operators(void);
#endif

