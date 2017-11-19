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
#include "symbol.h"
#include "vector.h"
#include <stdio.h>

UTILLIB_ETAB_BEGIN(utillib_symbol_kind)
UTILLIB_ETAB_ELEM_INIT(UT_SYMBOL_TERMINAL, "terminal-symbol")
UTILLIB_ETAB_ELEM_INIT(UT_SYMBOL_NON_TERMINAL, "non-terminal-symbol")
UTILLIB_ETAB_END(utillib_symbol_kind);

struct utillib_json_value *
utillib_symbol_json_string_create(struct utillib_symbol const *self)
{
  return utillib_json_string_create(&self->name);
}

struct utillib_json_value *
utillib_symbol_json_object_create(struct utillib_symbol const *self)
{
  char const *kind_str=utillib_symbol_kind_tostring(self->kind);
  struct utillib_json_value * object=utillib_json_object_create_empty();
  utillib_json_object_push_back(object, "kind", 
      utillib_json_string_create(&kind_str));
  utillib_json_object_push_back(object, "value",
      utillib_json_int_create(&self->value));
  utillib_json_object_push_back(object, "name",
      utillib_json_string_create(&self->name));
  return object;
}

struct utillib_symbol const utillib_symbol_eof = {
    /* following the convention that EOF is zero in bison */
    .value = UT_SYM_EOF,
    .kind = UT_SYMBOL_TERMINAL,
    .name = "end-of-input-symbol"};

struct utillib_symbol const utillib_symbol_error = {
    .value = UT_SYM_ERR, .kind = UT_SYMBOL_TERMINAL, .name = "error-symbol"};

struct utillib_symbol const utillib_symbol_epsilon = {
    .value = UT_SYM_EPS, .kind = UT_SYMBOL_TERMINAL, .name = "epsilon-symbol"};

/**
 * \function utillib_symbol_check
 * Checks whether the symbol array has any inconsistent
 * entry or less or more entries than the `enum' does.
 * \param symbols The symbol table defined using `UTILLIB_SYMBOL_BEGIN'.
 * \param expected_size The number of the enumeration. If the `enum' was
 * defined by `UTILLIB_ENUM_BEGIN' with name `NAME', then pass in the
 * `NAME_N'.
 * \return If the check success.
 */
bool utillib_symbol_check(struct utillib_symbol const *symbols,
                          size_t expected_size) {
  size_t actual_size = 0;
  struct utillib_symbol const *zero = symbols;
  bool preserved_zero =
      zero->name == NULL && zero->value == 0 && zero->kind == 0;
  if (!preserved_zero) {
    puts("Entry zero should be preserved for `eof'");
    printf("Forget to use `UTILLIB_ENUM_ELEM_INIT(%s, 1)' ?", zero->name);
    return false;
  }
  struct utillib_symbol const *symbol = symbols + 1;
  for (; NULL != utillib_symbol_name(symbol); ++symbol)
    ;
  actual_size = symbol - symbols;
  if (actual_size != expected_size) {
    struct utillib_symbol const *prev = symbol - 1;
    printf("Empty Entry at `%d' detected\n", prev->value + 1);
    struct utillib_json_value const*val =
        utillib_symbol_json_object_create(prev);
    puts("\tPrevious symbol is");
    utillib_json_pretty_print(val, stdout);
    utillib_json_value_destroy(val);
    return false;
  }
  return true;
}
