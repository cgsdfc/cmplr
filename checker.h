#ifndef CHECKER_H 
#define CHECKER_H 1
#include "char_class.h"
#include "state.h"
#include "char_buffer.h"

void check_counters(void);
void check_tknzr_table (void);
void  check_fields(void);
void check_can_transfer (tokenizer_state from, 
    tokenizer_state to, char_class_enum cc);
void check_table (void);
void check_init_len_type(void);
void check_peek_line (char_buffer *buffer);
void check_char_buffer (void);
void check_operators(void);
#endif

