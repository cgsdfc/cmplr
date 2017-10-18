#ifndef SCANNER_TOKEN_H
#define SCANNER_TOKEN_H
#include <stddef.h>
#include <stdio.h>
#include <utillib/position.h>

struct scanner_base_t;
typedef struct scanner_token {
  char *str;
  int type;
  utillib_position pos;
} scanner_token;

scanner_token *scanner_make_token(struct scanner_base_t *);
scanner_token *scanner_special_token(int);
void scanner_destroy_token(scanner_token *);
void scanner_print_token(FILE *, scanner_token *);

#endif
