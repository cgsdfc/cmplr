#ifndef RECURSIVE_PARSER_INFO_H
#define RECURSIVE_PARSER_INFO_H

struct node_base;
struct Lexer;
typedef enum parser_error
{
  PARSER_ERROR_NULL,
} parser_error;

typedef struct parser_info
{
  parser_error error;
  struct Lexer *lexer;
  struct node_base *node;
} parser_info;

#endif // RECURSIVE_PARSER_INFO_H
