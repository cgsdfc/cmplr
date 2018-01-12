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
void frontend_init(struct frontend *self,
                struct option const *option, FILE *file) {
        symbol_table_init(&self->symbol_table);
        scanner_init(&self->scanner, option, &self->parser, file);
        rd_parser_init(&self->parser, option, &self->symbol_table);
}

int frontend_tokenize(struct frontend *self, FILE *output)
{
        size_t code;
        /*
         * All semantic is string.
         */
        char const *semantic;
        char const *name;
        for (; (code = scanner_lookahead(&self->scanner)) != UT_SYM_EOF;
                        scanner_shiftaway(&self->scanner)) {
                if (code == UT_SYM_ERR) {
                        return 1;
                }
                semantic = scanner_semantic(&self->scanner);
                name = symbol_kind_tostring(code);
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

void frontend_destroy(struct frontend *self)
{
        rd_parser_destroy(&self->parser);
        symbol_table_destroy(&self->symbol_table);
        scanner_destroy(&self->scanner);
}

int frontend_parse(struct frontend *self) {
        if (0 != rd_parser_parse(&self->parser, &self->scanner)) {
                rd_parser_print_error(&self->parser);
                return 1;
        }
        return 0;
}

inline void frontend_dump_ast(struct frontend const *self, FILE *output) {
        utillib_json_pretty_fprint(self->parser.root, output);
}

void backend_init(struct backend *self, struct option const *option)
{
        ast_program_init(&self->ast_program, option);
        mips_program_init(&self->mips_program, option);
        mips_interp_init(&self->mips_interp, option);
}


void backend_destroy(struct backend *self)
{
        ast_program_destroy(&self->ast_program);
        mips_program_destroy(&self->mips_program);
        mips_interp_destroy(&self->mips_interp);
}

static void backend_optimize(struct backend *self);

void backend_codegen(struct backend *self,
                struct frontend *frontend) {
        ast_ir_emit_program(&self->ast_program,
                        frontend->parser.root,
                        &frontend->symbol_table);
        backend_optimize(self);
        mips_program_emit(&self->mips_program, &self->ast_program);
}

inline void backend_dump_ast_ir(struct backend const *self, FILE *output) 
{
        ast_program_print(&self->ast_program, output);
}

inline void backend_dump_mips(struct backend const *self, FILE *output)
{
        mips_program_print(&self->mips_program, output);
}

int backend_interpret(struct backend *self)
{
        mips_interp_load(&self->mips_interp, &self->mips_program);
        if (0 != mips_interp_exec(&self->mips_interp)) {
                mips_interp_print_error(&self->mips_interp);
                return 1;
        }
        return 0;
}

static void backend_optimize(struct backend *self)
{
        struct optimizer optimizer;
        struct ast_function *ast_func;
        UTILLIB_VECTOR_FOREACH(ast_func, &self->ast_program.funcs) {
                optimizer_init(&optimizer, self->ast_program.option, ast_func);
                optimizer_optimize(&optimizer, ast_func);
                optimizer_destroy(&optimizer);
        }
}
