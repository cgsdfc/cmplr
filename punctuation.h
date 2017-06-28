#ifndef PUNCTUATION_H
#define PUNCTUATION_H 1

#include "state_table.h"
#include "token_type.h"
#include "char_class.h"
#include "tknzr_state.h"

token_type char2punctuation (char);
void init_punctuation(void);

#endif
