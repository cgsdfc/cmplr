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

#include "pair.h"
#include <stdlib.h> // for malloc
                    /**
                     * \file utillib/pair.c
                     * Provides `utillib_make_pair'.
                     */

struct utillib_pair *utillib_make_pair(void *key, void *value) {
  struct utillib_pair *pair = malloc(sizeof *pair);
  pair->up_first = key;
  pair->up_second = value;
  return pair;
}
