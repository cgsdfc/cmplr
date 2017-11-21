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
#include "scanner.h"
#include "symbols.h"
#define CLING_KW_SIZE  14

const struct utillib_keyword_pair cling_keywords[]={
  {"case", SYM_KW_CASE},
  {"char", SYM_KW_CHAR},
  {"const", SYM_KW_CONST},
  {"default", SYM_KW_DEFAULT},
  {"else", SYM_KW_ELSE},
  {"for", SYM_KW_FOR},
  {"if", SYM_KW_IF},
  {"int", SYM_KW_INT},
  {"main", SYM_KW_MAIN},
  {"printf", SYM_KW_PRINTF},
  {"return", SYM_KW_RETURN},
  {"scanf", SYM_KW_SCANF},
  {"switch", SYW_KW_SWITCH},
  {"void", SYM_KW_VOID},
};

static const struct utillib_token_scanner_callback cling_scanner_callback={
  .read_handler=cling_scanner_read_handler,
  .error_handler=cling_scanner_error_handler,
  .semantic_handler=cling_scanner_semantic_handler,
};

void cling_scanner_init(struct utillib_token_scanner *self, FILE *file)
{
  utillib_token_scanner_init(self, file, &cling_scanner_callback);
}

int cling_scanner_read_handler(struct utillib_char_scanner *chars, 
    struct utillib_string *buffer);
int cling_scanner_error_handler(struct utillib_char_scanner *chars, 
    struct utillib_token_scanner_error const *error);
void const* cling_scanner_semantic_handler(size_t value, char const *str);

