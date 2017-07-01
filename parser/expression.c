#include "expression.h"
#include "parser_table.h"

#define N_PARSER_TABLE_ROWS (_PA_NON_ACCEPTED_END - PA_INIT)
#define N_PARSER_TABLE_COLS (PA_NULL - PA_INIT)
static state_table *parser_table;

void init_parser_table(void)
{
  parser_table=alloc_table();
  if (init_state_table(
      parser_table,
      "parser's table",
      N_PARSER_TABLE_ROWS,
      N_PARSER_TABLE_COLS,
      PA_INIT,
      token_is_in_class)<0)
  {
    puts("init_parser_table failed");
    exit(4);
  }

  // init all the syntax rules here

}

      
