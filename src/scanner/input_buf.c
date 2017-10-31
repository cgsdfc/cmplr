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
#define _BSD_SOURCE		// for strdup
#include "input_buf.h"
#include <errno.h>		// for errno
#include <string.h>		// strerror
#include <unistd.h>		// getcwd
#include <utillib/color.h>	// for GREEN_CARET
#include <utillib/print.h>

typedef utillib_position scanner_position;

/* line_offset is the offset that can be used to seek a line */
/* in a file with its lineno */
/* it is implemented with an offset array, the i th entry of which */
/* is the line_offset of the i th line */
static void save_line_offset(scanner_input_buf * self)
{
	// called when a newline character was encountered, before self->row and
	// self->col
	// were modified. add self->col+1 to the line_offset of the previous line
	// to form that of the current line whose newline character is just met.
	// then push the current line_offset into the vector.

	// previous line
	size_t cur_offset =
	    (size_t) utillib_vector_at(&(self->line_offset),
				       self->row - 1);
	cur_offset += self->col + 1;	// for the \n char
	utillib_vector_push_back(&(self->line_offset),
				 (utillib_element_t) cur_offset);
}

/* do the fseek based on line_offset and return the line */
static char const *seek_line(scanner_input_buf * self,
			     scanner_position const *pos)
{
	size_t row = UTILLIB_POS_ROW(pos);
	// the line_offset needed to seek line i is stored in i-1 in the vector
	size_t cur_offset =
	    (size_t) utillib_vector_at(&(self->line_offset), row - 1);
	// save the cur_pos so that we can seek back later
	long cur_pos = ftell(self->file);
	if (fseek(self->file, cur_offset, SEEK_SET) < 0) {
		utillib_die(strerror(errno));
	}
	char *buf = utillib_static_buf();
	if (NULL == fgets(buf, UTILLIB_STATIC_BUF_SIZE, self->file)) {
		utillib_die(strerror(errno));
	}
	// seek back
	fseek(self->file, SEEK_SET, cur_pos);
	return buf;
}

/* print a green caret pointing the annoying character */
/* in a separated line */
static void insert_indicator(scanner_position * pos)
{
	for (int i = 0; i < UTILLIB_POS_COL(pos); ++i) {
		fputc(' ', stderr);
	}
	fputs(GREEN_CARET, stderr);
	fputc('\n', stderr);
}

/* the tuple (absfilename, row, col, level) is called the location prefix */
/* of an error message */
static char const *location_prefix(scanner_input_buf * self, int lv,
				   scanner_position * pos)
{
	static char absdir[SCANNER_PATH_MAX];
	if (NULL == getcwd(absdir, sizeof absdir)) {
		utillib_die(strerror(errno));
	}
	return utillib_static_sprintf("%s/%s:%lu:%lu: %s ", absdir,
				      self->filename, UTILLIB_POS_ROW(pos),
				      UTILLIB_POS_COL(pos),
				      utillib_error_lv_tostring(lv));
}

/* constructor */
/* assumes that file was just opened for reading. */
void scanner_input_buf_init(scanner_input_buf * self, FILE * file,
			    char const *filename)
{
	static const size_t init_line_capacity = 100;
	self->file = file;
	self->filename = filename;
	self->row = 1;
	self->col = 0;
	utillib_vector_init(&(self->line_offset));
	utillib_vector_reserve(&(self->line_offset), init_line_capacity);
	// the line_offset of line 1 is zero.
	utillib_vector_push_back(&(self->line_offset),
				 (utillib_element_t) 0);
}

/* destructor */
/* also close the FILE * NOT opened by self */
void scanner_input_buf_destroy(scanner_input_buf * self)
{
	utillib_vector_destroy(&(self->line_offset));
	if (self->file != stdin) {
		fclose(self->file);
	}
}

/* fetch a char from self->file, increment the row and col */
/* and save_line_offset when newline is encountered */
int scanner_input_buf_getc(scanner_input_buf * self)
{
	int ch = fgetc(self->file);
	switch (ch) {
	case '\n':
		save_line_offset(self);
		self->row++;
		self->col = 0;
		break;
	case EOF:
		return EOF;
	default:
		self->col++;
		break;
	}
	return ch;
}

/* push a char back into self->file and decrement row or col */
int scanner_input_buf_ungetc(scanner_input_buf * self, int ch)
{
	switch (ch) {
	case '\n':
		self->row--;
		break;
	case EOF:
		return EOF;
	default:
		self->col--;
		break;
	}
	return ungetc(ch, self->file);
}

/* wrapping ftell */
long scanner_input_buf_ftell(scanner_input_buf * self)
{
	return ftell(self->file);
}

/* wrapping feof */
bool scanner_input_buf_feof(scanner_input_buf * self)
{
	return feof(self->file);
}

/* access filename */
const char *scanner_input_buf_filename(scanner_input_buf * self)
{
	return self->filename;
}

/* access row */
size_t scanner_input_buf_row(scanner_input_buf * self)
{
	return self->row;
}

/* access col*/
size_t scanner_input_buf_col(scanner_input_buf * self)
{
	return self->col;
}

/* access utillib_position */
/* using static variable, return value must be stored else where */
/* by the caller */
scanner_position *scanner_input_buf_current_pos(scanner_input_buf * self)
{
	static scanner_position pos;
	pos.lineno = self->row;
	pos.column = self->col;
	return &pos;
}

/* combine the location_prefix and seek_line with the info stored in the */
/* error to print a pretty error message to stderr. */
/* it is pretty because the message printed is colorful with indicator */
void scanner_input_buf_pretty_perror(scanner_input_buf * self,
				     utillib_error * error)
{
	scanner_position *pos = &(error->ue_pos);
	int lv = error->ue_lv;
	char const *str = location_prefix(self, lv, pos);
	fputs(str, stderr);
	fputs(error->ue_msg, stderr);
	fputc('\n', stderr);
	str = seek_line(self, pos);
	utillib_print_indent(stderr, 4, ' ');
	fputs(str, stderr);
	utillib_print_indent(stderr, 4, ' ');
	insert_indicator(pos);
}

bool scanner_input_buf_begin_of_line(scanner_input_buf * self)
{
	int c;
	if (self->row == 1) {
		return true;
	}
	if ((c = scanner_input_buf_getc(self)) == '\n') {
		return true;
	}
	scanner_input_buf_ungetc(self, c);
	return false;
}

utillib_error *scanner_input_buf_make_errorV(scanner_input_buf * self,
					     int lv, char const *fmt,
					     va_list ap)
{
	char const *str = utillib_static_vsprintf(fmt, ap);
	utillib_error *err =
	    utillib_make_error(lv, scanner_input_buf_current_pos(self),
			       strdup(str));
	return err;
}

// make an utillib_error with the current position
// of input_buf. taking printf like
// arguments
utillib_error *scanner_input_buf_make_error(scanner_input_buf * self,
					    int lv, char const *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	return scanner_input_buf_make_errorV(self, lv, fmt, ap);
}
