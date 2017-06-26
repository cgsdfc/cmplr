#ifndef TKNZR_TABLE_H
#define TKNZR_TABLE_H 1

// this header is for tknzr_table
// initialization use only
// for readability I splitted
// the init of diff tokens to
// diff files then includes them
// all here for the use of tknzr_table.
// in other word, this file is a private  part of
// tokenizer 
#include "string_literal.h"
#include "char_literal.h"
#include "identifier.h"
#include "operator.h"
#include "comment.h"
#include "float_literal.h"
#include "integer_literal.h"
#include "token.h"

int init_tknzr_table(void);
extern state_table *tknzr_table;

#endif

