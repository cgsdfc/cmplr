/* integer_literal.h */
#ifndef INTEGER_LITERAL_H
#define INTEGER_LITERAL_H 1
#include "state_table.h"
#include "tknzr_state.h"
#include "char_class.h"

void init_integer_literal(void);

// TODO enum has signed int as default 
// type
typedef enum integer_type
{
  
  INT_CHAR,
  INT_UNSIGNED_CHAR,
  INT_SIGNED_CHAR,

  INT_SHORT,
  INT_UNSIGNED_SHORT,
  INT_SIGNED_SHORT,

  INT_INT,
  INT_UNSIGNED_INT,
  INT_SIGNED_INT,

  INT_LONG,
  INT_SIGNED_LONG,
  INT_UNSIGNED_LONG,
  
  INT_LONG_LONG,
  INT_UNSIGNED_LONG_LONG,
  INT_SIGNED_LONG_LONG,
} integer_type;

#endif

