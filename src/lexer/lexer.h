#ifndef LEXER_LEXER_H
#define LEXER_LEXER_H 1
#include "lexer/token.h"
#include "lexer/token_type.h"
#include "lexer/token_buffer.h"
#include "lexer/error.h"

typedef token_buffer  Lexer;
typedef token_type TokenType;
typedef token Token;
Lexer * CreateLexerFromFile(const char *);
Lexer * CreateLexerFromString(const char *);
Token * LexerGetToken(Lexer*);
void LexerConsume(Lexer*);
void LexerError(Lexer*);
void DestroyLexer(Lexer*);
int LexerPrintToken(Lexer *);
#endif // LEXER_LEXER_H

