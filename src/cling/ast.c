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
#include "ast.h"
#include "symbol_table.h"
#include "symbols.h"

#include <utillib/json_foreach.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

union cling_primary *cling_primary_copy(union cling_primary const *self) {
  union cling_primary *other = malloc(sizeof *other);
  memcpy(other, self, sizeof *other);
  return other;
}

inline size_t cling_primary_inthash(union cling_primary const *lhs) {
  return lhs->signed_int;
}

inline int cling_primary_intcmp(union cling_primary const *lhs,
                                union cling_primary const *rhs) {
  return lhs->signed_int - rhs->signed_int;
}

void cling_primary_toint(union cling_primary *self, size_t type) {
  switch (type) {
  case SYM_UINT:
    self->signed_int = (int)self->unsigned_int;
    break;
  case SYM_CHAR:
    self->signed_int = (char)self->signed_char;
    break;
  case SYM_INTEGER:
    break;
  }
}

void cling_primary_init(union cling_primary *self, size_t type,
                        char const *rawstr) {
  switch (type) {
  case SYM_UINT:
    sscanf(rawstr, "%u", &self->unsigned_int);
    return;
  case SYM_INTEGER:
    sscanf(rawstr, "%d", &self->signed_int);
    return;
  case SYM_STRING:
    self->string = rawstr;
    return;
  case SYM_CHAR:
    sscanf(rawstr, "%c", &self->signed_char);
    return;
  default:
    assert(false);
  }
}

