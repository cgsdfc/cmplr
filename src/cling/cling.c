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

static struct cling_option option={
  .auto_newline=true,
  .plain_return=true,
};

static struct cling_frontend frontend;
static struct cling_backend backend;
static FILE *source_file;
static struct cling_data_flow data_flow;
static struct cling_reaching_definition definition;
static struct cling_live_variable live_variable;
static char filename_buffer[100];

static void do_definition(void)
{
  struct cling_ast_function const *ast_func;
  UTILLIB_VECTOR_FOREACH(ast_func, &backend.ast_program.funcs) {
    cling_data_flow_init(&data_flow, ast_func);
    cling_data_flow_print(&data_flow, stdout);
    cling_reaching_definition_analyze(&definition, &data_flow);
    cling_reaching_definition_print(&definition, stdout);
    reaching_definition_destroy(&definition);
    cling_data_flow_destroy(&data_flow);
  }
}

static void do_live_variable(void)
{
  struct cling_ast_function const *ast_func;
  UTILLIB_VECTOR_FOREACH(ast_func, &backend.ast_program.funcs) {
    cling_data_flow_init(&data_flow, ast_func);
    cling_data_flow_print(&data_flow, stdout);
    cling_live_variable_analyze(&live_variable, &data_flow);
    cling_live_variable_print(&live_variable, stdout);
    live_variable_destroy(&live_variable);
    cling_data_flow_destroy(&data_flow);
  }
}

int main(int argc, char *argv[])
{
  source_file=fopen(argv[1], "r");
  if (!source_file) {
    fprintf(stderr, "%s cannot be opened\n", filename_buffer);
    exit(EFOPEN);
  }
  cling_frontend_init(&frontend, &option, source_file);
  if (0 != cling_frontend_parse(&frontend)) {
    cling_frontend_destroy(&frontend);
    return ESYNTAX;
  }
  cling_backend_init(&backend, &option);
  cling_backend_codegen(&backend, &frontend);
  do_live_variable();
  cling_frontend_destroy(&frontend);
  cling_backend_destroy(&backend);
  return 0;
}
