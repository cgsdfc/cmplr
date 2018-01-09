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
#include "error.h"
#include "ast_pretty.h"
#include "misc.h"
#include "scanner.h"
#include "symbol_table.h"
#include "symbols.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define cling_symbol_cast(code)                                                \
        ((code) == UT_SYM_EOF ? "end-of-input" : cling_symbol_kind_tostring((code)))

static struct cling_error * error_create(int kind, struct cling_scanner const *scanner, size_t context) {

        struct cling_error *self = malloc(sizeof *self);
        self->kind = kind;
        self->row = scanner->input.row;
        self->col = scanner->input.col;
        self->context = cling_symbol_cast(context);
        return self;
}

struct cling_error *cling_expected_error(struct cling_scanner const *scanner, size_t expected, size_t context) 
{
        struct cling_error *self = error_create(CL_EEXPECT, scanner, context);
        self->expected.expected = cling_symbol_cast(expected);
        self->expected.actual = cling_symbol_cast(cling_scanner_lookahead(scanner));
        return self;
}

struct cling_error *cling_unexpected_error(struct cling_scanner const *scanner, size_t context) 
{
        struct cling_error *self = error_create(CL_EUNEXPECTED, scanner, context);
        self->unexpected.unexpected =
                cling_symbol_cast(cling_scanner_lookahead(scanner));
        return self;
}

struct cling_error *cling_redefined_error(struct cling_scanner const *scanner, char const *name, size_t context) 
{
        struct cling_error *self = error_create(CL_EREDEFINED, scanner, context);
        self->redefined.name = strdup(name);
        return self;
}

struct cling_error * cling_undefined_name_error(struct cling_scanner const *scanner, char const *name, size_t context) 
{
        struct cling_error *self = error_create(CL_EUNDEFINED, scanner, context);
        self->undefined.name = strdup(name);
        return self;
}

struct cling_error * cling_not_lvalue_error(struct cling_scanner const *scanner, struct utillib_json_value const *value, size_t context) 
{
        struct cling_error *self = error_create(CL_ENOTLVALUE, scanner, context);
        self->not_lvalue.value = cling_ast_pretty_expr(value);
        return self;
}

struct cling_error * cling_incompatible_type_error(struct cling_scanner const *scanner, int actual_type, int expected_type, size_t context) 
{
        struct cling_error *self = error_create(CL_EINCTYPE, scanner, context);
        self->inctype.expected = cling_type_tostring(expected_type);
        self->inctype.actual = cling_type_tostring(actual_type);
        return self;
}

struct cling_error * cling_argc_unmatched_error(struct cling_scanner const *scanner, char const *func_name, unsigned int actual_argc, unsigned int expected_argc, size_t context) 
{
        struct cling_error *self = error_create(CL_EARGCUNMAT, scanner, context);
        self->argc_unmat.expected = expected_argc;
        self->argc_unmat.actual = actual_argc;
        self->argc_unmat.func_name = strdup(func_name);
        return self;
}

struct cling_error * cling_invalid_expr_error(struct cling_scanner const *scanner, struct utillib_json_value const *value, size_t context) 
{
        struct cling_error *self = error_create(CL_EINVEXPR, scanner, context);
        self->invexpr.expr = cling_ast_pretty_expr(value);
        return self;
}

struct cling_error *cling_dupcase_error(struct cling_scanner const *scanner, int case_type, char const *label, size_t context) 
{
        struct cling_error *self = error_create(CL_EDUPCASE, scanner, context);
        self->dupcase.case_type=case_type;
        self->dupcase.label = strdup(label);
        return self;
}

struct cling_error *cling_badtoken_error(struct cling_scanner const *scanner, int type, char const *evidence, size_t context) 
{
        struct cling_error *self = error_create(CL_EBADTOKEN, scanner, context);
        self->badtoken.type = type;
        self->badtoken.evidence = strdup(evidence);
        return self;
}

void cling_error_print(struct cling_error const *self) 
{

        fprintf(stderr, "ERROR at %lu:%lu in %s: ", positive_number(self->row),
                        positive_number(self->col), self->context);

        switch (self->kind) {
                case CL_EEXPECT:
                        fprintf(stderr, "expects '%s', but actually gets '%s'\n",
                                        self->expected.expected, self->expected.actual);
                        break;
                case CL_EUNEXPECTED:
                        fprintf(stderr, "unexpected toke '%s'\n", self->unexpected.unexpected);
                        break;
                case CL_EREDEFINED:
                        /*
                         * TODO: print previous definition.
                         */
                        fprintf(stderr, "name '%s' was redefined\n", self->redefined.name);
                        break;
                case CL_EUNDEFINED:
                        fprintf(stderr, "undefined reference to '%s'\n", self->undefined.name);
                        break;
                case CL_ENOTLVALUE:
                        fprintf(stderr, "'%s' is not lvalue\n", self->not_lvalue.value);
                        break;
                case CL_EARGCUNMAT:
                        fprintf(stderr, "function '%s' expects %u arguments but %u were given\n",
                                        self->argc_unmat.func_name, self->argc_unmat.expected,
                                        self->argc_unmat.actual);
                        break;
                case CL_EINCTYPE:
                        fprintf(stderr, "incompatible type: expected '%s' but saw '%s'\n",
                                        self->inctype.expected, self->inctype.actual);
                        break;
                case CL_EINVEXPR:
                        fprintf(stderr, "'%s' is not allowed here\n", self->invexpr.expr);
                        break;
                case CL_EDUPCASE:
                        fprintf(stderr, "duplicated case label '%s' of type '%s'\n", self->dupcase.label,
                                        cling_symbol_kind_tostring(self->dupcase.case_type));
                        break;
                case CL_EBADTOKEN:
                        switch (self->badtoken.type) {
                                case CL_EUNKNOWN:
                                        fprintf(stderr, "insane char '%s' in the program\n",
                                                        self->badtoken.evidence);
                                        break;
                                case CL_EBADNEQ:
                                        fprintf(stderr, "a single exclaim (%s) without equal makes no sense\n",
                                                        self->badtoken.evidence);
                                        break;
                                case CL_ECHRCHAR:
                                        fprintf(stderr, "unrecogized char '%s' in char constant\n",
                                                        self->badtoken.evidence);
                                        break;
                                case CL_ESTRCHAR:
                                        fprintf(stderr, "unrecogized char '%s' in string constant\n",
                                                        self->badtoken.evidence);
                                        break;
                                case CL_ELEADZERO:
                                        fprintf(stderr, "leading zero (%s) in unsigned integer is not allowed\n",
                                                        self->badtoken.evidence);
                                        break;
                                case CL_EUNTCHAR:
                                        fprintf(stderr, "unterminated char constant '%s'\n", self->badtoken.evidence);
                                        break;
                                case CL_EUNTSTR:
                                        fprintf(stderr, "unterminated string constant '%s'\n", self->badtoken.evidence);
                                        break;
                                default:
                                        assert(false);
                        }
                        break;
                default:
                        assert(false);
        }
}

void cling_error_destroy(struct cling_error *self) 
{
        switch (self->kind) {
                case CL_EREDEFINED:
                        free(self->redefined.name);
                        break;
                case CL_EUNDEFINED:
                        free(self->undefined.name);
                        break;
                case CL_ENOTLVALUE:
                        free(self->not_lvalue.value);
                        break;
                case CL_EARGCUNMAT:
                        free(self->argc_unmat.func_name);
                        break;
                case CL_EINVEXPR:
                        free(self->invexpr.expr);
                        break;
                case CL_EDUPCASE:
                        free(self->dupcase.label);
                        break;
                case CL_EBADTOKEN:
                        free(self->badtoken.evidence);
                        break;
        }
        free(self);
}
