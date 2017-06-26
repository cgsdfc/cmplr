/* float_literal.h */
#ifndef FLOAT_LITERAL_h
#define FLOAT_LITERAL_h 1
#include "base_type.h"
#include "state_table.h"
#include "char_class.h"
#include "tknzr_state.h"


void init_float_literal(void);

typedef enum float_type
{
  FLT_FLOAT=FLOAT_TYPE_BEGIN,
  FLT_DOUBLE,
  FLT_LONG_DOUBLE,
} float_type;

#endif

