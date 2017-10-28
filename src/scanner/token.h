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
