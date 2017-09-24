#ifndef RECURSIVE_PARSER_INFO_H
#define RECURSIVE_PARSER_INFO_H

struct ast_node;
struct lexer;
typedef unsigned parser_result;
typedef struct parser_info
{
  parser_result pi_parser_result;
  struct lexer * pi_lexer;
  struct ast_node * pi_ast_node;
} parser_info;

#endif // RECURSIVE_PARSER_INFO_H
