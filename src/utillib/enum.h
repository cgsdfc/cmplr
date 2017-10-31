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
#ifndef UTILLIB_ENUM
#define UTILLIB_ENUM
#include <stdio.h>		// for FILE*
// ENUM
#define UTILLIB_ETAB_TOSTRING(NAME) const char *NAME##_tostring(int tag)
#define UTILLIB_ENUM_ELEM(TAG) TAG,
#define UTILLIB_ENUM_ELEM_INIT(TAG, VAL) TAG = (VAL),
#define UTILLIB_ENUM_BEGIN(NAME) typedef enum NAME {
#define UTILLIB_ENUM_END(NAME)                                                 \
  NAME##_N                                                                     \
  }                                                                            \
  NAME;                                                                        \
  UTILLIB_ETAB_TOSTRING(NAME);
// ETAB
#define UTILLIB_ETAB_ELEM_ARRAY_IMPL(TAB, VAL) [TAB] = (VAL),
#define UTILLIB_ETAB_ELEM_ARRAY(TAB) UTILLIB_ETAB_ELEM_ARRAY_IMPL(TAB, #TAB)
#define UTILLIB_ETAB_ELEM_SWITCH_IMPL(TAB, VAL)                                \
  case (TAB):                                                                  \
    return (VAL);
#define UTILLIB_ETAB_ELEM_SWITCH(TAB) UTILLIB_ETAB_ELEM_SWITCH_IMPL(TAB, #TAB)
#define UTILLIB_ETAB_BIGIN_IMPL_COMMON(NAME) UTILLIB_ETAB_TOSTRING(NAME) {
#define UTILLIB_ETAB_BEGIN_SWITCH(NAME) switch (tag) {
#define UTILLIB_ETAB_END_SWITCH(NAME)                                          \
  default:                                                                     \
    return "(null)";                                                           \
    }
#define UTILLIB_ETAB_BEGIN_ARRAY(NAME)                                         \
  if (tag < 0 || tag >= NAME##_N) {                                            \
    return "(null)";                                                           \
  }                                                                            \
  const static char *NAME##_tab[] = {
#define UTILLIB_ETAB_END_ARRAY(NAME)                                           \
  }                                                                            \
  ;                                                                            \
  return NAME##_tab[tag];
#define UTILLIB_ETAB_END_IMPL_COMMON(NAME) }

#ifdef UTILLIB_ETAB_USE_SWITCH
// begin
#define UTILLIB_ETAB_BEGIN_IMPL(NAME)                                          \
  UTILLIB_ETAB_BIGIN_IMPL_COMMON(NAME)                                         \
  UTILLIB_ETAB_BEGIN_SWITCH(NAME)
// elem
#define UTILLIB_ETAB_ELEM_IMPL(NAME) UTILLIB_ETAB_ELEM_SWITCH(NAME)
// end
#define UTILLIB_ETAB_END_IMPL(NAME)                                            \
  UTILLIB_ETAB_END_SWITCH(NAME)                                                \
  UTILLIB_ETAB_END_IMPL_COMMON(NAME)
// elem_init
#define UTILLIB_ETAB_ELEM_INIT(NAME, VAL)                                      \
  UTILLIB_ETAB_ELEM_SWITCH_IMPL(NAME, VAL)

#else
// begin
#define UTILLIB_ETAB_BEGIN_IMPL(NAME)                                          \
  UTILLIB_ETAB_BIGIN_IMPL_COMMON(NAME)                                         \
  UTILLIB_ETAB_BEGIN_ARRAY(NAME)
// elem
#define UTILLIB_ETAB_ELEM_IMPL(NAME) UTILLIB_ETAB_ELEM_ARRAY(NAME)
// end
#define UTILLIB_ETAB_END_IMPL(NAME)                                            \
  UTILLIB_ETAB_END_ARRAY(NAME)                                                 \
  UTILLIB_ETAB_END_IMPL_COMMON(NAME)
// elem_init
#define UTILLIB_ETAB_ELEM_INIT(NAME, VAL)                                      \
  UTILLIB_ETAB_ELEM_ARRAY_IMPL(NAME, VAL)
#endif

#define UTILLIB_ETAB_BEGIN(NAME) UTILLIB_ETAB_BEGIN_IMPL(NAME)
#define UTILLIB_ETAB_END(NAME) UTILLIB_ETAB_END_IMPL(NAME)
#define UTILLIB_ETAB_ELEM(NAME) UTILLIB_ETAB_ELEM_IMPL(NAME)

#define UTILLIB_ENUM_CONST(NAME, VALUE) NAME = (VALUE),
typedef const char *(utillib_enum_tostring_function) (int);
void utillib_print_enum_range(FILE *, utillib_enum_tostring_function *,
			      const char *, int, int);
void utillib_print_enum(utillib_enum_tostring_function *, int);

#endif				// UTILLIB_ENUM
