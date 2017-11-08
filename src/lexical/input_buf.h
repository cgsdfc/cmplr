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
#ifndef SCANNER_INPUT_BUF_H
#define SCANNER_INPUT_BUF_H
#include <stdarg.h>           // for va_list
#include <utillib/error.h>    // for utillib_error
#include <utillib/position.h> // for utillib_position
#include <utillib/vector.h>   // for utillib_vector

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#define INPUT_BUF_STDIN "<stdin>"
#define INPUT_BUF_STRING "<string>"
#define SCANNER_PATH_MAX 1024

typedef struct scanner_input_buf {
  /* for getting lines from file */
  utillib_vector line_offset;
  const char *filename;
  FILE *file;
  size_t col;
  size_t row;
} scanner_input_buf;

/* constructor, destructor */
void scanner_input_buf_init(scanner_input_buf *, FILE *, char const *);
void scanner_input_buf_destroy(scanner_input_buf *);

/* file function */
long scanner_input_buf_ftell(scanner_input_buf *);
bool scanner_input_buf_feof(scanner_input_buf *);
int scanner_input_buf_getc(scanner_input_buf *);
int scanner_input_buf_ungetc(scanner_input_buf *, int);

/* file info access */
const char *scanner_input_buf_filename(scanner_input_buf *);
size_t scanner_input_buf_row(scanner_input_buf *);
size_t scanner_input_buf_col(scanner_input_buf *);
utillib_position *scanner_input_buf_current_pos(scanner_input_buf *);

/* error message display */
void scanner_input_buf_pretty_perror(scanner_input_buf *, utillib_error *);

/* other utility */
bool scanner_input_buf_begin_of_line(scanner_input_buf *);
utillib_error *scanner_input_buf_make_error(scanner_input_buf *, int,
                                            char const *, ...);
utillib_error *scanner_input_buf_make_errorV(scanner_input_buf *, int,
                                             char const *, va_list);

#endif // SCANNER_INPUT_BUF_H
