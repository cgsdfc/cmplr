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
extern size_t const pascaL_firsts[][15];
extern size_t const pascaL_follows[][15];

/* program := subprogram
 * first(program) = 'const' 'var' 'procedure'
 * iden 'if' 'while' 'call' 'begin' 'read' 'write' eps
 * follow(program) := eof
 * # OK
 */

/* subprogram := [ const_decl ] [ var_decl ] [ proc_decl ] stmt
 * first(subprogram) = 'const' 'var' 'procedure'
 * iden 'if' 'while' 'call' 'begin' 'read' 'write' eps
 * follow(subprogram) = ';' eof
 * # union first(stmt).
 * # FXXK
 */

/* const_decl := 'const' const_def {',' const_def } ';' | eps
 * first(const_decl) = 'const' eps
 * follow(const_decl) = 'var' 'procedure'
 * iden 'if' 'while' 'call' 'begin' 'read' 'write'
 * eof
 * # OK
 */

/* const_def := iden '=' uint | eps
 * first(const_def) = iden eps
 * follow(const_def) = ',' ';'
 * # OK
 */

/* var_decl := 'var' iden {, iden } ';'
 * first(var_decl) = var
 * follow(var_decl) = 'procedure'
 * iden 'if' 'while' 'call' 'begin' 'read' 'write'
 * eof
 * # OK
 */

/* proc_head := 'procedure' iden ';'
 * first(proc_head) = 'procedure'
 * follow(proc_head) =
 * 'const' 'var' 'procedure'
 * iden 'if' 'while' 'call' 'begin' 'read' 'write' eof
 */

/* proc_decl := proc_head subprogram { ';' subprogram } ';'
 * first(proc_decl) = 'procedure' eps
 * follow(proc_decl) =
 * iden 'if' 'while' 'call' 'begin' 'read' 'write'
 * eof
 * # OK
 */

/* stmt := assign | cond | loop | call | composite | read | write | eps */
/* first(stmt) = iden 'if' 'while' 'call' 'begin' 'read' 'write' eps
 * follow(stmt) = eof ';' 'end' # WRONG
 */

/* assign := iden ':=' expr
 * first(assign) = iden
 * follow(assign) = ';' eof 'end'
 */

/* expr := ['+'|'-'] term { add_op term } */
/* first(expr) = '+' '-' iden uint '('
 * follow(expr) = ')'  '<>'  '<'  '>'  '<='  '>=' ','
 * '='
 * ';' eof 'end'
 *
 */

/* term := factor { mul_op factor }
 * first(term) = iden uint '('
 * follow(term) = '+' '-'
 * ';' eof 'end'
 *
 */

/* factor := iden | uint | '(' expr ')'
 * first(factor) = iden uint '('
 * follow(factor) = '*' '/'
 * ';' eof 'end'
 */

/* add_op := '+' | '-'
 * first(add_op) = '+', '-'
 * follow(add_op)=iden uint '('
 */

/* mul_op := '*' | '/'
 * first(mul_op) = '*', '/'
 * follow(mul_op) = iden uint '('
 */

/* condition := expr rel_op expr | 'odd' expr
 * first(condition) = 'odd' iden uint '('
 * follow(condition) = 'do' 'then'
 */

/* rel_op := '<>' | '<' | '>' | '<=' | '>=' | '='
 * first(rel_op) = '<>'  '<'  '>'  '<='  '>='  | '='
 * follow(rel_op) = iden uint '('
 */

/* cond := 'if' condition 'then' stmt
 * first(cond) = 'if'
 * follow(cond) = eof ';' 'end'
 */

/* loop := 'while' condition 'do' stmt
 * first(loop) = 'while'
 * follow(loop) = eof ';' 'end'
 */

/* call := 'call' iden
 * first(call) = 'call'
 * follow(call) = eof ';' end
 */

/* composite := 'begin' stmt {; stmt} 'end'
 * first(composite) = 'begin'
 * follow(composite) = 'end' ';' eof
 */

/* read := 'read' '(' iden { , iden } ')'
 * first(read) = 'read'
 * follow(read) = eof 'end' ';'
 *  */

/* write := 'write' '(' expr {, expr } ')'
 * first(write) = 'write'
 * follow(read) = eof 'end' ';'
 */

/* end of rules */

#endif /* PASCAL_RULES_H */
