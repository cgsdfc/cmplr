#ifndef PARSER_TABLE_H
#define PARSER_TABLE_H 1
#include "transfer.h"
#define N_PARSER_TABLE_ROWS (_PA_NON_ACCEPTED_END - PA_INIT)
#define N_PARSER_TABLE_COLS (PA_NULL - PA_INIT)

typedef enum non_terminal_type
{
  NTT_STATEMENT,
  NTT_EXPRESSION,
  NTT_PRIMARY,
  NTT_TERM
} non_terminal_type;

typedef enum parser_state
{
  PA_INIT,
  PA_FOR_BEGIN,
  PA_WHILE_BEGIN,
  PA_DO_BEGIN,
  PA_IF_BEGIN,
  PA_GOTO_BEGIN,
  PA_EXPRESSION_BEGIN,
  PA_BLOCK_BEGIN,
  PA_STRUCT_BEGIN,
  PA_UNION_BEGIN,
  PA_ENUM_BEGIN,
  PA_FUNC_DECLARE_BEGIN,
  PA_FUNC_DEFINE_BEGIN,

  _PA_NON_ACCEPTED_END,


  PA_NULL,
} parser_state;


#endif

