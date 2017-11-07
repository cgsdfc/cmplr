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
#include "rules.h"
#include "symbols.h"

UTILLIB_RULE_BEGIN(pascal_rules)
/* program := subprogram */
UTILLIB_RULE_ELEM(SYM_PROGRAM, SYM_SUBPRG)
/* subprogram := [ const_decl ] [ var_decl ] [ proc_decl ] stmt */
UTILLIB_RULE_ELEM(SYM_SUBPRG, SYM_CONST_DECL, SYM_VAR_DECL, SYM_PROC_DECL,
                  SYM_STMT)
/* const_decl := 'const' const_def {',' const_def } ';' */
UTILLIB_RULE_ELEM(SYM_CONST_DECL, SYM_KW_CONST, SYM_CONST_DEF, SYM_CONST_DEF_LS,
                  SYM_SEMI)
/* const_def := iden '=' uint */
UTILLIB_RULE_ELEM(SYM_CONST_DEF, SYM_IDEN, SYM_EQ, SYM_UINT)
/* var_decl := 'var' iden {, iden } ';' */
UTILLIB_RULE_ELEM(SYM_VAR_DECL, SYM_KW_VAR, SYM_IDEN, SYM_IDEN_LS, SYM_SEMI)
/* proc_decl := proc_head subprogram { ';' subprogram } ';' */
UTILLIB_RULE_ELEM(SYM_PROC_DECL, SYM_PROC_HD, SYM_SUBPRG, SYM_PROC_DECL_LS,
                  SYM_SEMI)
/* proc_head := 'procedure' iden ';' */
UTILLIB_RULE_ELEM(SYM_PROC_HD, SYM_KW_PROC, SYM_IDEN, SYM_SEMI)
/* stmt := assign | cond | loop | call | composite | read | write | eps */
UTILLIB_RULE_ELEM(SYM_STMT, SYM_ASS_STMT)
UTILLIB_RULE_ELEM(SYM_STMT, SYM_COND_STMT)
UTILLIB_RULE_ELEM(SYM_STMT, SYM_LOOP_STMT)
UTILLIB_RULE_ELEM(SYM_STMT, SYM_CALL_STMT)
UTILLIB_RULE_ELEM(SYM_STMT, SYM_COMP_STMT)
UTILLIB_RULE_ELEM(SYM_STMT, SYM_READ_STMT)
UTILLIB_RULE_ELEM(SYM_STMT, SYM_WRITE_STMT)
UTILLIB_RULE_ELEM(SYM_STMT, UT_SYM_EPS)
/* assign := iden ':=' expr */
UTILLIB_RULE_ELEM(SYM_ASS_STMT, SYM_IDEN, SYM_CEQ, SYM_EXPR)
/* expr := ['+'|'-'] term { add_op term } */
UTILLIB_RULE_ELEM(SYM_EXPR, SYM_SIGN, SYM_TERM, SYM_TERM_LS)
/* term := factor { mul_op factor } */
UTILLIB_RULE_ELEM(SYM_TERM, SYM_FACT, SYM_FACT_LS)
/* factor := iden | uint | '(' expr ')' */
UTILLIB_RULE_ELEM(SYM_FACT, SYM_IDEN)
UTILLIB_RULE_ELEM(SYM_FACT, SYM_UINT)
UTILLIB_RULE_ELEM(SYM_FACT, SYM_LP, SYM_EXPR, SYM_RP)
/* add_op := '+' | '-' */
UTILLIB_RULE_ELEM(SYM_ADD_OP, SYM_ADD)
UTILLIB_RULE_ELEM(SYM_ADD_OP, SYM_MINUS)
/* mul_op := '*' | '/' */
UTILLIB_RULE_ELEM(SYM_MUL_OP, SYM_MUL)
UTILLIB_RULE_ELEM(SYM_MUL_OP, SYM_DIV)
/* condition := expr rel_op expr | 'odd' expr */
UTILLIB_RULE_ELEM(SYM_COND, SYM_EXPR, SYM_REL_OP, SYM_EXPR)
UTILLIB_RULE_ELEM(SYM_COND, SYM_KW_ODD, SYM_EXPR)
/* rel_op := '<>' | '==' | '<' | '>' | '<=' | '>=' */
UTILLIB_RULE_ELEM(SYM_REL_OP, SYM_NE)
UTILLIB_RULE_ELEM(SYM_REL_OP, SYM_DEQ)
UTILLIB_RULE_ELEM(SYM_REL_OP, SYM_LT)
UTILLIB_RULE_ELEM(SYM_REL_OP, SYM_LE)
UTILLIB_RULE_ELEM(SYM_REL_OP, SYM_GT)
UTILLIB_RULE_ELEM(SYM_REL_OP, SYM_GE)
/* cond := 'if' condition 'then' stmt */
UTILLIB_RULE_ELEM(SYM_COND_STMT, SYM_KW_IF, SYM_COND, SYM_KW_THEN, SYM_STMT)
/* loop := 'while' condition 'do' stmt */
UTILLIB_RULE_ELEM(SYM_LOOP_STMT, SYM_KW_WHILE, SYM_COND, SYM_KW_DO, SYM_STMT)
/* call := 'call' iden */
UTILLIB_RULE_ELEM(SYM_CALL_STMT, SYM_KW_CALL, SYM_IDEN)
/* composite := 'begin' stmt 'end' */
UTILLIB_RULE_ELEM(SYM_COMP_STMT, SYM_KW_BEGIN, SYM_STMT, SYM_STMT_LS,
                  SYM_KW_END)
/* read := 'read' '(' iden { , iden } ')' */
UTILLIB_RULE_ELEM(SYM_READ_STMT, SYM_KW_READ, SYM_LP, SYM_IDEN, SYM_IDEN_LS,
                  SYM_RP)
/* write := 'write' '(' expr {, expr } ')' */
UTILLIB_RULE_ELEM(SYM_WRITE_STMT, SYM_KW_WRITE, SYM_LP, SYM_EXPR, SYM_EXPR_LS,
                  SYM_RP)

/* Make up for '[]' */
UTILLIB_RULE_ELEM(SYM_CONST_DECL, UT_SYM_EPS)
UTILLIB_RULE_ELEM(SYM_VAR_DECL, UT_SYM_EPS)
UTILLIB_RULE_ELEM(SYM_PROC_DECL, UT_SYM_EPS)
UTILLIB_RULE_ELEM(SYM_SIGN, UT_SYM_EPS)

/* Make up for '{}' */
UTILLIB_RULE_ELEM(SYM_CONST_DEF_LS, UT_SYM_EPS)
UTILLIB_RULE_ELEM(SYM_CONST_DEF_LS, SYM_COMMA, SYM_CONST_DEF, SYM_CONST_DEF_LS)
UTILLIB_RULE_ELEM(SYM_IDEN_LS, UT_SYM_EPS)
UTILLIB_RULE_ELEM(SYM_IDEN_LS, SYM_COMMA, SYM_IDEN, SYM_IDEN_LS)
UTILLIB_RULE_ELEM(SYM_PROC_DECL_LS, SYM_SEMI, SYM_PROC_DECL, SYM_PROC_DECL_LS)
UTILLIB_RULE_ELEM(SYM_SIGN, SYM_ADD)
UTILLIB_RULE_ELEM(SYM_SIGN, SYM_MINUS)
UTILLIB_RULE_ELEM(SYM_EXPR_LS, SYM_COMMA, SYM_EXPR, SYM_EXPR_LS)
UTILLIB_RULE_ELEM(SYM_TERM_LS, SYM_ADD_OP, SYM_TERM, SYM_TERM_LS)
UTILLIB_RULE_ELEM(SYM_FACT_LS, SYM_MUL_OP, SYM_FACT, SYM_FACT_LS)

UTILLIB_RULE_END(pascal_rules);
