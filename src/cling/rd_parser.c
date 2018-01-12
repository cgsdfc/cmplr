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

#include "rd_parser.h"
#include "ast_build.h"
#include "ast_check.h"
#include "error.h"
#include "misc.h"
#include "opg_parser.h"
#include "scanner.h"
#include "symbol_table.h"
#include "symbols.h"
#include "option.h"
#include <utillib/hashmap.h>

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/**
 * File cling/rd_parser.c
 * All the recursive decent subroutines
 * go here.
 */

#define rd_parser_skip_init(self, ...)                                         \
        do {                                                                         \
                rd_parser_skip_target_init(self, __VA_ARGS__, -1);                         \
        } while (0)

static bool rd_parser_is_stmt_lookahead(size_t code);

static void rd_parser_fatal(struct rd_parser *self) {
        longjmp(self->fatal_saver, 1);
}

static void rd_parser_skip_target_init(struct rd_parser *self, ...) {
        va_list ap;
        va_start(ap, self);
        for (int i = 0, tar = va_arg(ap, int); tar != -1 && i < RD_PARSER_SKIP_MAX;
                        tar = va_arg(ap, int), ++i) {
                self->tars[i] = tar;
        }
        self->tars[RD_PARSER_SKIP_MAX - 1] = -1;
        va_end(ap);
}

/*
 * When the skip targets are less than 4, use this
 * function. Otherwise, use special skipto_XXX functions.
 */
static size_t rd_parser_skipto(struct rd_parser *self,
                struct scanner *scanner) {
        size_t code;
        while (true) {
                code = scanner_lookahead(scanner);
                if (code == UT_SYM_EOF || code == UT_SYM_ERR)
                        rd_parser_fatal(self);
                for (int const *pi = self->tars; *pi != -1; ++pi)
                        if (*pi == code)
                                return code;
                scanner_shiftaway(scanner);
        }
}

/*
 * Skips to the lookaheads of a statement.
 */

static int rd_parser_skipto_stmt(struct rd_parser *self,
                struct scanner *scanner) {
        size_t code;
        while (true) {
                code = scanner_lookahead(scanner);
                if (code == UT_SYM_EOF || code == UT_SYM_ERR)
                        rd_parser_fatal(self);
                if (rd_parser_is_stmt_lookahead(code))
                        return code;
                scanner_shiftaway(scanner);
        }
}

/*
 * Skips to lookaheads of a declaration, const_decls, var_decls,
 * func_decls.
 */
static int rd_parser_skipto_decl(struct rd_parser *self,
                struct scanner *scanner) {
        size_t code;
        while (true) {
                code = scanner_lookahead(scanner);
                if (code == UT_SYM_EOF || code == UT_SYM_ERR)
                        rd_parser_fatal(self);
                if (code == SYM_KW_CONST || code == SYM_KW_INT || code == SYM_KW_VOID ||
                                code == SYM_CHAR)
                        return code;
                scanner_shiftaway(scanner);
        }
}

void rd_parser_error_push_back(struct rd_parser *self,
                struct error *error) {
        utillib_vector_push_back(&self->elist, error);
}

/*
 * Forward Declarations
 */

static struct utillib_json_value *
single_const_decl(struct rd_parser *self, struct scanner *scanner,
                int scope_kind);

static struct utillib_json_value *
multiple_const_decl(struct rd_parser *self, struct scanner *scanner,
                int scope_kind);

static struct utillib_json_value *scanf_stmt(struct rd_parser *self,
                struct scanner *scanner);

static struct utillib_json_value *var_defs(struct rd_parser *self,
                struct scanner *scanner,
                char const *first_iden,
                int scope_kind);

static struct utillib_json_value *for_stmt(struct rd_parser *self,
                struct scanner *scanner);

/*
 * composite_stmt may enter scope or parse decl.
 * based on this enum, it may do both or none.
 */
enum { COMP_NO_SCOPE_AND_DECL = 0, COMP_ALLOW_DECL = 1, COMP_ENTER_SCOPE = 2 };

/*
 * Handles the SYM_LB for composite_stmt.
 * Passes comp_flag to it.
 */
static struct utillib_json_value *
composite_stmt_nolookahead(struct rd_parser *self,
                struct scanner *scanner, int comp_flag);

static struct utillib_json_value *composite_stmt(struct rd_parser *self,
                struct scanner *scanner,
                int comp_flag);

static struct utillib_json_value *formal_arglist(struct rd_parser *self,
                struct scanner *scanner);

/*
 * Common construct used in if_stmt and switch_stmt.
 */
static struct utillib_json_value *
lp_expr_rp(struct rd_parser *self, struct scanner *scanner,
                /*
                 * Since it is part of other constructs,
                 * it needs context from those.
                 */
                size_t context);

/*
 * composite_stmt is considered one kind of statement,
 * as C does.
 */
static struct utillib_json_value *statement(struct rd_parser *self,
                struct scanner *scanner);

/*
 * Unlike C, we have printf and scanf built-in
 * the grammar.
 */
static struct utillib_json_value *printf_stmt(struct rd_parser *self,
                struct scanner *scanner);

static struct utillib_json_value *
function_args_body(struct rd_parser *self, struct scanner *scanner,
                struct utillib_json_value *object);

static struct utillib_json_value *function(struct rd_parser *self,
                struct scanner *scanner,
                bool *is_main);

static struct utillib_json_value *program(struct rd_parser *self,
                struct scanner *scanner);
/*
 * init/destroy
 */
void rd_parser_init(struct rd_parser *self,
                struct option const *option,
                struct symbol_table *symbol_table) {
        self->option = option;
        self->curfunc = NULL;
        self->symbol_table = symbol_table;
        self->root = utillib_json_object_create_empty();
        utillib_vector_init(&self->elist);
}

void rd_parser_destroy(struct rd_parser *self) {
        utillib_vector_destroy_owning(&self->elist, error_destroy);
        utillib_json_value_destroy(self->root);
}

int rd_parser_parse(struct rd_parser *self,
                struct scanner *scanner) {
        switch (setjmp(self->fatal_saver)) {
                case 0:
                        program(self, scanner);
                        return !utillib_vector_empty(&self->elist);
                default:
                        return 1;
        }
}

/*
 * Call when rd_parser_has_errors returns true.
 */
void rd_parser_print_error(struct rd_parser const *self) {
        struct error const *error;
        UTILLIB_VECTOR_FOREACH(error, &self->elist) { error_print(error); }
}

/*
 * Recursive Decent Subroutines
 */

/**
 * Lookahead: SYM_KW_CHAR | SYM_KW_INT
 */
static struct utillib_json_value *const_defs(struct rd_parser *self,
                struct scanner *scanner,
                size_t expected_initializer,
                int scope_kind) {

        size_t code;
        char const *name;
        struct utillib_json_value *object, *array, *value;
        struct error *error;
        const size_t context = SYM_CONST_DEF;
        assert(expected_initializer == SYM_CHAR ||
                        expected_initializer == SYM_INTEGER);

        array = utillib_json_array_create_empty();

        while (true) {
                code = scanner_lookahead(scanner);
                if (code != SYM_IDEN) {
                        error = expected_error(scanner, SYM_IDEN, context);
                        goto error;
                }
                object = utillib_json_object_create_empty();
                name = scanner_semantic(scanner);
                if (symbol_table_exist_name(self->symbol_table, name, scope_kind)) {
                        error = redefined_error(scanner, name, context);
                        goto error;
                }

                symbol_table_reserve(self->symbol_table, name, scope_kind);
                ast_set_name(object, name);
                scanner_shiftaway(scanner);

                code = scanner_lookahead(scanner);
                if (code != SYM_EQ) {
                        error = expected_error(scanner, SYM_EQ, context);
                        goto error;
                }
                scanner_shiftaway(scanner);

                code = scanner_lookahead(scanner);
                if (code != expected_initializer) {
                        error = expected_error(scanner, expected_initializer, context);
                        goto error;
                }

                value = ast_string(scanner_semantic(scanner));
                scanner_shiftaway(scanner);

                utillib_json_object_push_back(object, "value", value);
                utillib_json_array_push_back(array, object);

                code = scanner_lookahead(scanner);
                switch (code) {
                        case SYM_SEMI:
                                goto return_array;
                        case SYM_COMMA:
                                scanner_shiftaway(scanner);
                                break;
                        default:
                                /* unexpected */
                                error = unexpected_error(scanner, context);
                                goto error;
                }
        }
return_array:
        return array;
error:
        rd_parser_error_push_back(self, error);
        utillib_json_value_destroy(array);
        return NULL;

}

/**
 * Lookahead: SYM_KW_CONST
 * Maybenull
 */
static struct utillib_json_value *
single_const_decl(struct rd_parser *self, struct scanner *scanner,
                int scope_kind) {
        size_t code;
        struct utillib_json_value *object, *array;
        const size_t context = SYM_CONST_DECL;

        scanner_shiftaway(scanner);
        object = utillib_json_object_create_empty();
        code = scanner_lookahead(scanner);

        if (code != SYM_KW_CHAR && code != SYM_KW_INT) {
                rd_parser_error_push_back(
                                self, expected_error(scanner, SYM_TYPENAME, context));
                goto skip;
        }
        ast_set_type(object, code);
        scanner_shiftaway(scanner);
        array=const_defs(self, scanner, code == SYM_KW_INT ? SYM_INTEGER : SYM_CHAR, scope_kind);
        if (array)
                utillib_json_object_push_back(object, "const_defs", array);
        else
                goto return_null;

        code = scanner_lookahead(scanner);
        if (code != SYM_SEMI) {
                rd_parser_error_push_back(self,
                                expected_error(scanner, SYM_SEMI, context));
        } else {
                scanner_shiftaway(scanner);
        }
        return object;

return_null:
        utillib_json_value_destroy(object);
        return NULL;

skip:
        rd_parser_skip_init(self, SYM_SEMI, SYM_KW_CONST);
        switch (rd_parser_skipto(self, scanner)) {
                case SYM_SEMI:
                        scanner_shiftaway(scanner);
                        goto return_null;
                case SYM_KW_CONST:
                        /*
                         * This const is deliberately left for
                         * `multiple_const_decl' to parse.
                         */
                        goto return_null;
        }
}

/**
 * Lookahead: SYM_KW_CONST
 * Nonull
 */
static struct utillib_json_value *
multiple_const_decl(struct rd_parser *self, struct scanner *scanner, int scope_kind) 
{
        size_t code;
        struct utillib_json_value *array;
        struct utillib_json_value *object;

        array = utillib_json_array_create_empty();
        while (true) {
                object = single_const_decl(self, scanner, scope_kind);
                if (object != NULL) {
                        symbol_table_insert_const(self->symbol_table, object);
                        utillib_json_array_push_back(array, object);
                }
                code = scanner_lookahead(scanner);
                if (code != SYM_KW_CONST)
                        return array;
        }
}

/**
 * Lookahead: SYM_LK | SYM_SEMI | SYM_COMMA
 */
static struct utillib_json_value *var_defs(struct rd_parser *self,
                struct scanner *scanner, char const *first_iden, int scope_kind)
{
        size_t code;
        size_t expected;
        size_t const context = SYM_VAR_DEF;
        struct utillib_json_value *array = utillib_json_array_create_empty();
        struct utillib_json_value *object = utillib_json_object_create_empty();

        if (symbol_table_exist_name(self->symbol_table, first_iden,
                                scope_kind)) {
                goto redefined;
        }
        ast_set_name(object, first_iden);
        symbol_table_reserve(self->symbol_table, first_iden, scope_kind);
        utillib_json_array_push_back(array, object);

        while (true) {
loop:
                code = scanner_lookahead(scanner);
                switch (code) {
                        case SYM_LK:
                                scanner_shiftaway(scanner);
                                code = scanner_lookahead(scanner);
                                /*
                                 * Extend must be a uint.
                                 */
                                if (code != SYM_INTEGER) {
                                        expected = SYM_INTEGER;
                                        goto expected;
                                }
                                char const *extend = scanner_semantic(scanner);
                                utillib_json_object_push_back(object, "extend", ast_string(extend));
                                scanner_shiftaway(scanner);
                                code = scanner_lookahead(scanner);
                                if (code != SYM_RK) {
                                        expected = SYM_RK;
                                        goto expected;
                                }
before_rk:
                                scanner_shiftaway(scanner);
                                break;
before_comma:
                        case SYM_COMMA:
                                scanner_shiftaway(scanner);
                                code = scanner_lookahead(scanner);
                                if (code != SYM_IDEN) {
                                        expected = SYM_IDEN;
                                        goto expected;
                                }
                                first_iden = scanner_semantic(scanner);
                                if (symbol_table_exist_name(self->symbol_table, first_iden,
                                                        scope_kind)) {
                                        goto redefined;
                                }
                                object = utillib_json_object_create_empty();
                                ast_set_name(object, first_iden);
                                symbol_table_reserve(self->symbol_table, first_iden, scope_kind);
                                utillib_json_array_push_back(array, object);
                                scanner_shiftaway(scanner);
                                break;
return_array:
                        default:
                                return array;
                }
        }
redefined:
        rd_parser_error_push_back(
                        self, redefined_error(scanner, first_iden, context));
        goto loop;

expected:
        rd_parser_error_push_back(self,
                        expected_error(scanner, expected, context));
        switch (expected) {
                case SYM_INTEGER:
                        rd_parser_skip_init(self, SYM_RK, SYM_COMMA, SYM_SEMI);
                        switch (rd_parser_skipto(self, scanner)) {
                                case SYM_RK:
                                        goto before_rk;
                                case SYM_COMMA:
                                        goto before_comma;
                                case SYM_SEMI:
                                        goto return_array;
                        }
                case SYM_IDEN:
                        rd_parser_skip_init(self, SYM_COMMA, SYM_SEMI);
                        switch (rd_parser_skipto(self, scanner)) {
                                case SYM_COMMA:
                                        goto before_comma;
                                case SYM_SEMI:
                                        goto return_array;
                        }
        }
}

/**
 * Lookahead SYM_KW_INT | SYM_KW_CHAR SYM_IDEN
 * Nonull
 */
static struct utillib_json_value *singel_var_decl(struct rd_parser *self,
                struct scanner *scanner, size_t type, char *first_iden, int scope_kind) 
{
        size_t code;
        struct utillib_json_value *object;

        object = utillib_json_object_create_empty();
        ast_set_type(object, type);
        /*
         * Var_decl differs from const_decls in that
         * the first_iden and type are lookaheaded
         * which means there won't be a null one returned
         * by var_defs.
         */
        utillib_json_object_push_back(
                        object, "var_defs", var_defs(self, scanner, first_iden, scope_kind));
        code = scanner_lookahead(scanner);
        if (code != SYM_SEMI) {
                rd_parser_error_push_back(
                                self, expected_error(scanner, SYM_SEMI, SYM_VAR_DECL));
        } else {
                scanner_shiftaway(scanner);
        }
        return object;
}

/*
 * Lookahead SYM_KW_INT | SYM_KW_CHAR
 * Nonull
 */
static struct utillib_json_value *
maybe_multiple_var_decls(struct rd_parser *self,
                struct scanner *scanner, size_t *type,
                char **first_iden, int scope_kind) {
        size_t code;
        struct error *error;
        struct utillib_json_value *object;
        struct utillib_json_value *array;
        const size_t context = SYM_VAR_DECL;

        *type = scanner_lookahead(scanner);
        scanner_shiftaway(scanner);
        array = utillib_json_array_create_empty();

        while (true) {
                code = scanner_lookahead(scanner);
                if (code != SYM_IDEN && code != SYM_KW_MAIN) {
                        error = expected_error(scanner, SYM_IDEN, context);
                        goto skip;
                }
                /* Must strdup first_iden */
                *first_iden = strdup(scanner_semantic(scanner));
                scanner_shiftaway(scanner);
                code = scanner_lookahead(scanner);
                switch (code) {
                        case SYM_LP:
                                /* We may have function here */
                                goto return_array;
                        case SYM_LK:
                        case SYM_COMMA:
                        case SYM_SEMI:
                                object = singel_var_decl(self, scanner, *type, *first_iden, scope_kind);
                                free(*first_iden);
                                /*
                                 * These names were checked in var_defs.
                                 */
                                symbol_table_insert_variable(self->symbol_table, object);
                                utillib_json_array_push_back(array, object);
                                code = scanner_lookahead(scanner);
                                switch(code) {
                                        case SYM_KW_INT:
                                        case SYM_KW_CHAR:
                                                *type = code;
                                                scanner_shiftaway(scanner);
                                                break;
                                        default:
                                                goto return_array;
                                }
                                break;
                        default:
                                goto skip;
                }
        }
return_array:
        return array;

skip:
        error = unexpected_error(scanner, context);
        rd_parser_error_push_back(self, error);
        rd_parser_skip_init(self, SYM_SEMI);
        switch (rd_parser_skipto(self, scanner)) {
                case SYM_SEMI:
                        scanner_shiftaway(scanner);
                        goto return_array;
        }
}

/**
 * Lookahead SYM_KW_SCANF
 * Maybenull if **any** error happened.
 * scanf(iden...)
 */
static struct utillib_json_value *scanf_stmt(struct rd_parser *self,
                struct scanner *scanner) {
        size_t code;
        struct utillib_json_value *name;
        struct utillib_json_value *array;
        struct utillib_json_value *object;
        /*
         * This error is also used to indicate whether
         * an error ever happened. After all, what is
         * the need for an errored branch to be left
         * in the ast?
         */
        struct error *error = NULL;

        const size_t context = SYM_SCANF_STMT;
        scanner_shiftaway(scanner);
        object = utillib_json_object_create_empty();
        ast_set_type(object, SYM_SCANF_STMT);
        array = utillib_json_array_create_empty();
        utillib_json_object_push_back(object, "arglist", array);
        code = scanner_lookahead(scanner);

        if (code != SYM_LP) {
                error = expected_error(scanner, SYM_LP, context);
                goto skip;
        }
        scanner_shiftaway(scanner);

        while (true) {
                code = scanner_lookahead(scanner);
                if (code != SYM_IDEN) {
                        error = expected_error(scanner, SYM_IDEN, context);
                        goto skip;
                }
before_iden:
                name = ast_factor(code, scanner_semantic(scanner));
                if (CL_UNDEF !=
                                ast_check_iden_assignable(name, self, scanner, context))
                        utillib_json_array_push_back(array, name);
                scanner_shiftaway(scanner);
                code = scanner_lookahead(scanner);
                switch (code) {
before_comma:
                        case SYM_COMMA:
                                scanner_shiftaway(scanner);
                                break;
                        case SYM_RP:
                                scanner_shiftaway(scanner);
                                goto return_object;
                        default:
                                error = unexpected_error(scanner, context);
                                goto skip;
                }
        }
return_object:
        if (!error)
                return object;
        utillib_json_value_destroy(object);
        return NULL;

skip:
        rd_parser_error_push_back(self, error);
        rd_parser_skip_init(self, SYM_IDEN, SYM_SEMI, SYM_COMMA);
        switch (rd_parser_skipto(self, scanner)) {
                case SYM_COMMA:
                        goto before_comma;
                case SYM_IDEN:
                        goto before_iden;
                case SYM_SEMI:
                        goto return_object;
        }
}

/*
 * Lookahead SYM_LP
 * Nonull
 */
static struct utillib_json_value *
formal_arglist(struct rd_parser *self, struct scanner *scanner) {
        size_t code;
        char const *name;
        struct utillib_json_value *object;
        struct utillib_json_value *array;

        const size_t context = SYM_NARGS;

        scanner_shiftaway(scanner);
        array = utillib_json_array_create_empty();
        code = scanner_lookahead(scanner);
        if (code == SYM_RP) {
                /*
                 * Check empty.
                 */
                goto return_array;
        }

        while (true) {
                code = scanner_lookahead(scanner);
                switch (code) {
                        case SYM_KW_INT:
                        case SYM_KW_CHAR:
                                object = utillib_json_object_create_empty();
                                ast_set_type(object, code);
                                scanner_shiftaway(scanner);
                                code = scanner_lookahead(scanner);
                                if (code != SYM_IDEN) {
                                        rd_parser_error_push_back(
                                                        self, expected_error(scanner, SYM_IDEN, context));
                                        goto skip;
                                }
                                name = scanner_semantic(scanner);
                                if (symbol_table_exist_name(self->symbol_table, name, CL_LOCAL)) {
                                        utillib_json_value_destroy(object);
                                        rd_parser_error_push_back(
                                                        self, redefined_error(scanner, name, context));
                                        goto skip;
                                }
                                symbol_table_reserve(self->symbol_table, name, CL_LOCAL);
                                ast_set_name(object, name);
                                scanner_shiftaway(scanner);
                                utillib_json_array_push_back(array, object);
                                code = scanner_lookahead(scanner);
                                switch (code) {
                                        case SYM_COMMA:
                                                scanner_shiftaway(scanner);
                                                break;
                                        case SYM_RP:
                                                goto return_array;
                                        default:
                                                goto unexpected;
                                }
                                break;
                        default:
                                goto unexpected;
                }
        }

return_array:
        scanner_shiftaway(scanner);
        return array;

unexpected:
        rd_parser_error_push_back(self, unexpected_error(scanner, context));
        goto skip;

skip:
        /*
         * If some error happened, we skip to SYM_RP
         * because any attempt to recover at the midway
         * of an arglist may be wroing-guessing.
         * For example:
         * int foo(int i,char, char b)
         * can we interpret it as
         * int foo(int i, char b) ?
         */
        rd_parser_skip_init(self, SYM_RP);
        switch (rd_parser_skipto(self, scanner)) {
                case SYM_RP:
                        goto return_array;
        }
}

/*
 * Lookahead SYM_IDEN
 * Nonull, but expr will be absent
 * when error.
 */
static struct utillib_json_value *expr_stmt(struct rd_parser *self,
                struct scanner *scanner) {

        struct opg_parser opg_parser;
        struct utillib_json_value *object, *expr;
        const size_t context = SYM_EXPR_STMT;

        object = ast_statement(SYM_EXPR_STMT);
        opg_parser_init(&opg_parser, SYM_SEMI);

        expr = opg_parser_parse(&opg_parser, scanner);
        if (expr == NULL) {
                goto expected_expr;
        }
        if (CL_UNDEF == ast_check_expr_stmt(expr, self, scanner, context)) {
                utillib_json_value_destroy(expr);
                goto return_null;
        }

        utillib_json_object_push_back(object, "expr", expr);
        opg_parser_destroy(&opg_parser);
        return object;

return_null:
        opg_parser_destroy(&opg_parser);
        utillib_json_value_destroy(object);
        return NULL;

expected_expr:
        /*
         * An expression statement can go wrong easily.
         * Any grammatical error causes the whole expr
         * to lose due to the nature of opg_parser.
         * And we skip to SYM_SEMI.
         */
        rd_parser_error_push_back(
                        self, expected_error(scanner, SYM_EXPR, SYM_EXPR_STMT));
        rd_parser_skip_init(self, SYM_SEMI);
        switch (rd_parser_skipto(self, scanner)) {
                case SYM_SEMI:
                        goto return_null;
        }
}

/*
 * Lookahead SYM_KW_RETURN
 * Maybenull
 */
static struct utillib_json_value *return_stmt(struct rd_parser *self,
                struct scanner *scanner) {
        size_t code;
        struct utillib_json_value *expr = NULL, *object;
        struct opg_parser opg_parser;

        const size_t context = SYM_RETURN_STMT;
        bool void_flag;

        scanner_shiftaway(scanner);
        opg_parser_init(&opg_parser, SYM_SEMI);
        object = ast_statement(SYM_RETURN_STMT);
        code = scanner_lookahead(scanner);

        switch (code) {
                case SYM_SEMI:
                        void_flag = true;
                        goto check_return;
                case SYM_LP:
parse_expr:
                        expr = opg_parser_parse(&opg_parser, scanner);
                        if (expr == NULL) {
                                /*
                                 * If the expr is null, which is a syntax error,
                                 * there is no need to check for returnness.
                                 */
                                rd_parser_error_push_back(
                                                self, expected_error(scanner, SYM_EXPR, context));
                                goto return_null;
                        }
                        void_flag = false;
                        goto check_return;
                default:
                        if (self->option->plain_return) {
                                goto parse_expr;
                        }
                        rd_parser_error_push_back(self, unexpected_error(scanner, context));
                        goto skip;
        }

check_return:
        if (CL_UNDEF ==
                        ast_check_returnness(expr, self, scanner, context, void_flag))
                goto return_null;
        goto return_object;

return_object:
        if (!void_flag)
                /*
                 * We are guarding NULL entering our
                 * AST, which is a disaster.
                 */
                utillib_json_object_push_back(object, "expr", expr);
        opg_parser_destroy(&opg_parser);
        return object;

return_null:
        /*
         * Destroy everything.
         */
        if (expr)
                utillib_json_value_destroy(expr);
        opg_parser_destroy(&opg_parser);
        utillib_json_value_destroy(object);
        return NULL;

skip:
        /*
         * Again, just skip to SYM_SEMI and
         * abort the whole statement.
         */
        rd_parser_skip_init(self, SYM_SEMI);
        switch (rd_parser_skipto(self, scanner)) {
                case SYM_SEMI:
                        goto return_null;
        }
}

/**
 * Lookahead None
 * Maybe null
 * Skips to SYM_RP
 */
static struct utillib_json_value *lp_expr_rp(struct rd_parser *self,
                struct scanner *scanner,
                size_t context) {
        size_t code;
        struct opg_parser opg_parser;
        struct utillib_json_value *expr;

        opg_parser_init(&opg_parser, SYM_RP);
        code = scanner_lookahead(scanner);

        if (code != SYM_LP) {
                /*
                 * If the SYM_LP is missing it will be
                 * too naive to abort the whole expression.
                 * So we raise an error and go on the parse
                 * the expression.
                 */
                rd_parser_error_push_back(self,
                                expected_error(scanner, SYM_LP, context));
        } else {
                scanner_shiftaway(scanner);
        }
        expr = opg_parser_parse(&opg_parser, scanner);
        if (expr == NULL) {
                rd_parser_error_push_back(self,
                                expected_error(scanner, SYM_EXPR, context));
                goto skip;
        }

return_expr:
        scanner_shiftaway(scanner);
        opg_parser_destroy(&opg_parser);
        return expr;

skip:
        /*
         * Again, the error expr causes us to
         * skip to SYM_RP and return null.
         */
        rd_parser_skip_init(self, SYM_RP);
        switch (rd_parser_skipto(self, scanner)) {
                case SYM_RP:
                        goto return_expr;
        }
}

/**
 * Lookahead SYM_KW_IF
 */
static struct utillib_json_value *if_stmt(struct rd_parser *self,
                struct scanner *scanner) {
        size_t code;
        struct utillib_json_value *expr, *object, *then_clause, *else_clause;

        scanner_shiftaway(scanner);
        const size_t context = SYM_IF_STMT;
        object = ast_statement(SYM_IF_STMT);

        expr = lp_expr_rp(self, scanner, context);
        if (expr != NULL) {
                if (CL_UNDEF != ast_check_condition(expr, self, scanner, context))
                        utillib_json_object_push_back(object, "expr", expr);
                else
                        utillib_json_value_destroy(expr);
        }
        then_clause = statement(self, scanner);
        if (then_clause)
                utillib_json_object_push_back(object, "then", then_clause);
        code = scanner_lookahead(scanner);
        if (code != SYM_KW_ELSE) {
                goto return_object;
        }
        scanner_shiftaway(scanner);
        else_clause = statement(self, scanner);
        if (else_clause)
                utillib_json_object_push_back(object, "else", else_clause);

return_object:
        return object;
}

/*
 * Lookahead SYM_KW_CASE | SYM_KW_DEFAULT
 * Maybenull
 */
static struct utillib_json_value *
switch_stmt_case_clause(struct rd_parser *self,
                struct scanner *scanner,
                struct utillib_hashmap *label_map) {
        size_t code;
        char const *label;
        struct error *error = NULL;
        struct utillib_json_value *object, *stmt;
        const size_t context = SYM_CASE_CLAUSE;
        object = utillib_json_object_create_empty();
        code = scanner_lookahead(scanner);
        scanner_shiftaway(scanner);
        switch (code) {
                case SYM_KW_DEFAULT:
                        utillib_json_object_push_back(object, "default", &utillib_json_true);
                        goto parse_colon_stmt;
                case SYM_KW_CASE:
                        code = scanner_lookahead(scanner);
                        switch (code) {
                                case SYM_INTEGER:
                                case SYM_CHAR:
                                        label = scanner_semantic(scanner);
                                        if (utillib_hashmap_exist_key(label_map, label)) {
                                                /*
                                                 * If a case label is duplicated, we parse the stmt
                                                 * but not add it to ast.
                                                 */
                                                rd_parser_error_push_back(
                                                                self, error = dupcase_error(scanner, code, label, context));
                                                /*
                                                 * Get rid of this duplicated case
                                                 */
                                                scanner_shiftaway(scanner);
                                                goto parse_colon_stmt;
                                        }
                                        utillib_hashmap_insert(label_map, strdup(label), NULL);
                                        utillib_json_object_push_back(object, "case",
                                                        ast_factor(code, label));
                                        scanner_shiftaway(scanner);
                                        goto parse_colon_stmt;
                                default:
                                        rd_parser_error_push_back(
                                                        self, error = expected_error(scanner, SYM_CONSTANT, context));
                                        goto skip;
                        }
                default:
                        assert(false);
        }
parse_colon_stmt:
        code = scanner_lookahead(scanner);
        if (code != SYM_COLON) {
                rd_parser_error_push_back(
                                self, error = expected_error(scanner, SYM_COLON, context));
                rd_parser_skipto_stmt(self, scanner);
        } else {
                scanner_shiftaway(scanner);
        }
        stmt = statement(self, scanner);
        if (stmt) {
                utillib_json_object_push_back(object, "stmt", stmt);
        }
        if (error)
                goto return_null;
        return object;

return_null:
        utillib_json_value_destroy(object);
        return NULL;
skip:
        rd_parser_skip_init(self, SYM_KW_CASE, SYM_KW_DEFAULT);
        switch (rd_parser_skipto(self, scanner)) {
                case SYM_KW_CASE:
                case SYM_KW_DEFAULT:
                        goto return_null;
        }
}

/*
 * Lookahead SYM_KW_CASE
 * Nonull
 */
static struct utillib_json_value *
switch_stmt_cases(struct rd_parser *self, struct scanner *scanner) {
        size_t code;
        struct utillib_json_value *array, *object;
        struct utillib_hashmap label_map;

        /*
         * constant in the case label are not allowed to have different
         * typs.
         */

        const size_t context = SYM_CASE_CLAUSE;
        array = utillib_json_array_create_empty();
        utillib_hashmap_init(&label_map, &string_hash);

        while (true) {
loop:
                code = scanner_lookahead(scanner);
                switch (code) {
                        case SYM_KW_DEFAULT:
                                object = switch_stmt_case_clause(self, scanner, &label_map);
                                if (object)
                                        utillib_json_array_push_back(array, object);
                                goto return_array;
                        case SYM_KW_CASE:
                                object = switch_stmt_case_clause(self, scanner, &label_map);
                                if (object)
                                        utillib_json_array_push_back(array, object);
                                break;
                        default:
                                goto unexpected;
                }
        }

return_array:
        utillib_hashmap_destroy_owning(&label_map, free, NULL);
        return array;

unexpected:
        rd_parser_error_push_back(self, unexpected_error(scanner, context));
        rd_parser_skip_init(self, SYM_KW_CASE, SYM_KW_DEFAULT, SYM_RB);
        switch (rd_parser_skipto(self, scanner)) {
                case SYM_KW_CASE:
                case SYM_KW_DEFAULT:
                        goto loop;
                case SYM_RB:
                        goto return_array;
        }
}

/*
 * Lookahead SYM_KW_SWITCH
 * cases and expr may both be absent.
 * Nonull
 */
static struct utillib_json_value *switch_stmt(struct rd_parser *self,
                struct scanner *scanner) {
        size_t code;
        struct utillib_json_value *object, *expr, *cases;
        struct error *error;
        int case_type;

        const size_t context = SYM_SWITCH_STMT;
        scanner_shiftaway(scanner);
        object = ast_statement(SYM_SWITCH_STMT);
        expr = lp_expr_rp(self, scanner, context);
        if (NULL == expr) {
                /*
                 * If lp_expr_rp returns NULL, it has
                 * skipped to SYM_RP.
                 */
                goto parse_lb;
        }
        case_type = ast_check_expression(expr, self, scanner, context);
        if (case_type == CL_UNDEF)
                utillib_json_value_destroy(expr);
        else
                utillib_json_object_push_back(object, "expr", expr);

parse_lb:
        code = scanner_lookahead(scanner);
        if (code != SYM_LB) {
                goto expected_lb;
        }
        scanner_shiftaway(scanner);

parse_cases:
        cases = switch_stmt_cases(self, scanner);
        utillib_json_object_push_back(object, "cases", cases);
        code = scanner_lookahead(scanner);
        if (code != SYM_RB) {
                error = expected_error(scanner, SYM_RB, context);
                goto expected_rb;
        }

return_object:
        scanner_shiftaway(scanner);
        return object;

expected_lb:
        rd_parser_error_push_back(self, error);
        rd_parser_skip_init(self, SYM_KW_CASE, SYM_RB);
        switch (rd_parser_skipto(self, scanner)) {
                case SYM_KW_CASE:
                        goto parse_cases;
                case SYM_RB:
                        goto return_object;
        }

expected_rb:
        rd_parser_error_push_back(self, error);
        return object;
}

/**
 * Lookahead None
 * Maybe null
 */
static struct utillib_json_value *statement(struct rd_parser *self,
                struct scanner *scanner) {
        size_t code = scanner_lookahead(scanner);
        struct utillib_json_value *object;
        const size_t context = SYM_STMT;

        switch (code) {
                case SYM_KW_RETURN:
                        object = return_stmt(self, scanner);
                        goto parse_semi;
                case SYM_KW_PRINTF:
                        object = printf_stmt(self, scanner);
                        goto parse_semi;
                case SYM_KW_SCANF:
                        object = scanf_stmt(self, scanner);
                        goto parse_semi;
                case SYM_KW_FOR:
                        object = for_stmt(self, scanner);
                        goto return_object;
                case SYM_KW_IF:
                        object = if_stmt(self, scanner);
                        goto return_object;
                case SYM_KW_SWITCH:
                        object = switch_stmt(self, scanner);
                        goto return_object;
                case SYM_IDEN:
                        /*
                         * Although the Lookahead for expression
                         * is far vaster than iden, according to
                         * the grammar, expr_stmt = assign_stmt | call_stmt,
                         * both of which start with iden.
                         * Thus, iden is simply enough.
                         */
                        object = expr_stmt(self, scanner);
                        goto parse_semi;
                case SYM_LB:
                        /*
                         * Since we are entering a statement
                         * not a function, we need a new scope.
                         *
                         */
                        object = composite_stmt_nolookahead(self, scanner, COMP_NO_SCOPE_AND_DECL);
                        goto return_object;
                case SYM_SEMI:
                        /*
                         * It is important that the empty
                         * statement is retained as json_null.
                         * because it is critical to grammatical
                         * correctness which the AST should reflect.
                         */
                        scanner_shiftaway(scanner);
                        return utillib_json_null_create();
                default:
                        rd_parser_error_push_back(self, unexpected_error(scanner, context));
                        goto unexpected;
        }
return_object:
        return object;
return_null:
        scanner_shiftaway(scanner);
        return NULL;

unexpected:
        rd_parser_skip_init(self, SYM_SEMI);
        switch (rd_parser_skipto(self, scanner)) {
                case SYM_SEMI:
                        goto return_null;
        }
parse_semi:
        code = scanner_lookahead(scanner);
        if (code == SYM_SEMI) {
                scanner_shiftaway(scanner);
                goto return_object;
        }
        rd_parser_error_push_back(self,
                        expected_error(scanner, SYM_SEMI, context));
        goto return_object;
}

/*
 * Lookahead SYM_KW_FOR
 * init, cond, step, stmt may all be absent.
 * Nonull
 */
static struct utillib_json_value *for_stmt(struct rd_parser *self,
                struct scanner *scanner) {
        size_t code;
        struct opg_parser opg_parser;
        struct utillib_json_value *init, *cond, *step, *stmt, *object;

        size_t context = SYM_FOR_STMT;
        opg_parser_init(&opg_parser, SYM_SEMI);
        object = ast_statement(SYM_FOR_STMT);

        scanner_shiftaway(scanner);
        code = scanner_lookahead(scanner);
        if (code != SYM_LP) {
                rd_parser_error_push_back(self,
                                expected_error(scanner, SYM_LP, context));
                goto expected_lp;
        }

        context = SYM_FOR_INIT;
        scanner_shiftaway(scanner);
        init = opg_parser_parse(&opg_parser, scanner);
        if (init == NULL) {
                rd_parser_error_push_back(self,
                                expected_error(scanner, SYM_EXPR, context));
                goto expected_init;
        }
        if (CL_UNDEF == ast_check_for_init(init, self, scanner, context)) {
                utillib_json_value_destroy(init);
                goto expected_init;
        }
        utillib_json_object_push_back(object, "init", init);
parse_cond:
        context = SYM_CONDITION;
        scanner_shiftaway(scanner);
        opg_parser_reinit(&opg_parser, SYM_SEMI);
        cond = opg_parser_parse(&opg_parser, scanner);
        if (cond == NULL) {
                rd_parser_error_push_back(self,
                                expected_error(scanner, SYM_EXPR, context));
                goto expected_cond;
        }
        if (CL_UNDEF == ast_check_condition(cond, self, scanner, context)) {
                utillib_json_value_destroy(cond);
                goto expected_cond;
        }
        utillib_json_object_push_back(object, "cond", cond);
parse_step:
        context = SYM_FOR_STEP;
        scanner_shiftaway(scanner);
        opg_parser_reinit(&opg_parser, SYM_RP);
        step = opg_parser_parse(&opg_parser, scanner);
        opg_parser_destroy(&opg_parser);
        if (step == NULL) {
                rd_parser_error_push_back(self,
                                expected_error(scanner, SYM_EXPR, context));
                goto expected_step;
        }
        if (CL_UNDEF == ast_check_for_step(step, self, scanner, context)) {
                utillib_json_value_destroy(step);
                goto expected_step;
        }
        utillib_json_object_push_back(object, "step", step);
        code = scanner_lookahead(scanner);
        if (code != SYM_RP) {
                rd_parser_error_push_back(self,
                                expected_error(scanner, SYM_RP, context));
                goto expected_rp;
        }

parse_stmt:
        scanner_shiftaway(scanner);
        stmt = statement(self, scanner);
        if (stmt != NULL)
                utillib_json_object_push_back(object, "stmt", stmt);
        return object;

expected_lp:
        rd_parser_skip_init(self, SYM_SEMI, SYM_RP);
        switch (rd_parser_skipto(self, scanner)) {
                case SYM_SEMI:
                        goto parse_cond;
                case SYM_RP:
                        goto parse_stmt;
        }

expected_init:
        rd_parser_skip_init(self, SYM_SEMI, SYM_RP);
        switch (rd_parser_skipto(self, scanner)) {
                case SYM_SEMI:
                        goto parse_cond;
                case SYM_RP:
                        goto parse_stmt;
        }
expected_cond:
        rd_parser_skip_init(self, SYM_SEMI, SYM_RP);
        switch (rd_parser_skipto(self, scanner)) {
                case SYM_SEMI:
                        goto parse_step;
                case SYM_RP:
                        goto parse_stmt;
        }
expected_step:
        rd_parser_skip_init(self, SYM_RP);
        switch (rd_parser_skipto(self, scanner)) {
                case SYM_RP:
                        goto parse_stmt;
        }
expected_rp:
        goto parse_stmt;
}

static bool rd_parser_is_stmt_lookahead(size_t code) {
        return code == SYM_KW_PRINTF || code == SYM_KW_SCANF || code == SYM_KW_FOR ||
                code == SYM_KW_IF || code == SYM_KW_SWITCH || code == SYM_KW_RETURN ||
                code == SYM_LB || code == SYM_SEMI || code == SYM_IDEN;
}

/*
 * Lookahead None
 * Nonull
 */
static struct utillib_json_value *
multiple_statement(struct rd_parser *self,
                struct scanner *scanner) {
        size_t code;
        struct utillib_json_value *object;
        struct utillib_json_value *array = utillib_json_array_create_empty();

        while (true) {
                code = scanner_lookahead(scanner);
                if (!rd_parser_is_stmt_lookahead(code))
                        return array;
                object = statement(self, scanner);
                if (object != NULL)
                        utillib_json_array_push_back(array, object);
        }
}

/*
 * Ensures a lookahead of `SYM_LB' and then call
 * composite_stmt.
 * Normally all the functions require their caller to
 * ensure lookahead token for them, which makes this
 * parser predictive, but in case of composite_stmt,
 * those lookahead-ensuring code is put into **this**
 * function, which should always be used.
 * Nonull
 */
static struct utillib_json_value *
composite_stmt_nolookahead(struct rd_parser *self,
                struct scanner *scanner, int comp_flag) {

        size_t code;
        const size_t context = SYM_COMP_STMT;

        code = scanner_lookahead(scanner);
        if (code != SYM_LB) {
                goto expected_lb;
        }
        scanner_shiftaway(scanner);

parse_comp:
        return composite_stmt(self, scanner, comp_flag);

expected_lb:
        /*
         * Missing a SYM_LB is very typical.
         * In that we just do some favour for composite_stmt
         * by skipping to its Lookaheads.
         */
        rd_parser_error_push_back(self,
                        expected_error(scanner, SYM_LB, context));
        rd_parser_skip_init(self, SYM_KW_CONST, SYM_KW_INT, SYM_KW_CHAR);
        switch (rd_parser_skipto(self, scanner)) {
                case SYM_KW_CONST:
                case SYM_KW_INT:
                case SYM_KW_CHAR:
                        goto parse_comp;
        }
}

/*
 * Lookahead SYM_KW_CONST | SYM_KW_INT | SYM_CHAR |
 * all those for statement.
 * Notes the stmts field may be missing because {}
 * Nonull
 */
static struct utillib_json_value *composite_stmt(struct rd_parser *self,
                struct scanner *scanner,
                int comp_flag) {
        size_t code;
        size_t type;
        char *first_iden;
        struct utillib_json_value *object, *const_decls, *var_decls, *stmts;
        const size_t context = SYM_COMP_STMT;

        object = ast_statement(SYM_COMP_STMT);
        if (comp_flag & COMP_ENTER_SCOPE)
                /*
                 * Sometimes scope is entered by `function_args_body'
                 */
                symbol_table_enter_scope(self->symbol_table);
        if (comp_flag & COMP_ALLOW_DECL) {
                /*
                 * function-scope composite_stmt can
                 * have decls.
                 */
                code = scanner_lookahead(scanner);
                if (code == SYM_KW_CONST) {
                        const_decls = multiple_const_decl(self, scanner, CL_LOCAL);
                        utillib_json_object_push_back(object, "const_decls", const_decls);
                }
                code = scanner_lookahead(scanner);
                if (code == SYM_KW_CHAR || code == SYM_KW_INT) {
                        var_decls =
                                maybe_multiple_var_decls(self, scanner, &type, &first_iden, CL_LOCAL);
                        utillib_json_object_push_back(object, "var_decls", var_decls);
                }
        }
        code = scanner_lookahead(scanner);
        if (code == SYM_RB)
                /*
                 * Handle the empty composite_stmt.
                 */
                goto parse_rb;

        code = scanner_lookahead(scanner);
        if (!rd_parser_is_stmt_lookahead(code)) {
                rd_parser_error_push_back(self, unexpected_error(scanner, context));
                rd_parser_skipto_stmt(self, scanner);
        }
        stmts = multiple_statement(self, scanner);
        utillib_json_object_push_back(object, "stmts", stmts);

parse_rb:
        code = scanner_lookahead(scanner);
        if (code != SYM_RB) {
                rd_parser_error_push_back(
                                self, expected_error(scanner, SYM_RB, SYM_COMP_STMT));
        } else {
                scanner_shiftaway(scanner);
        }

        if (comp_flag & COMP_ENTER_SCOPE)
                symbol_table_leave_scope(self->symbol_table);
        return object;
}

/*
 * Lookahead SYM_KW_PRINTF
 * printf(string)
 * printf(string, expr)
 * printf(expr)
 * Nonull
 */
static struct utillib_json_value *printf_stmt(struct rd_parser *self,
                struct scanner *scanner) {
        size_t code;
        struct opg_parser opg_parser;
        struct utillib_json_value *array, *object, *expr, *string;
        const size_t context = SYM_PRINTF_STMT;

        opg_parser_init(&opg_parser, SYM_RP);
        array = utillib_json_array_create_empty();
        object = ast_statement(SYM_PRINTF_STMT);
        utillib_json_object_push_back(object, "arglist", array);

        scanner_shiftaway(scanner);
        code = scanner_lookahead(scanner);

        if (code != SYM_LP) {
                rd_parser_error_push_back(self,
                                expected_error(scanner, SYM_LP, context));
                goto skip;
        }
        scanner_shiftaway(scanner);
        code = scanner_lookahead(scanner);

        switch (code) {
                case SYM_STRING:
                        string = ast_factor(code, scanner_semantic(scanner));
                        utillib_json_array_push_back(array, string);
                        scanner_shiftaway(scanner);
                        code = scanner_lookahead(scanner);
                        switch (code) {
                                case SYM_COMMA:
                                        scanner_shiftaway(scanner);
                                        goto parse_expr;
                                case SYM_RP:
                                        goto parse_rp;
                                default:
                                        goto unexpected;
                        }
                        break;
                        /*
                         * These are lookaheads for expr, note we use SYM_INTEGER & SYM_ADD
                         * instead of SYM_INTEGER.
                         */
                case SYM_IDEN:
                case SYM_LP:
                case SYM_INTEGER:
                case SYM_ADD:
                case SYM_MINUS:
                case SYM_CHAR:
parse_expr:
                        expr = opg_parser_parse(&opg_parser, scanner);
                        if (NULL == expr) {
                                rd_parser_error_push_back(
                                                self, expected_error(scanner, SYM_EXPR, context));
                                goto skip;
                        }
                        if (CL_UNDEF == ast_check_expression(expr, self, scanner, context)) {
                                goto skip;
                        }
                        utillib_json_array_push_back(array, expr);
                        goto parse_rp;
                default:
                        goto unexpected;
        }

parse_rp:
        code = scanner_lookahead(scanner);
        if (code != SYM_RP) {
                rd_parser_error_push_back(self,
                                expected_error(scanner, SYM_RP, context));
                goto skip;
        }
        scanner_shiftaway(scanner);

return_object:
        opg_parser_destroy(&opg_parser);
        return object;

unexpected:
        rd_parser_error_push_back(self, unexpected_error(scanner, context));

skip:
        rd_parser_skip_init(self, SYM_RP, SYM_SEMI);
        switch (rd_parser_skipto(self, scanner)) {
                case SYM_RP:
                        goto parse_rp;
                case SYM_SEMI:
                        goto return_object;
        }
}

/*
 * Lookahead SYM_LP
 * Nonull
 */
static struct utillib_json_value *
function_args_body(struct rd_parser *self, struct scanner *scanner,
                struct utillib_json_value *object) {

        struct utillib_json_value *comp_stmt, *arglist;

        symbol_table_enter_scope(self->symbol_table);
        arglist = formal_arglist(self, scanner);
        symbol_table_insert_arglist(self->symbol_table, arglist);
        utillib_json_object_push_back(object, "arglist", arglist);
        if (utillib_json_object_at(object, "name")) {
                /*
                 * A function may lose its name due to redefinition.
                 * Check here.
                 */
                symbol_table_insert_function(self->symbol_table, object);
        }

        /*
         * Handle maybe-missing SYM_LB
         */
        comp_stmt = composite_stmt_nolookahead(self, scanner, COMP_ALLOW_DECL);
        utillib_json_object_push_back(object, "comp", comp_stmt);
        symbol_table_leave_scope(self->symbol_table);
        /*
         * It is important to reset the curfunc field
         * since it may cause false positive.
         */
        self->curfunc = NULL;
        return object;
}

/*
 * Lookahead SYM_KW_VOID | SYM_KW_INT | SYM_KW_CHAR
 * Nonull
 */
static struct utillib_json_value *function(struct rd_parser *self,
                struct scanner *scanner,
                bool *is_main) {
        size_t code;
        struct utillib_json_value *object;
        struct utillib_json_value *comp;
        char const *name;

        const size_t context = SYM_FUNC_DECL;
        code = scanner_lookahead(scanner);
        object = utillib_json_object_create_empty();
        ast_set_type(object, code);
        scanner_shiftaway(scanner);

        code = scanner_lookahead(scanner);
        switch (code) {
                /*
                 * First of all, we try to recognize
                 * its name -- whether it is main.
                 */
                case SYM_IDEN:
                        *is_main = false;
                        name = scanner_semantic(scanner);
                        break;
                case SYM_KW_MAIN:
                        *is_main = true;
                        name = "main";
                        break;
                default:
                        goto unexpected;
        }
        if (symbol_table_exist_name(self->symbol_table, name, CL_GLOBAL)) {
                goto redefined;
        }
        symbol_table_reserve(self->symbol_table, name, CL_GLOBAL);
        self->curfunc = ast_set_name(object, name);

parse_args_body:
        /* SYM_IDEN */
        scanner_shiftaway(scanner);
        code = scanner_lookahead(scanner);
        if (code != SYM_LP) {
                goto expected_lp;
        }
        return function_args_body(self, scanner, object);
redefined:
        /*
         * Although the name is redefined, we still need
         * to look into its arglist and composite_stmt
         * to pick up more possible errors.
         */
        rd_parser_error_push_back(self,
                        redefined_error(scanner, name, context));
        goto parse_args_body;

unexpected:
        /*
         * We lost the name of the function.
         */
        self->curfunc = NULL;
        rd_parser_error_push_back(self, unexpected_error(scanner, context));
        rd_parser_skip_init(self, SYM_LP, SYM_RP, SYM_LB);
        switch (rd_parser_skipto(self, scanner)) {
                case SYM_LP:
                        goto parse_args_body;
                case SYM_RP:
                        scanner_shiftaway(scanner);
                        goto parse_comp;
                case SYM_LB:
                        goto parse_comp;
        }
expected_lp:
        rd_parser_skip_init(self, SYM_RP, SYM_LB);
        rd_parser_error_push_back(self,
                        expected_error(scanner, SYM_LP, context));
        switch (rd_parser_skipto(self, scanner)) {
                case SYM_RP:
                        scanner_shiftaway(scanner);
                        goto parse_comp;
                case SYM_LB:
                        goto parse_comp;
        }
parse_comp:
        /*
         * Since the arglist is missing totally,
         * we directly enter a new scope in
         * composite_stmt_nolookahead.
         */
        comp = composite_stmt_nolookahead(self, scanner,
                        COMP_ALLOW_DECL | COMP_ENTER_SCOPE);
        utillib_json_object_push_back(object, "comp", comp);
        return object;
}

/*
 * Lookahead SYM_KW_CHAR | SYM_KW_INT SYM_IDEN SYM_LP
 * Nonull
 */
static struct utillib_json_value *first_function(struct rd_parser *self,
                struct scanner *scanner,
                size_t type, char const *name,
                bool *is_main) {
        const size_t context = SYM_FUNC_DECL;
        struct utillib_json_value *object;
        object = utillib_json_object_create_empty();
        ast_set_type(object, type);
        *is_main=strcmp(name, "main")==0;

        if (symbol_table_exist_name(self->symbol_table, name, CL_GLOBAL)) {
                rd_parser_error_push_back(self,
                                redefined_error(scanner, name, context));
        } else {
                symbol_table_reserve(self->symbol_table, name, CL_GLOBAL);
                self->curfunc = ast_set_name(object, name);
        }
        return function_args_body(self, scanner, object);
}

/*
 * Lookahead SYM_KW_VOID | SYM_LP
 * Nonull
 */
static struct utillib_json_value *
multiple_function(struct rd_parser *self, struct scanner *scanner,
                size_t maybe_type, char *maybe_name) {
        size_t code;

        bool is_main;
        struct utillib_json_value *array;
        struct utillib_json_value *object;

        code = scanner_lookahead(scanner);
        array = utillib_json_array_create_empty();

        switch (code) {
                case SYM_KW_VOID:
                        object = function(self, scanner, &is_main);
                        utillib_json_array_push_back(array, object);
                        if (is_main) {
                                goto return_array;
                        }
                        break;
                case SYM_LP:
                        object = first_function(self, scanner, maybe_type, maybe_name, &is_main);
                        /*
                         * maybe_name is strdup by `maybe_multiple_var_decls'.
                         */
                        free(maybe_name);
                        utillib_json_array_push_back(array, object);
                        if (is_main)
                                goto return_array;
                        break;
                default:
                        goto unexpected;
        }
        while (true) {
                code = scanner_lookahead(scanner);
                switch (code) {
                        case SYM_KW_VOID:
                        case SYM_KW_INT:
                        case SYM_KW_CHAR:
                                object = function(self, scanner, &is_main);
                                utillib_json_array_push_back(array, object);
                                if (is_main)
                                        goto return_array;
                                break;
                        default:
                                goto unexpected;
                }
        }
return_array:
        code = scanner_lookahead(scanner);
        if (code != UT_SYM_EOF) {
                rd_parser_error_push_back(self,
                                unexpected_error(scanner, SYM_FUNC_DECL));
        }
        return array;

unexpected:
        rd_parser_error_push_back(self,
                        unexpected_error(scanner, SYM_FUNC_DECL));
        utillib_json_value_destroy(array);
        rd_parser_fatal(self);
}

/*
 * Lookahead None
 * Nonull
 */
static struct utillib_json_value *program(struct rd_parser *self,
                struct scanner *scanner) {
        size_t code;
        size_t type;
        char *first_iden;
        struct utillib_json_value *const_decls, *var_decls, *func_decls;

        code = scanner_lookahead(scanner);
        switch (code) {
                case SYM_KW_CONST:
parse_const:
                        const_decls = multiple_const_decl(self, scanner, CL_GLOBAL);
                        utillib_json_object_push_back(self->root, "const_decls", const_decls);
                        /* Fall through */
                case SYM_KW_INT:
                case SYM_KW_CHAR:
parse_maybe_var:
                        var_decls =
                                maybe_multiple_var_decls(self, scanner, &type, &first_iden, CL_GLOBAL);
                        utillib_json_object_push_back(self->root, "var_decls", var_decls);
                        /* Fall through */
                case SYM_KW_VOID:
parse_function:
                        func_decls = multiple_function(self, scanner, type, first_iden);
                        utillib_json_object_push_back(self->root, "func_decls", func_decls);
                        break;
                default:
                        goto unexpected;
        }
        return self->root;
unexpected:
        rd_parser_error_push_back(self, unexpected_error(scanner, SYM_PROGRAM));
        switch (rd_parser_skipto_decl(self, scanner)) {
                case SYM_KW_CONST:
                        goto parse_const;
                case SYM_KW_INT:
                case SYM_KW_CHAR:
                        goto parse_maybe_var;
                case SYM_KW_VOID:
                        goto parse_function;
        }
}
