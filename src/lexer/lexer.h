#ifndef LEXER_LEXER_H
#define LEXER_LEXER_H 1
#include "lexer/token.h"
#include "lexer/token_type.h"
#include "lexer/list.h"
#include "lexer/error.h"

typedef token_list Lexer;
typedef token_type TokenType;
typedef token Token;
Lexer *CreateLexerFromFile (char *);
Lexer *CreateLexerFromString (char *);
Token *LexerGetToken (Lexer *);
void LexerConsume (Lexer *);
void LexerError (Lexer *);
void DestroyLexer (Lexer *);
int LexerPrintToken (Lexer *);
const char *LexerTerminalString (Token *);
#endif // LEXER_LEXER_H
