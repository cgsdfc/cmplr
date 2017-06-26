#ifndef PARSER_ERROR_H
#define PARSER_ERROR_H 1

typedef enum parser_error
{
  PERR_BAD_ESCAPED,
} parser_error;

void parser_error_set(parser_error err);

#endif

