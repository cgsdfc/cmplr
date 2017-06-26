#ifndef CHAR_LITERAL_H 
#define CHAR_LITERAL_H 1
#include "state_table.h"
#include "char_class.h"
#include "tknzr_state.h"
void init_char_literal(void);

// TODO char_literal is of type `int`
// TODO: hex digits are unlimited, not at most 2
// but if the range exceeds the char set limit
// will warning
// but oct digits are at most 3

#endif

