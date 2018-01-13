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
#define FN_BUFSIZ 100
#define PROMPT_USER

static char filename_buffer[FN_BUFSIZ];
static void die_of_fopen(char const *filename){
        fprintf(stderr, "%s cannot be opened\n", filename);
        exit(1);
}

static FILE * open_source_prompt(struct option *option)
{
        char optch;
        FILE *source_file;

        printf("please input a file");
        fgets(filename_buffer, FN_BUFSIZ, stdin);
        filename_buffer[strlen(filename_buffer)-1]='\0';
        source_file=fopen(filename_buffer, "r");
        if (!source_file) {
                die_of_fopen(filename_buffer);
        }
        return source_file;
}

static FILE *open_source_argv(char const *path)
{
        FILE *source_file=fopen(path, "r");
        if (!source_file)
                die_of_fopen(path);
        strncpy(filename_buffer, path, FN_BUFSIZ);
        return source_file;
}

int main(int argc, char **argv)
{
        struct option option={0};
        struct frontend frontend={0};
        struct backend backend={0};
        FILE *source_file=NULL;
        FILE *mips_file=NULL;

#ifdef PROMPT_USER
        source_file=open_source_prompt(&option);
#else
        source_file=open_source_argv(argv[1]);
#endif
        frontend_init(&frontend, &option, source_file);
        backend_init(&backend, &option);
        if (0 != frontend_parse(&frontend)) {
                frontend_destroy(&frontend);
                return 1;
        }
        backend_codegen(&backend, &frontend);
        strncat(filename_buffer, ".s", sizeof filename_buffer);
        mips_file=fopen(filename_buffer, "w");
        if (!mips_file) {
                die_of_fopen(filename_buffer);
        }
        backend_dump_mips(&backend, mips_file);
        fclose(mips_file);
        printf("Assembly has been written to %s\n", filename_buffer);
        frontend_destroy(&frontend);
        backend_destroy(&backend);
        return 0;
}
