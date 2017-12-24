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

#ifndef CLING_CLING_CORE_H
#define CLING_CLING_CORE_H
#include "ast_ir.h"
#include "mips.h"
#include "mips_interp.h"
#include "option.h"
#include "rd_parser.h"
#include "scanner.h"
#include "symbol_table.h"
#include "symbols.h"
#include <stdio.h>
/*
 * Main cling driver
 */

struct cling_frontend {
  struct cling_scanner scanner;
  struct cling_rd_parser parser;
  struct cling_symbol_table symbol_table;
};

struct cling_backend {
  struct cling_ast_program ast_program;
  struct cling_mips_program mips_program;
  struct cling_mips_interp mips_interp;
};

void cling_frontend_init(struct cling_frontend *self,
                         struct cling_option const *option, FILE *file);
void cling_frontend_destroy(struct cling_frontend *self);
int cling_frontend_tokenize(struct cling_frontend *self, FILE *output);
int cling_frontend_parse(struct cling_frontend *self);
void cling_frontend_dump_ast(struct cling_frontend const *self, FILE *output);

void cling_backend_init(struct cling_backend *self);
void cling_backend_destroy(struct cling_backend *self);
void cling_backend_dump_mips(struct cling_backend const *self, FILE *output);
void cling_backend_dump_ast_ir(struct cling_backend const *self, FILE *output);
void cling_backend_codegen(struct cling_backend *self, struct cling_frontend *frontend);


#endif /* CLING_CLING_CORE_H */
