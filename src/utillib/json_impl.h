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
#ifndef UTILLIB_JSON_IMPL_H
#define UTILLIB_JSON_IMPL_H

#include <assert.h>
#define json_value_check_kind(VAL, KIND) do {\
  assert ((VAL)->kind == (KIND) && "JSON value should be of kind `" # KIND "'");\
} while (0)


#define JSON_PRIMARY_CREATE_FUNC_DEFINE(NAME, FIELD, TYPE, VALUE) \
struct utillib_json_value *  NAME(void const *base) \
{\
  struct utillib_json_value *self=malloc(sizeof *self);\
  self->FIELD=*(TYPE*) base;\
  self->kind=(VALUE);\
  return self;\
}

#endif /* UTILLIB_JSON_IMPL_H */

