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
#include <stdio.h>
#include <string.h>

enum {
  ENOINPUT=1, EFOPEN, ESYNTAX
};

static struct cling_option option;
static struct cling_frontend frontend;
static struct cling_backend backend;
static FILE *source_file;
static FILE *mips_file;
static char filename_buffer[100];

static void die_of_fopen(char const *filename){
  fprintf(stderr, "%s cannot be opened\n", filename_buffer);
  exit(EFOPEN);
}


int main(void)
{
  char optch;
  printf("please input a file\n");
  fgets(filename_buffer, sizeof filename_buffer, stdin);
  filename_buffer[strlen(filename_buffer)-1]='\0';
  source_file=fopen(filename_buffer, "r");
  if (!source_file) {
    die_of_fopen(filename_buffer);
  }
  printf("please tell me whether to do optimization(Y/n)");
  optch=getchar();
  switch(optch) {
    case 'Y': case 'y':
      option.optimize_lcse=true;
      break;
    case 'N': case 'n':
      option.optimize_lcse=false;
      break;
    default:
      puts("say Y or n, please");
      exit(1);
  }
  
  cling_frontend_init(&frontend, &option, source_file);
  if (0 != cling_frontend_parse(&frontend)) {
    cling_frontend_destroy(&frontend);
    return ESYNTAX;
  }
  cling_backend_init(&backend, &option);
  cling_backend_codegen(&backend, &frontend);
  strncat(filename_buffer, ".s", sizeof filename_buffer);
  mips_file=fopen(filename_buffer, "w");
  if (!mips_file) {
    die_of_fopen(filename_buffer);
  }
  cling_backend_dump_mips(&backend, mips_file);
  fclose(mips_file);
  printf("Assembly has been written to %s\n", filename_buffer);
  cling_frontend_destroy(&frontend);
  cling_backend_destroy(&backend);
  return 0;
}
