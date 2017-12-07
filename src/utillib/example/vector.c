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

#include <stdio.h>
#include <utillib/vector.h>

/**
 * Since there is no resizable array that
 * can hold any data in C, utillib supplies
 * a vector data structure that is similar
 * to `std::vector' in C++.
 * Vector provides common subroutines like
 * push_back, back, front, pop_back and size
 * and capacity.
 * Also due to the frequency it is used, a
 * foreach loop implemented as macro is also
 * available.
 *
 */

int main(void) {
  struct utillib_vector vector;
  utillib_vector_init(&vector);
  int const int_vals[] = {1, 3, 4, 5, 6, -1};
  int val;

  for (int const *pi = int_vals; *pi != -1; ++pi) {
    utillib_vector_push_back(&vector, (void const *)*pi);
  }
  UTILLIB_VECTOR_FOREACH(val, &vector) { printf("Value is %d\n", val); }
  utillib_vector_destroy(&vector);
}
