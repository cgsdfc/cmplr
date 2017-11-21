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
#ifndef CLING_RULES_H
#define CLING_RULES_H
#include <utillib/rule.h>
extern struct utillib_rule_literal const cling_rules[];

/*
 * Rules:
 * add_op : '+' | '-'
 * mul_op : '*' | '/'
 * rel_op : ...
 * program : const_decl entity_decl
 * expr_ : '(' expr ')'
 * constant : integer | char
 * 
 * const_decl : 'const' const_def ';' const_decl | eps
 * const_def : const_int_def | const_char_def
 * const_int_def : 'int' iden '=' integer const_int_def_ls 
 * const_char_def : 'char' iden '=' char const_char_def_ls
 * const_int_def_ls : ',' iden '=' integer const_int_def_ls | eps
 * const_char_def_ls : ',' iden '=' char const_char_def_ls | eps
 * 
 * entity_decl : decl_head var_or_func_decl | 'void' main_or_func_decl
 * var_or_func_decl : 
 * var_decl : var_def ';' var_decl | eps
 * var_def : 
 */

#endif /* CLING_RULES_H */
