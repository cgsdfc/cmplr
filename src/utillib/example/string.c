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

#include <utillib/string.h>
#include <stdio.h>

/*
 * A string that has varying length.
 */

int main(void)
{
  struct utillib_string  string;
  utillib_string_init_c_str(&string, "initialized with a c str");

  char const * c_str=utillib_string_c_str(&string);
  puts(c_str);
  utillib_string_destroy(&string);
  return 0;
}

