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
#include <stdio.h>
#include <stdlib.h>
#include <utillib/print.h>
#include <utillib/scanner.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    utillib_error_printf("ERROR: no input file\n"
                         "USAGE: cling_scanner input [output]\n"
                         "       Scans the `input' file and writes the tokens\n"
                         "       to `output'. If no `output' file, writes to\n"
                         "       stdout.\n");
    exit(1);
  }
  FILE *output;
  FILE *file = fopen(argv[1], "r");
  if (!file) {
    utillib_error_printf("ERROR: %s: no such file or directory\n", argv[1]);
    exit(2);
  }
  if (argc == 3) {
    output = fopen(argv[2], "w");
    if (!output) {
      utillib_error_printf("ERROR: %s: cannot open for writing\n", argv[3]);
      exit(3);
    }
  } else {
    output = stdout;
  }

  struct utillib_token_scanner cling;
  size_t code;
  bool success = true;
  cling_scanner_init(&cling, file);
  while ((code = utillib_token_scanner_lookahead(&cling)) != UT_SYM_EOF) {
    if (code == UT_SYM_ERR) {
      success = false;
      break;
    }
    void const *semantic = utillib_token_scanner_semantic(&cling);
    char const *name =cling_symbol_kind_tostring(code);
    fprintf(output, "%s\t", name);
    switch (code) {
    case SYM_UINT:
      fprintf(output, "%lu", (size_t)semantic);
      break;
    case SYM_IDEN:
      fprintf(output, "%s", (char const *)semantic);
      break;
    case SYM_STRING:
      fprintf(output, "\"%s\"", (char const *)semantic);
      break;
    case SYM_CHAR:
      fprintf(output, "'%c'", (char)semantic);
      break;
    default:
      break;
    }
    fputs("\n", output);
    utillib_token_scanner_shiftaway(&cling);
  }
  utillib_token_scanner_destroy(&cling);
  return !success;
}
