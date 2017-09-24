#ifndef RECURSIVE_PARSER_FUNCTION_H
#define RECURSIVE_PARSER_FUNCTION_H
#include <stdbool.h>

struct parser_info;
typedef bool (*pf_function) (struct parser_info *);
typedef struct parser_function
{
  pf_function apply;
} parser_function;

#endif // RECURSIVE_PARSER_FUNCTION_H
