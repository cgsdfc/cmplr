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
#ifndef UTILLIB_STRING_H
#define UTILLIB_STRING_H
#define UTILLIB_C_STR(S) ((S)->c_str)
#include "enum.h"
#include <stdbool.h>		// for bool
#include <stddef.h>		// for size_t

/**
 * \file utillib/string.h
 */

/**
 * \struct utillib_string
 * \invariant size+1 <= capacity if size!=0
 * else size == capacity == 0.
 * \invariant size == strlen(c_str) if
 * c_str!=NULL else size == 0.
 */

typedef struct utillib_string {
	char *c_str;
	size_t size, capacity;
} utillib_string;

UTILLIB_ENUM_BEGIN(string_cmpop)
    UTILLIB_ENUM_ELEM(STRING_EQ)
    UTILLIB_ENUM_ELEM(STRING_GE)
    UTILLIB_ENUM_ELEM(STRING_GT)
    UTILLIB_ENUM_ELEM(STRING_NE)
    UTILLIB_ENUM_ELEM(STRING_LE)
    UTILLIB_ENUM_ELEM(STRING_LT)
    UTILLIB_ENUM_END(string_cmpop)

/* constructor destructor */
void utillib_string_init(utillib_string *);
void utillib_string_init_c_str(utillib_string *, char const *);
void utillib_string_destroy(utillib_string *);

/* observer */
size_t utillib_string_size(utillib_string *);
size_t utillib_string_capacity(utillib_string *);
bool utillib_string_empty(utillib_string *);
char const *utillib_string_c_str(utillib_string *);
bool utillib_string_richcmp(utillib_string *, utillib_string *,
			    enum string_cmpop);

/* modifier */
void utillib_string_clear(utillib_string *);
void utillib_string_erase_last(utillib_string *);
void utillib_string_replace_last(utillib_string *, char);
void utillib_string_reserve(utillib_string *, size_t);
void utillib_string_append(utillib_string *, char const *);
void utillib_string_append_char(utillib_string *, char);

#endif				// UTILLIB_STRING_H
