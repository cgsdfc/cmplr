#ifndef ENUMERATION_H
#define ENUMERATION_H 1
#include "state_table.h"

// enum is default unsigned int
// it is kind of type specifier
// the tag(name) or body (the brace)
// can be omitted, but not both.
// 
// variable can be declared right
// after its left brace but before
// ';'
// very similiar to `struct` and `union`
// but, its fields are integer constants,
// having `id [ = expression of const int val],` form
// 
// 
// if its fields are too many, an unsigned int
// may be not enough, needed to enlarge it
// 
// if only see a `enum tag` but no body,
// cannot declare variable of it, so as when
// it has no name.
// 
// it cannot have empty body 
// 
// the value ordering is increased, or explicit given
// by integer literals or other enum fields
// the last field can has or has not a tailing comma

typedef enum enum_state
{
  ENUM_INIT, /* place holder */
  ENUM_BEGIN,
  ENUM_NAME,
  ENUM_BODY,
  ENUM_FIELD,
  ENUM_ASSIGN,
  ENUM_END,

  ENUM_NULL,
} enum_state;

void   init_enum_type(token_buffer *buf);
ast_node *get_enum_node(void);
bool parse_enum_type(entry_t cond, char tkt);

#endif

