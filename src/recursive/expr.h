#ifndef RECUSIVE_EXPR_H
#define RECUSIVE_EXPR_H
#include <stdbool.h>
#include "lexer/lexer.h"

bool expr_is_primary (Lexer *);
bool expr_is_expression (Lexer *);
#endif // RECUSIVE_EXPR_H
