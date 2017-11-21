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
#ifndef CLING_SCANNER_H
#define CLING_SCANNER_H
#include <stdio.h> // FILE*
#include <utillib/enum.h>
#include <utillib/scanner.h>
#include <utillib/string.h>
#include <utillib/vector.h>

extern const struct utillib_keyword_pair cling_keywords[];

void cling_scanner_init(struct utillib_token_scanner *self, FILE *file);
int cling_scanner_read_handler(struct utillib_char_scanner *chars, 
    struct utillib_string *buffer);
int cling_scanner_error_handler(struct utillib_char_scanner *chars, 
    struct utillib_token_scanner_error const *error);
void const* cling_scanner_semantic_handler(size_t value, char const *str);

#endif /* CLING_SCANNER_H */

