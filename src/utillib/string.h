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
#include <stdbool.h>
#include <stddef.h>

typedef struct utillib_string {
  char *c_str;
  size_t size, capacity;
} utillib_string;

typedef enum string_cmpop {
  STRING_EQ,
  STRING_GE,
  STRING_GT,
  STRING_NE,
  STRING_LE,
  STRING_LT,
} string_cmpop;

void utillib_string_clear(utillib_string *);
void utillib_string_erase_last(utillib_string *);
char const *utillib_string_c_str(struct utillib_string *);
size_t utillib_string_size(struct utillib_string *);
bool utillib_string_empty(struct utillib_string *);
int utillib_string_init(struct utillib_string *);
int utillib_string_append(struct utillib_string *, char);
void utillib_string_destroy(struct utillib_string *);
bool utillib_string_richcmp(utillib_string *, utillib_string *, string_cmpop);
#endif // UTILLIB_STRING_H
