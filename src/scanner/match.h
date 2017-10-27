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
#ifndef SCANNER_MATCH_H
#define SCANNER_MATCH_H
/* provides routines to match the charachers
 * against tokens.
 * on successful match, they return SCANNER_MATCHED.
 * on unmatch, they return SCANNER_UNMATCHED.
 * on error when matching, i.e. unterminated
 * string or comment, they return SCANNER_ERROR.
 */
#include "scanner.h"
typedef utillib_pair_t scanner_str_entry_t;
int scanner_match_any_char(scanner_base_t *);
int scanner_match_identifier(scanner_base_t *);
int scanner_match_string_double(scanner_base_t *);
int scanner_match_string_single(scanner_base_t *);
int scanner_match_string_angle(scanner_base_t *);
int scanner_skip_cpp_comment(scanner_base_t *);
int scanner_skip_c_comment(scanner_base_t *);
int scanner_skip_space(scanner_input_buf *);
scanner_str_entry_t const *scanner_search_string(scanner_str_entry_t const *,
                                                 char const *);
#endif // SCANNER_MATCH_H
