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
#include "print.h"
#include "error.h"
#include <stdarg.h> // for va_list

void utillib_print_indent(FILE *file, size_t nchar, int ch) {
  while (nchar--) {
    fputc(ch, file);
  }
}

/* returns a local static char buffer */
char *utillib_static_buf(void) {
  static char static_buf[UTILLIB_STATIC_BUF_SIZE];
  return static_buf;
}

/* use the static buffer to return a formatted str */
/* use vsnprintf to detect buffer overflow */
char const *utillib_static_vsprintf(char const *fmt, va_list ap) {
  char *buf = utillib_static_buf();
  if (vsnprintf(buf, UTILLIB_STATIC_BUF_SIZE, fmt, ap) <
      UTILLIB_STATIC_BUF_SIZE) {
    return buf;
  }
  utillib_die("buffer overflowed");
}

char const *utillib_static_sprintf(char const *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  return utillib_static_vsprintf(fmt, ap);
}
