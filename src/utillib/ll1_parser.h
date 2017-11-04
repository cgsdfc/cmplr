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
#ifndef UTILLIB_LL1_PARSER_H
#define UTILLIB_LL1_PARSER_H
#include "slist.h"
#include "rule.h"
#include "vector2.h"

/**
 * \struct utillib_ll1_parser
 * Parser for the LL(1) grammar.
 * Handles the runtime parsing of input.
 */

struct utillib_ll1_parser {
 struct utillib_slist symbol_stack;
 struct utillib_slist tree_stack;
 struct utillib_slist error_stack;
 struct utillib_rule_index const* rule_index;
 struct utillib_vector2 const* table;
};

#endif // UTILLIB_LL1_PARSER_H
