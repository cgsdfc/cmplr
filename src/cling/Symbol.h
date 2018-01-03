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
#ifndef CLING_SYMBOL_HXX
#define CLING_SYMBOL_HXX
#include "Grammar.h"
char const *SymbolString(int symbol);
#define LEMON_SYM_MAX 18

enum {
  SYM_EOF=LEMON_SYM_MAX,
  SYM_PROGRAM,
  SYM_ADD_OP,
  SYM_REL_OP,
  SYM_MUL_OP,
  SYM_CONSTANT,
  SYM_TYPENAME,
  SYM_CONST_DEF,
  SYM_VAR_DECL,
  SYM_VAR_DEF,
  SYM_CONST_DECL,
  SYM_FUNC_DECL,
  SYM_MAIN_DECL,
  SYM_NARGS,
  SYM_AARGS,
  SYM_IF_STMT,
  SYM_FOR_STMT,
  SYM_FOR_INIT,
  SYM_FOR_STEP,
  SYM_SWITCH_STMT,
  SYM_COMP_STMT,
  SYM_PRINTF_STMT,
  SYM_SCANF_STMT,
  SYM_EXPR_STMT,
  SYM_RETURN_STMT,
  SYM_INVOKE_STMT,
  SYM_ELSE_CLAUSE,
  SYM_DEFAULT_CLAUSE,
  SYM_CASE_CLAUSE,
  SYM_STMT,
  SYM_EXPR,
  SYM_CONDITION,

  SYM_STRING,

  SYM_KW_VOID,
  SYM_KW_IF,
  SYM_KW_FOR,
  SYM_KW_SWITCH,
  SYM_KW_MAIN,
  SYM_KW_ELSE,
  SYM_KW_CASE,
  SYM_KW_DEFAULT,
  SYM_KW_RETURN,
  SYM_KW_SCANF,
  SYM_KW_PRINTF,

  SYM_COLON,

  SYM_MUL,
  SYM_NE,
  SYM_DEQ,
  SYM_LT,
  SYM_LE,
  SYM_GT,
  SYM_GE,
  SYM_DIV,
  SYM_ERR,
};

#endif
