
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

UTILLIB_ETAB_BEGIN(cling_symbol_kind)

UTILLIB_ETAB_ELEM_INIT(SYM_PROGRAM, "program")
UTILLIB_ETAB_ELEM_INIT(SYM_ADD_OP, "additive operator")
UTILLIB_ETAB_ELEM_INIT(SYM_REL_OP, "relational operator")
UTILLIB_ETAB_ELEM_INIT(SYM_MUL_OP, "multiplicative operator")
UTILLIB_ETAB_ELEM_INIT(SYM_CONSTANT, "constant")
UTILLIB_ETAB_ELEM_INIT(SYM_TYPENAME, "typename")
UTILLIB_ETAB_ELEM_INIT(SYM_CONST_DEF, "constant declaration")
UTILLIB_ETAB_ELEM_INIT(SYM_VAR_DECL, "variable declaration")
UTILLIB_ETAB_ELEM_INIT(SYM_VAR_DEF, "variable definition")
UTILLIB_ETAB_ELEM_INIT(SYM_CONST_DECL, "constant definition")
UTILLIB_ETAB_ELEM_INIT(SYM_FUNC_DECL, "function declaration")
UTILLIB_ETAB_ELEM_INIT(SYM_MAIN_DECL, "main declaration")
UTILLIB_ETAB_ELEM_INIT(SYM_NARGS, "normal arguments")
UTILLIB_ETAB_ELEM_INIT(SYM_AARGS, "actual arguments")
UTILLIB_ETAB_ELEM_INIT(SYM_IF_STMT, "if statement")
UTILLIB_ETAB_ELEM_INIT(SYM_FOR_STMT, "for statement")
UTILLIB_ETAB_ELEM_INIT(SYM_FOR_INIT, "initilizing part of for statement")
UTILLIB_ETAB_ELEM_INIT(SYM_FOR_STEP, "steping part of for statement")
UTILLIB_ETAB_ELEM_INIT(SYM_STMT, "statement")
UTILLIB_ETAB_ELEM_INIT(SYM_SWITCH_STMT, "switch statement")
UTILLIB_ETAB_ELEM_INIT(SYM_COMP_STMT, "composite statement")
UTILLIB_ETAB_ELEM_INIT(SYM_PRINTF_STMT, "printf statement")
UTILLIB_ETAB_ELEM_INIT(SYM_SCANF_STMT, "scanf statement")
UTILLIB_ETAB_ELEM_INIT(SYM_EXPR_STMT, "expression statement")
UTILLIB_ETAB_ELEM_INIT(SYM_RETURN_STMT, "return statement")
UTILLIB_ETAB_ELEM_INIT(SYM_INVOKE_STMT, "invoke statement")
UTILLIB_ETAB_ELEM_INIT(SYM_ELSE_CLAUSE, "else clause")
UTILLIB_ETAB_ELEM_INIT(SYM_DEFAULT_CLAUSE, "default clause")
UTILLIB_ETAB_ELEM_INIT(SYM_CASE_CLAUSE, "case clause")
UTILLIB_ETAB_ELEM_INIT(SYM_EXPR, "expression")
UTILLIB_ETAB_ELEM_INIT(SYM_CONDITION, "condition")
UTILLIB_ETAB_ELEM_INIT(SYM_UINT, "unsigned integer")
UTILLIB_ETAB_ELEM_INIT(SYM_STRING, "string")
UTILLIB_ETAB_ELEM_INIT(SYM_CHAR, "character")

UTILLIB_ETAB_ELEM_INIT(SYM_KW_INT, "int")
UTILLIB_ETAB_ELEM_INIT(SYM_KW_CHAR, "char")
UTILLIB_ETAB_ELEM_INIT(SYM_KW_VOID, "void")
UTILLIB_ETAB_ELEM_INIT(SYM_KW_CONST, "const")
UTILLIB_ETAB_ELEM_INIT(SYM_KW_IF, "if")
UTILLIB_ETAB_ELEM_INIT(SYM_KW_FOR, "for")
UTILLIB_ETAB_ELEM_INIT(SYM_KW_SWITCH, "switch")
UTILLIB_ETAB_ELEM_INIT(SYM_KW_MAIN, "main")
UTILLIB_ETAB_ELEM_INIT(SYM_KW_ELSE, "else")
UTILLIB_ETAB_ELEM_INIT(SYM_KW_CASE, "case")
UTILLIB_ETAB_ELEM_INIT(SYM_KW_DEFAULT, "default")
UTILLIB_ETAB_ELEM_INIT(SYM_KW_RETURN, "return")
UTILLIB_ETAB_ELEM_INIT(SYM_KW_SCANF, "scanf")
UTILLIB_ETAB_ELEM_INIT(SYM_KW_PRINTF, "printf")

UTILLIB_ETAB_ELEM_INIT(SYM_COLON, ":")
UTILLIB_ETAB_ELEM_INIT(SYM_DEQ, "==")
UTILLIB_ETAB_ELEM_INIT(SYM_ADD, "+")
UTILLIB_ETAB_ELEM_INIT(SYM_MUL, "*")
UTILLIB_ETAB_ELEM_INIT(SYM_RB, "}")
UTILLIB_ETAB_ELEM_INIT(SYM_NE, "!=")
UTILLIB_ETAB_ELEM_INIT(SYM_SEMI, ";")
UTILLIB_ETAB_ELEM_INIT(SYM_IDEN, "identifier")
UTILLIB_ETAB_ELEM_INIT(SYM_EQ, "=")
UTILLIB_ETAB_ELEM_INIT(SYM_LK, "[")
UTILLIB_ETAB_ELEM_INIT(SYM_RK, "]")
UTILLIB_ETAB_ELEM_INIT(SYM_LB, "{")
UTILLIB_ETAB_ELEM_INIT(SYM_LT, "<")
UTILLIB_ETAB_ELEM_INIT(SYM_LE, "<=")
UTILLIB_ETAB_ELEM_INIT(SYM_GT, ">")
UTILLIB_ETAB_ELEM_INIT(SYM_GE, ">=")
UTILLIB_ETAB_ELEM_INIT(SYM_DIV, "/")
UTILLIB_ETAB_ELEM_INIT(SYM_MINUS, "-")
UTILLIB_ETAB_ELEM_INIT(SYM_COMMA, ",")
UTILLIB_ETAB_ELEM_INIT(SYM_LP, "(")
UTILLIB_ETAB_ELEM_INIT(SYM_RP, ")")

UTILLIB_ETAB_END(cling_symbol_kind);

bool opg_parser_is_relop(unsigned op) {
  return op == SYM_NE || op == SYM_DEQ || op == SYM_LT || op == SYM_LE ||
         op == SYM_GT || op == SYM_GE;
}
