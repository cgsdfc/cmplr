#ifndef LEXER_LEXER_H
#define LEXER_LEXER_H 1
#include "token_buffer.h"
token_buffer *
create_lexer(const char * file);
void destroy_lexer(token_buffer * lexer);
#endif

