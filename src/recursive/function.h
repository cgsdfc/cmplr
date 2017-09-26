#ifndef RECURSIVE_PARSER_FUNCTION_H
#define RECURSIVE_PARSER_FUNCTION_H
#include <stdbool.h>

struct parser_info;
typedef bool (*pf_parser_function) (struct parser_info *);
typedef struct parser_function
{
  pf_parser_function apply;
} parser_function;

typedef struct pred_function
{
  pf_parser_function apply;
} pred_function;

#endif // RECURSIVE_PARSER_FUNCTION_H
