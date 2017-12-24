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

/*
 * file must be opened for reading.
 */
void cling_frontend_init(struct cling_frontend *self,
                         struct cling_option const *option, FILE *file) {
  cling_symbol_table_init(&self->symbol_table);
  cling_scanner_init(&self->scanner, option, &self->parser, file);
  cling_rd_parser_init(&self->parser, option, &self->symbol_table);
}

int cling_frontend_tokenize(struct cling_frontend *self, FILE *output)
{
  size_t code;
  /*
   * All semantic is string.
   */
  char const *semantic;
  char const *name;
  for (; (code = cling_scanner_lookahead(&self->scanner)) != UT_SYM_EOF;
       cling_scanner_shiftaway(&self->scanner)) {
    if (code == UT_SYM_ERR) {
      return 1;
    }
    semantic = cling_scanner_semantic(&self->scanner);
    name = cling_symbol_kind_tostring(code);
    fprintf(output, "'%s'\t", name);
    switch (code) {
    case SYM_INTEGER:
    case SYM_IDEN:
      fprintf(output, "%s\n", semantic);
      break;
    case SYM_STRING:
      fprintf(output, "\"%s\"\n", semantic);
      break;
    case SYM_CHAR:
      fprintf(output, "'%s'\n", semantic);
      break;
    default:
      fputs("\n", output);
      break;
    }
  }
  return 0;
}

void cling_frontend_destroy(struct cling_frontend *self)
{
  cling_rd_parser_destroy(&self->parser);
  cling_symbol_table_destroy(&self->symbol_table);
  cling_scanner_destroy(&self->scanner);
}

inline int cling_frontend_parse(struct cling_frontend *self) {
  return cling_rd_parser_parse(&self->parser, &self->scanner);
}

inline void cling_frontend_dump_ast(struct cling_frontend const *self, FILE *output) {
  utillib_json_pretty_fprint(self->parser.root, output);
}

void cling_backend_init(struct cling_backend *self)
{
  cling_ast_program_init(&self->ast_program);
  cling_mips_program_init(&self->mips_program);
  cling_mips_interp_init(&self->mips_interp);
}


void cling_backend_destroy(struct cling_backend *self)
{
  cling_ast_program_destroy(&self->ast_program);
  cling_mips_program_destroy(&self->mips_program);
  cling_mips_interp_destroy(&self->mips_interp);
}


void cling_backend_codegen(struct cling_backend *self, struct cling_frontend *frontend)
{
  cling_ast_ir_emit_program(frontend->parser.root, &frontend->symbol_table, &self->ast_program);
  cling_mips_program_emit(&self->mips_program, &self->ast_program);
}

void cling_backend_dump_ast_ir(struct cling_backend const *self, FILE *output) 
{
  cling_ast_program_print(&self->ast_program, output);
}

void cling_backend_dump_mips(struct cling_backend const *self, FILE *output)
{
  cling_mips_program_print(&self->mips_program, output);
}

