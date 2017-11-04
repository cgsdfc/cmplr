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
#include "equal.h"
#include <string.h>

bool utillib_equal_c_str(char const *lhs, char const *rhs)
{
	return 0 == strcmp(lhs, rhs);
}

bool utillib_equal_int(int const * i0, int const * i1) 
{
  return *(int*) i0 == *(int*) i1;
}

bool utillib_equal_bool(bool const *b0, bool const *b1)
{
  return *(bool*) b0 == *(bool*) b1;
}

