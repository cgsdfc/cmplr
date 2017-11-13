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

#include "pascal/parser.h"
#include "pascal/symbols.h"
#include <utillib/scanner.h>
#include <utillib/test.h>

static struct utillib_symbol_scanner symbol_scanner;

UTILLIB_TEST_SET_UP() { pascal_parser_init(UT_FIXTURE); }

UTILLIB_TEST_TEAR_DOWN() { pascal_parser_destroy(UT_FIXTURE); }

UTILLIB_TEST_AUX(parser_assert_accepted, size_t const *input) {
  utillib_symbol_scanner_init(&symbol_scanner, input, pascal_symbols);
  bool asserted= pascal_parser_parse(UT_FIXTURE, &symbol_scanner,
                                 &utillib_symbol_scanner_op);
  UTILLIB_TEST_ASSERT(asserted);
}

UTILLIB_TEST_AUX(parser_assert_rejected, size_t const * input) {
  utillib_symbol_scanner_init(&symbol_scanner, input, pascal_symbols);
  bool asserted= pascal_parser_parse(UT_FIXTURE, &symbol_scanner,
                                 &utillib_symbol_scanner_op);
  UTILLIB_TEST_ASSERT_FALSE(asserted);
}


UTILLIB_TEST(parser_empty_input) {
  size_t const empty_input[] = {
      UT_SYM_EOF,
  };
  UTILLIB_TEST_AUX_INVOKE(parser_assert_accepted, empty_input);
}

UTILLIB_TEST(parser_const_decl) {
  /*
   * const_decl := 'const' const_def {, const_def } ';'
   * const_def := iden '=' uint
   */
  UTILLIB_TEST_MESSAGE("Single const_decl instance");
  /* const bar=1; */
  size_t const const_decl_input[] = {SYM_KW_CONST, SYM_IDEN, SYM_EQ,
                                     SYM_UINT,     SYM_SEMI,  UT_SYM_EOF};
  UTILLIB_TEST_AUX_INVOKE(parser_assert_accepted , const_decl_input);
}

UTILLIB_TEST(parser_const_decl_multi)
{
  /* const boo=12, foo=14, cat=13 ; */
  size_t const const_decl_input_multi[] = {
    SYM_KW_CONST, SYM_IDEN, SYM_EQ, SYM_UINT,
    SYM_COMMA, SYM_IDEN, SYM_EQ, SYM_UINT,
    SYM_COMMA, SYM_IDEN, SYM_EQ, SYM_UINT, SYM_SEMI, UT_SYM_EOF
  };

  UTILLIB_TEST_MESSAGE("Multiple const_decl");
  UTILLIB_TEST_AUX_INVOKE(parser_assert_accepted, const_decl_input_multi);
}

UTILLIB_TEST(parser_const_decl_reject)
{
  /* const foo ; */
  size_t const const_decl_input_bad_1[] = {
    SYM_KW_CONST, SYM_IDEN, SYM_SEMI , UT_SYM_EOF 
  };
  UTILLIB_TEST_AUX_INVOKE(parser_assert_rejected, const_decl_input_bad_1);
}

UTILLIB_TEST(parser_var_decl)
{
  /* var foo ; */
  size_t const var_decl_input[]={
    SYM_KW_VAR, SYM_IDEN, SYM_SEMI, UT_SYM_EOF ,
  };
  UTILLIB_TEST_AUX_INVOKE(parser_assert_accepted, var_decl_input);
}

UTILLIB_TEST(parser_var_decl_multi)
{
  /* var foo ; */
  size_t const var_decl_input[]={
    SYM_KW_VAR, SYM_IDEN, SYM_COMMA,
    SYM_IDEN, SYM_COMMA,
    SYM_IDEN, SYM_SEMI, UT_SYM_EOF ,
  };
  UTILLIB_TEST_AUX_INVOKE(parser_assert_accepted, var_decl_input);
}

UTILLIB_TEST(parser_var_decl_reject)
{
 size_t const var_decl_input[]={
   SYM_KW_VAR, SYM_IDEN, SYM_EQ, SYM_UINT ,
   UT_SYM_EOF
 };
 UTILLIB_TEST_AUX_INVOKE(parser_assert_rejected, var_decl_input);
}

UTILLIB_TEST(parser_procedure_decl)
{
  /* procedure foo; begin end ; ; */
  size_t const procedure_input[]={
    SYM_KW_PROC, SYM_IDEN, SYM_SEMI, 
    SYM_KW_BEGIN, SYM_KW_END,  SYM_SEMI,
    UT_SYM_EOF
  };
  UTILLIB_TEST_AUX_INVOKE(parser_assert_accepted, procedure_input);
}

UTILLIB_TEST(parser_stmt_assign)
{
  /* bar := foo; */
  size_t const stmt_input[] = {
    SYM_IDEN, SYM_CEQ, SYM_IDEN, 
    UT_SYM_EOF
  };
  UTILLIB_TEST_AUX_INVOKE(parser_assert_accepted, stmt_input);
}

UTILLIB_TEST(parser_stmt_write) {
  size_t const stmt_input[] = {
    SYM_KW_WHILE, SYM_LP, SYM_IDEN, 
    SYM_COMMA, SYM_IDEN, SYM_RP,
    UT_SYM_EOF
  };
  UTILLIB_TEST_AUX_INVOKE(parser_stmt_write);
}
    
UTILLIB_TEST_DEFINE(Pascal_Parser) {
  UTILLIB_TEST_BEGIN(Pascal_Parser)
  UTILLIB_TEST_RUN(parser_const_decl)
  UTILLIB_TEST_RUN(parser_const_decl_multi)
  UTILLIB_TEST_RUN(parser_const_decl_reject)
  UTILLIB_TEST_RUN(parser_var_decl)
  UTILLIB_TEST_RUN(parser_var_decl_multi)
  UTILLIB_TEST_RUN(parser_var_decl_reject)
  UTILLIB_TEST_RUN(parser_procedure_decl)
  UTILLIB_TEST_RUN(parser_stmt_assign)
  UTILLIB_TEST_END(Pascal_Parser)
  UTILLIB_TEST_FIXTURE(struct pascal_parser)
  UTILLIB_TEST_RETURN(Pascal_Parser)
}