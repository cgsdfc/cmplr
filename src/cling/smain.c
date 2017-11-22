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
#include <utillib/print.h>
#include <utillib/scanner.h>
#include "symbols.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  if (argc != 2) {
    utillib_error_printf("ERROR: no input file\n");
    utillib_error_printf("USAGE: cling_scanner input\n");
    exit(1);
  }
  FILE *file=fopen(argv[1], "r");
  if (!file) {
    utillib_error_printf("ERROR: %s: no such file or directory\n",
        argv[1]);
    exit(2);
  }
  struct utillib_token_scanner cling;
  size_t code;
  cling_scanner_init(&cling, file);
  while ((code=utillib_token_scanner_lookahead(&cling))!=UT_SYM_EOF) {
    if (code == UT_SYM_ERR) {
      exit(EXIT_FAILURE);
    }
    void const * semantic=utillib_token_scanner_semantic(&cling);
    char const * name=cling_symbols[code].name;
    printf("code=%lu, name=%s, ", code, name);
    switch (code) {
      case SYM_UINT:
        printf("value=%lu\n", (size_t) semantic);
        break;
      case SYM_IDEN:
        printf("value=%s\n", (char const*) semantic);
        free(semantic);
        break;
      case SYM_STRING:
        printf("value=\"%s\"\n", (char const*) semantic);
        free(semantic);
        break;
      case SYM_CHAR:
        printf("value='%s'\n", (char) semantic);
        break;
      default:
        puts("null");
        break;
    }
    utillib_token_scanner_shiftaway(&cling);
  }
  utillib_token_scanner_destroy(&cling);
  exit(EXIT_SUCCESS);
}


