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
#include "optimize.h"
#include "basic_block.h"
#include "rd_parser.h"
#include "scanner.h"
#include "symbol_table.h"
#include "symbols.h"
#include <stdio.h>
/*
 * Main cling driver
 */
struct frontend {
        struct scanner scanner;
        struct rd_parser parser;
        struct symbol_table symbol_table;
};

struct backend {
        struct ast_program ast_program;
        struct mips_program mips_program;
        struct mips_interp mips_interp;
};

void frontend_init(struct frontend *self,
                struct option const *option, FILE *file);
void frontend_destroy(struct frontend *self);
int frontend_tokenize(struct frontend *self, FILE *output);
int frontend_parse(struct frontend *self);
void frontend_print_error(struct frontend const *self);
void frontend_dump_ast(struct frontend const *self, FILE *output);

void backend_init(struct backend *self, struct option const *option);
void backend_destroy(struct backend *self);
void backend_dump_mips(struct backend const *self, FILE *output);
void backend_dump_ast_ir(struct backend const *self, FILE *output);
int backend_interpret(struct backend *self);
void backend_codegen(struct backend *self, struct frontend *frontend);

#endif /* CLING_CLING_CORE_H */
