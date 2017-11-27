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
#ifndef CLING_SYMBOLS_H
#define CLING_SYMBOLS_H
#include <utillib/enum.h>
#include <utillib/symbol.h>

#define cling_symbol_cast(code)                                                \
  ((code) == UT_SYM_EOF ? UTILLIB_SYMBOL_EOF : cling_symbol_kind_tostring((code)))

UTILLIB_ENUM_BEGIN(cling_symbol_kind)

UTILLIB_ENUM_ELEM_INIT(SYM_PROGRAM, 1)
UTILLIB_ENUM_ELEM(SYM_ADD_OP)
UTILLIB_ENUM_ELEM(SYM_REL_OP)
UTILLIB_ENUM_ELEM(SYM_MUL_OP)
UTILLIB_ENUM_ELEM(SYM_CONSTANT)
UTILLIB_ENUM_ELEM(SYM_TYPENAME)
UTILLIB_ENUM_ELEM(SYM_CONST_DEF)
UTILLIB_ENUM_ELEM(SYM_VAR_DECL)
UTILLIB_ENUM_ELEM(SYM_VAR_DEF)
UTILLIB_ENUM_ELEM(SYM_CONST_DECL)
UTILLIB_ENUM_ELEM(SYM_FUNC_DECL)
UTILLIB_ENUM_ELEM(SYM_MAIN_DECL)
UTILLIB_ENUM_ELEM(SYM_DECL_HD)
UTILLIB_ENUM_ELEM(SYM_NARGS)
UTILLIB_ENUM_ELEM(SYM_AARGS)
UTILLIB_ENUM_ELEM(SYM_IF_STMT)
UTILLIB_ENUM_ELEM(SYM_FOR_STMT)
UTILLIB_ENUM_ELEM(SYM_FOR_INIT)
UTILLIB_ENUM_ELEM(SYM_FOR_STEP)
UTILLIB_ENUM_ELEM(SYM_SWITCH_STMT)
UTILLIB_ENUM_ELEM(SYM_COMP_STMT)
UTILLIB_ENUM_ELEM(SYM_PRINTF_STMT)
UTILLIB_ENUM_ELEM(SYM_PRINTF_ARGS)
UTILLIB_ENUM_ELEM(SYM_PRINTF_ARGS_)
UTILLIB_ENUM_ELEM(SYM_SCANF_STMT)
UTILLIB_ENUM_ELEM(SYM_ASSIGN_STMT)
UTILLIB_ENUM_ELEM(SYM_RETURN_STMT)
UTILLIB_ENUM_ELEM(SYM_EXPR_OPT)
UTILLIB_ENUM_ELEM(SYM_INVOKE_STMT)
UTILLIB_ENUM_ELEM(SYM_ELSE_CLAUSE)
UTILLIB_ENUM_ELEM(SYM_DEFAULT_CLAUSE)
UTILLIB_ENUM_ELEM(SYM_CASE_CLAUSE)
UTILLIB_ENUM_ELEM(SYM_STMT)
UTILLIB_ENUM_ELEM(SYM_EXPR)
UTILLIB_ENUM_ELEM(SYM_COND)
UTILLIB_ENUM_ELEM(SYM_TERM)
UTILLIB_ENUM_ELEM(SYM_FACT)

UTILLIB_ENUM_ELEM(SYM_UINT)
UTILLIB_ENUM_ELEM(SYM_INTEGER)
UTILLIB_ENUM_ELEM(SYM_STRING)
UTILLIB_ENUM_ELEM(SYM_CHAR)

UTILLIB_ENUM_ELEM(SYM_KW_INT)
UTILLIB_ENUM_ELEM(SYM_KW_CHAR)
UTILLIB_ENUM_ELEM(SYM_KW_VOID)
UTILLIB_ENUM_ELEM(SYM_KW_CONST)
UTILLIB_ENUM_ELEM(SYM_KW_IF)
UTILLIB_ENUM_ELEM(SYM_KW_FOR)
UTILLIB_ENUM_ELEM(SYW_KW_SWITCH)
UTILLIB_ENUM_ELEM(SYM_KW_MAIN)
UTILLIB_ENUM_ELEM(SYM_KW_ELSE)
UTILLIB_ENUM_ELEM(SYM_KW_CASE)
UTILLIB_ENUM_ELEM(SYM_KW_DEFAULT)
UTILLIB_ENUM_ELEM(SYM_KW_RETURN)
UTILLIB_ENUM_ELEM(SYM_KW_SCANF)
UTILLIB_ENUM_ELEM(SYM_KW_PRINTF)

UTILLIB_ENUM_ELEM(SYM_COLON)
UTILLIB_ENUM_ELEM(SYM_DEQ)
UTILLIB_ENUM_ELEM(SYM_ADD)
UTILLIB_ENUM_ELEM(SYM_MUL)
UTILLIB_ENUM_ELEM(SYM_RB)
UTILLIB_ENUM_ELEM(SYM_NE)
UTILLIB_ENUM_ELEM(SYM_SEMI)
UTILLIB_ENUM_ELEM(SYM_IDEN)
UTILLIB_ENUM_ELEM(SYM_EQ)
UTILLIB_ENUM_ELEM(SYM_LK)
UTILLIB_ENUM_ELEM(SYM_RK)
UTILLIB_ENUM_ELEM(SYM_LB)
UTILLIB_ENUM_ELEM(SYM_LT)
UTILLIB_ENUM_ELEM(SYM_LE)
UTILLIB_ENUM_ELEM(SYM_GT)
UTILLIB_ENUM_ELEM(SYM_GE)
UTILLIB_ENUM_ELEM(SYM_DIV)
UTILLIB_ENUM_ELEM(SYM_MINUS)
UTILLIB_ENUM_ELEM(SYM_COMMA)
UTILLIB_ENUM_ELEM(SYM_LP)
UTILLIB_ENUM_ELEM(SYM_RP)

UTILLIB_ENUM_END(cling_symbol_kind);


#endif /* CLING_SYMBOLS_H */
