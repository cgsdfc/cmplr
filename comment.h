#ifndef COMMENT_H
#define COMMENT_H 1
#include "state_table.h"
#include "char_class.h"
#include "tknzr_state.h"

void init_punctuation(void);
void init_single_line_comment_cfamily(void);
void init_multi_line_comment_cfamily(void);
#endif

