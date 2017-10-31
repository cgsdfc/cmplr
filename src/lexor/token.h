/* token.h */
#ifndef TOKEN_H
#define TOKEN_H 1
#include "lexer/position.h"	// for position
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#define TOKEN_TYPE(t) (((token *)t)->type)
#define TOKEN_STRING(t) (((token *)t)->string != NULL ? (t)->string : "")
#define TOKEN_VARLEN_INIT_LEN 10
#define TERMINATE_STRING(tk) ((tk)->string[(tk)->len] = 0)
#define MARK_NO_STRING(tk) ((tk)->string = 0)
// TODO: remove man and len fields
// use char *
typedef struct token {
	int type;
	position begin;
	char *string;
	int len;
	int max;

} token;

int collect_char(token * tk, char ch);
int alloc_buffer(token * tk, position * begin);
int accept_char(token * tk, char ch);
int accept_operator(token * tk, char ch);
int accept_identifier(token * tk, char ch);
int accept_float(token * tk, char ch);
int accept_integer(token * tk, char ch);
int accept_punctuation(token * tk, char ch);
int accept_string(token * tk, char ch);
char *format_token(token *);
void destroy_token(token *);

#endif
