#ifndef LEXER_LEXER_H
#define LEXER_LEXER_H 1
#include "lexer/error.h"
#include "lexer/list.h"
#include "lexer/token.h"
#include "lexer/token_type.h"

typedef token_list Lexer;
typedef token_type TokenType;
typedef token Token;
Lexer *CreateLexerFromFile(char *);
Lexer *CreateLexerFromString(char *);
Token *LexerGetToken(Lexer *);
void LexerConsume(Lexer *);
void LexerError(Lexer *);
void DestroyLexer(Lexer *);
int LexerPrintToken(Lexer *);
const char *LexerTerminalString(Token *);
const char **LexerTerminalTab(void);
#endif				// LEXER_LEXER_H
