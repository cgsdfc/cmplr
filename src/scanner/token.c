/* 
   Cmplr Library
   Copyright (C) 2017-2018 Cong Feng <cgsdfc@126.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
   02110-1301 USA

*/
#define _BSD_SOURCE
#include "token.h"
#include "scanner.h"
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
static scanner_token *make_token_impl(char *str, int type,
                                      utillib_position *pos) {
  scanner_token *token = malloc(sizeof *token);
  if (token) {
    token->str = str;
    token->type = type;
    memcpy(&(token->pos), pos, sizeof *pos);
    return token;
  }
  utillib_die(strerror(errno));
}

scanner_token *scanner_special_token(int type) {
  static scanner_token TOKEN_EOF = {.type = SCANNER_EOF};
  static scanner_token TOKEN_ERR = {.type = SCANNER_ERROR};
  switch (type) {
  case SCANNER_EOF:
    return &TOKEN_EOF;
  case SCANNER_ERROR:
    return &TOKEN_ERR;
  default:
    assert(false);
  }
}

scanner_token *scanner_make_token(scanner_base_t *self) {
  char const *str = self->sc_text;
  return make_token_impl(strdup(self->sc_text), self->sc_val,
                         scanner_input_buf_current_pos(self->sc_buf));
}

void scanner_destroy_token(scanner_token *token) {
  free((void *)token->str);
  free(token);
}

void scanner_print_token(FILE *file, scanner_token *token) {
  fprintf(file, "%d %s\n", token->type, token->str);
}
