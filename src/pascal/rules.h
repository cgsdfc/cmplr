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
#ifndef PASCAL_RULES_H
#define PASCAL_RULES_H
#include <utillib/rule.h>
extern struct utillib_rule_literal const pascal_rules[];

/* program := subprogram */
/* subprogram := [ const_decl ] [ var_decl ] [ proc_decl ] stmt */
/* const_decl := 'const' const_def {',' const_def } ';' */
/* const_def := iden '=' uint */
/* var_decl := 'var' iden {, iden } ';' */
/* proc_decl := proc_head subprogram { ';' subprogram } ';' */
/* proc_head := 'procedure' iden ';' */
/* stmt := assign | cond | loop | call | composite | read | write | eps */
/* assign := iden ':=' expr */
/* expr := ['+'|'-'] term { add_op term } */
/* term := factor { mul_op factor } */
/* factor := iden | uint | '(' expr ')' */
/* add_op := '+' | '-' */
/* mul_op := '*' | '/' */
/* condition := expr rel_op expr | 'odd' expr */
/* rel_op := '<>' | '<' | '>' | '<=' | '>=' */
/* cond := 'if' condition 'then' stmt */
/* loop := 'while' condition 'do' stmt */
/* call := 'call' iden */
/* composite := 'begin' stmt {; stmt} 'end' */
/* read := 'read' '(' iden { , iden } ')' */
/* write := 'write' '(' expr {, expr } ')' */
#endif /* PASCAL_RULES_H */
