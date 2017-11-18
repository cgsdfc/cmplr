
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
#include "symbols.h"
#include <assert.h>

UTILLIB_SYMBOL_BEGIN(cling_symbols)

UTILLIB_SYMBOL_NON_TERMINAL(SYM_PROGRAM)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_ADD_OP)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_REL_OP)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_MUL_OP)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_CONSTANT)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_TYPENAME)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_CONST_DEF)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_CONST_DEF_INT)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_CONST_DEF_CHAR)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_VAR_DECL)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_VAR_DEF)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_CONST_DECL)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_FUNC_DECL)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_MAIN_DECL)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_DECL_HD)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_NARGS)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_NARGS_LS)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_AARGS)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_AARGS_LS)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_IF_STMT)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_FOR_STMT)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_FOR_INIT)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_FOR_STEP)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_SWITCH_STMT)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_COMP_STMT)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_PRINTF_STMT)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_PRINTF_ARGS)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_PRINTF_ARGS_)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_SCANF_STMT)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_ASSIGN_STMT)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_RETURN_STMT)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_EXPR_OPT)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_INVOKE_STMT)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_ELSE_CLAUSE)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_DEFAULT_CLAUSE)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_CASE_CLAUSE)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_STMT)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_SIGN)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_EXPR)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_EXPR_)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_COND)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_TERM)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_FACT)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_EXTEND)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_CONST_DEF_LS)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_CONST_DEF_INT_LS)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_CONST_DEF_CHAR_LS)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_VAR_DEF_LS)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_FUNC_DECL_LS)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_CASE_CLAUSE_LS)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_TERM_LS)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_FACT_LS)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_EXPR_LS)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_IDEN_LS)
UTILLIB_SYMBOL_NON_TERMINAL(SYM_STMT_LS)

UTILLIB_SYMBOL_TERMINAL(SYM_UINT)
UTILLIB_SYMBOL_TERMINAL(SYM_INTEGER)
UTILLIB_SYMBOL_TERMINAL(SYM_STRING)
UTILLIB_SYMBOL_TERMINAL(SYM_CHAR)
UTILLIB_SYMBOL_TERMINAL(SYM_KW_INT)
UTILLIB_SYMBOL_TERMINAL(SYM_KW_CHAR)
UTILLIB_SYMBOL_TERMINAL(SYM_KW_VOID)
UTILLIB_SYMBOL_TERMINAL(SYM_KW_CONST)
UTILLIB_SYMBOL_TERMINAL(SYM_KW_IF)
UTILLIB_SYMBOL_TERMINAL(SYM_KW_FOR)
UTILLIB_SYMBOL_TERMINAL(SYW_KW_SWITCH)
UTILLIB_SYMBOL_TERMINAL(SYM_KW_MAIN)
UTILLIB_SYMBOL_TERMINAL(SYM_KW_ELSE)
UTILLIB_SYMBOL_TERMINAL(SYM_KW_CASE)
UTILLIB_SYMBOL_TERMINAL(SYM_KW_DEFAULT)
UTILLIB_SYMBOL_TERMINAL(SYM_KW_RETURN)
UTILLIB_SYMBOL_TERMINAL(SYM_KW_SCANF)
UTILLIB_SYMBOL_TERMINAL(SYM_KW_PRINTF)
UTILLIB_SYMBOL_TERMINAL(SYM_COLON)
UTILLIB_SYMBOL_TERMINAL(SYM_DEQ)
UTILLIB_SYMBOL_TERMINAL(SYM_ADD)
UTILLIB_SYMBOL_TERMINAL(SYM_MUL)
UTILLIB_SYMBOL_TERMINAL(SYM_RB)
UTILLIB_SYMBOL_TERMINAL(SYM_NE)
UTILLIB_SYMBOL_TERMINAL(SYM_SEMI)
UTILLIB_SYMBOL_TERMINAL(SYM_IDEN)
UTILLIB_SYMBOL_TERMINAL(SYM_EQ)
UTILLIB_SYMBOL_TERMINAL(SYM_LK)
UTILLIB_SYMBOL_TERMINAL(SYM_RK)
UTILLIB_SYMBOL_TERMINAL(SYM_LB)
UTILLIB_SYMBOL_TERMINAL(SYM_LT)
UTILLIB_SYMBOL_TERMINAL(SYM_LE)
UTILLIB_SYMBOL_TERMINAL(SYM_GT)
UTILLIB_SYMBOL_TERMINAL(SYM_GE)
UTILLIB_SYMBOL_TERMINAL(SYM_DIV)
UTILLIB_SYMBOL_TERMINAL(SYM_MINUS)
UTILLIB_SYMBOL_TERMINAL(SYM_COMMA)
UTILLIB_SYMBOL_TERMINAL(SYM_LP)
UTILLIB_SYMBOL_TERMINAL(SYM_RP)

UTILLIB_SYMBOL_END(cling_symbols);

