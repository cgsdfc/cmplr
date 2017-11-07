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

/**
 * \function utillib_printer_init
 * Initializes a printer with a stream to output.
 * \param file The stream to write to.
 * \param npad Number of padding space used in one indentation.
 * If it is zero, the default 2 spaces padding will be used.
 * It should be in [0, 6).
 */
void utillib_printer_init(utillib_printer_t *self, FILE *file, size_t npad) {
  static const char *static_padstr[] = {"  ",  " ",    "  ",
                                        "   ", "    ", "     "};
  self->file = file;
  self->level = 0;
  self->padstr = static_padstr[npad];
}

/**
 * \function printer_padding
 * Pads spaces without changing the indentation.
 */
static void printer_padding(utillib_printer_t *self) {
  size_t lv = self->level;
  for (size_t i = 0; i < lv; ++i)
    fputs(self->padstr, self->file);
}

/**
 * \function printer_indent
 * First increases the indentation and then
 * pads spaces.
 */
static void printer_indent(utillib_printer_t *self) {
  ++self->level;
  printer_padding(self);
}

/**
 * \function printer_unindent
 * First decreases the indentation and then
 * pads spaces.
 */
static void printer_unindent(utillib_printer_t *self) {
  --self->level;
  printer_padding(self);
}

/**
 * \function utillib_printer_print_json
 * Prettily prints the JSON data to file with proper indentation.
 * \param str Assumed to be in JSON.
 */
void utillib_printer_print_json(utillib_printer_t *self, char const *str) {
  FILE *file = self->file;
  for (; *str; ++str) {
    switch (*str) {
    case '{':
    case '[':
      fprintf(file, "%c\n", *str);
      printer_indent(self);
      break;
    case ':':
      fputs(": ", file);
      break;
    case ',':
      fputs(",\n", file);
      printer_padding(self);
      break;
    case ']':
    case '}':
      fputc('\n', file);
      printer_unindent(self);
      fputc(*str, file);
      break;
    default:
      fputc(*str, file);
      break;
    }
  }
  fputc('\n', file);
}
