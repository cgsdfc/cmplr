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
#ifndef UTILLIB_PRINT_H
#define UTILLIB_PRINT_H
#include "typedef.h"
#include <stdarg.h>		// for va_list
#include <stddef.h>		// for offsetof
#include <stdio.h>
#define UTILLIB_STATIC_BUF_SIZE BUFSIZ

void utillib_print_indent(FILE *, size_t, int);
char *utillib_static_buf(void);
char const *utillib_static_vsprintf(char const *fmt, va_list ap);
char const *utillib_static_sprintf(char const *fmt, ...)
    __attribute__ ((__format__(__printf__, 1, 2)));
char const *utillib_int_str(void *);

typedef struct utillib_printer_t {
	FILE *file;
	size_t level;
	char const *padstr;
} utillib_printer_t;

void utillib_printer_init(utillib_printer_t *, FILE *, size_t);
void utillib_printer_print_json(utillib_printer_t *, char const *);

#endif				// UTILLIB_PRINT_H
