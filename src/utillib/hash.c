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
#include "hash.h"
#include <string.h>

static size_t utillib_charp_hash_impl_simple(char const *str)
{
	return strlen(str);
}

static size_t charp_hash_length_relative(char const *str)
{
	size_t len = strlen(str);
	size_t sum = str[0] + str[len - 1];
	sum += len << 4;
	return sum;
}

size_t utillib_charp_hash(char const *str)
{
	return charp_hash_length_relative(str);
}
