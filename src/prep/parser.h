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
#ifndef PREP_SECOND_PASS_H
#define PREP_SECOND_PASS_H
#include "scanner.h"
#include <utillib/slist.h>
#include <utillib/unordered_map.h>
#include <utillib/vector.h>
typedef struct prep_arg_t prep_arg_t;
typedef struct prep_macro_entry_t {

} prep_macro_entry_t;

typedef struct prep_parser_t {
  prep_scanner_t *psp_cur_scan;
  utillib_slist psp_incl_stack;
  utillib_unordered_map psp_macro_map;
  utillib_unordered_map psp_directives;
  utillib_vector psp_errors;
} prep_parser_t;

void prep_parser_init(prep_parser_t *, prep_arg_t *);
void prep_parser_destroy(prep_parser_t *);
#endif // PREP_SECOND_PASS_H
