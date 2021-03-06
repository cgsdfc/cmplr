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

#ifndef UTILLIB_SYMBOL_H
#define UTILLIB_SYMBOL_H
#include "enum.h"
#include "json.h"
#define utillib_symbol_name(SYM) ((SYM)->name)
#define utillib_symbol_kind(SYM) ((SYM)->kind)
#define utillib_symbol_value(SYM) ((SYM)->value)
#define UTTILLIB_SYMBOL_SIZE(SYMBOL_ARR)                                       \
  ((sizeof SYMBOL_ARR / sizeof SYMBOL_ARR[0]) - 1)

#define UTILLIB_SYMBOL_EOF (&utillib_symbol_eof)
#define UTILLIB_SYMBOL_ERR (&utillib_symbol_error)
#define UTILLIB_SYMBOL_EPS (&utillib_symbol_epsilon)

#define UTILLIB_SYMBOL_BEGIN(NAME) const struct utillib_symbol NAME[] = {
#define UTILLIB_SYMBOL_NON_TERMINAL(VALUE)                                     \
  [VALUE] = {.value = (VALUE), .kind = UT_SYMBOL_NON_TERMINAL, .name = #VALUE},
#define UTILLIB_SYMBOL_TERMINAL(VALUE)                                         \
  {.value = (VALUE), .kind = UT_SYMBOL_TERMINAL, .name = #VALUE},
#define UTILLIB_SYMBOL_END(NAME)                                               \
  { 0 }                                                                        \
  }                                                                            \
  ;

UTILLIB_ENUM_BEGIN(utillib_symbol_special_kind)
/* We must give `EOF' a non-negative value since */
/* in many cases it needs to fix into a table */
UTILLIB_ENUM_ELEM_INIT(UT_SYM_EOF, 0)
UTILLIB_ENUM_ELEM_INIT(UT_SYM_EPS, -1)
UTILLIB_ENUM_ELEM_INIT(UT_SYM_NULL, -2)
UTILLIB_ENUM_ELEM_INIT(UT_SYM_ERR, -3)
UTILLIB_ENUM_END(utillib_symbol_special_kind)

UTILLIB_ENUM_BEGIN(utillib_symbol_kind)
UTILLIB_ENUM_ELEM_INIT(UT_SYMBOL_TERMINAL, 1)
UTILLIB_ENUM_ELEM_INIT(UT_SYMBOL_NON_TERMINAL, 2)
UTILLIB_ENUM_END(utillib_symbol_kind)

/**
 * \struct utillib_symbol
 * An abstract representation
 * of a symbol (both terminal and non terminal)
 * in formal language theory.
 * Usually `utillib_symbol's are static initialized
 * to define bigger constructs of a language.
 */
struct utillib_symbol {
  int kind;
  int value;
  char const *name;
};

extern struct utillib_symbol const utillib_symbol_epsilon;
extern struct utillib_symbol const utillib_symbol_eof;
extern struct utillib_symbol const utillib_symbol_error;
extern struct utillib_symbol const utillib_symbol_null;

struct utillib_json_value *
utillib_symbol_json_object_create(struct utillib_symbol const *self);
struct utillib_json_value *
utillib_symbol_json_string_create(struct utillib_symbol const *self);

bool utillib_symbol_check(struct utillib_symbol const *symbols, size_t size);

#endif /* UTILLIB_SYMBOL_H */
