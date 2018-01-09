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
#ifndef CLING_ERROR_H
#define CLING_ERROR_H
#include <utillib/json.h>

enum {
        CL_EEXPECT = 1,
        CL_EUNEXPECTED,
        CL_EREDEFINED,
        CL_EINCTYPE,
        CL_EUNDEFINED,
        CL_ENOTLVALUE,
        CL_EINCARG,
        CL_EARGCUNMAT,
        CL_EDUPCASE,
        CL_EBADCASE,
        CL_EINVEXPR,
        CL_EBADTOKEN,
};

/*
 * Badtoken kind
 */
enum {
        CL_EBADNEQ = 1,
        CL_EUNTCHAR,
        CL_EUNTSTR,
        CL_ESTRCHAR,
        CL_ECHRCHAR,
        CL_ELEADZERO,
        CL_EUNKNOWN,
};

struct cling_scanner;
struct cling_error 
{
        int kind;
        char const *context;
        size_t row;
        size_t col;
        union {
                struct {
                        char *name;
                } redefined;
                struct {
                        char *name;
                } undefined;
                struct {
                        char *value;
                } not_lvalue;
                struct {
                        char *func_name;
                        unsigned int expected;
                        unsigned int actual;
                } argc_unmat;
                struct {
                        char *expr;
                } invexpr;
                struct {
                        char const *expected;
                        char const *actual;
                } expected;
                struct {
                        char const *unexpected;
                } unexpected;
                struct {
                        char const *expected;
                        char const *actual;
                } inctype;
                struct {
                        int case_type;
                        char *label;
                } dupcase;
                struct {
                        int type;
                        char *evidence;
                } badtoken;
        };
};

void cling_error_destroy(struct cling_error *self);

struct cling_error *cling_expected_error(struct cling_scanner const *scanner,
                size_t expected, size_t context);

struct cling_error *cling_unexpected_error(struct cling_scanner const *scanner,
                size_t context);

struct cling_error *cling_redefined_error(struct cling_scanner const *scanner,
                char const *name, size_t context);

struct cling_error *
cling_undefined_name_error(struct cling_scanner const *scanner,
                char const *name, size_t context);

struct cling_error *
cling_not_lvalue_error(struct cling_scanner const *scanner,
                struct utillib_json_value const *value, size_t context);

struct cling_error *
cling_incompatible_type_error(struct cling_scanner const *scanner,
                int actual_type, int expected_type,
                size_t context);
struct cling_error *
cling_invalid_expr_error(struct cling_scanner const *scanner,
                struct utillib_json_value const *value,
                size_t context);

struct cling_error *
cling_argc_unmatched_error(struct cling_scanner const *scanner,
                char const *func_name, unsigned int actual_argc,
                unsigned int expected_argc, size_t context);

struct cling_error *cling_dupcase_error(struct cling_scanner const *scanner,
                int case_type, char const *label,
                size_t context);
struct cling_error *cling_badtoken_error(struct cling_scanner const *scanner,
                int type, char const *evidence,
                size_t context);

void cling_error_print(struct cling_error const *self);

#endif /* CLING_ERROR_H */
