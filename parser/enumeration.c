#include "enumeration.h"

static state_table *enum_table;
static token_buffer *enum_buffer;
static ast_node *enum_node;

void init_enum_type(token_buffer *buf)
{
  enum_buffer = buf;
  enum_table = alloc_table();

  assert(buf);
  assert(enum_table);

  add_initial(ENUM_BEGIN, TKT_LEFT_BRACE);
  add_initial(ENUM_NAME, TKT_IDENTIFIER);

  add_intermedia(ENUM_NAME, TKT_LEFT_BRACE);

  add_intermedia(ENUM_BEGIN,ENUM_BODY,TKT_IDENTIFIER);
  add_intermedia(ENUM_BODY,ENUM_COMMA,TKT_COMMA);

  add_accepted(ENUM_COMMA, ENUM_END, TKT_RIGHT_BRACE);
  add_accepted(ENUM_BODY,ENUM_END, TKT_RIGHT_BRACE);
  
}

ast_node *get_enum_node(void)
{
  assert (enum_node);
  return enum_node;
}

int parse_enum_type(entry_t cond, char tkt)
{
  assert (
