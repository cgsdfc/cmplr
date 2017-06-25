#ifndef TKNZ_TABLE_H
#define TKNZ_TABLE_H 1
#include "transfer.h"
#include "string_literal.h"
#include "char_literal.h"
#include "identifier.h"
#include "operator.h"
#include "comment.h"
#include "float_literal.h"
#include "integer_literal.h"


void init_tknzr_table(void);
node do_transfer(node,char,entry_t*);
state_table *alloc_tokenizer_table(void);
state_table *get_tokenizer_table(void);



#endif

