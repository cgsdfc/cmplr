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

#include "cling-core.h"
#include <stdlib.h>

enum {
  ENOINPUT=1, EFOPEN, ESYNTAX
};

static struct cling_option option={
  .allow_comment=true,
};

static struct cling_frontend frontend;
static struct cling_backend backend;
FILE *source_file;

int main(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr, "no input file\n");
    exit(ENOINPUT);
  }
  source_file=fopen(argv[1], "r");
  if (!source_file) {
    fprintf(stderr, "%s cannot be opened\n", argv[1]);
    exit(EFOPEN);
  }
  cling_frontend_init(&frontend, &option, source_file);
  /* if (0 != cling_frontend_tokenize(&frontend, stdout)) */
  /*   cling_frontend_print_error(&frontend); */
  if (0 != cling_frontend_parse(&frontend)) {
    cling_frontend_print_error(&frontend);
    return ESYNTAX;
  }
  cling_backend_init(&backend);
  cling_backend_codegen(&backend, &option, &frontend);
  /* cling_backend_dump_mips(&backend, stdout); */
  cling_backend_interpret(&backend);
  cling_frontend_destroy(&frontend);
  cling_backend_destroy(&backend);
  return 0;
}
