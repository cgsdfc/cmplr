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
#include "enum.h"

void utillib_print_enum_range(FILE * file,
			      utillib_enum_tostring_function * tostring,
			      const char *sep, int from, int to)
{
	for (int i = from; i < to; ++i) {
		fprintf(file, "%s%s", tostring(i), sep);
	}
	fputs("\n", file);
}

void utillib_print_enum(utillib_enum_tostring_function * tostring,
			int limit)
{
	utillib_print_enum_range(stdout, tostring, "\n", 0, limit);
}
