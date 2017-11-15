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

#ifndef UTILLIB_LL1_BUILDER_IMPL_H
#define UTILLIB_LL1_BUILDER_IMPL_H

#include "bitset.h"
#include "symbol.h"
#include "enum.h"
#include "rule.h"

#define UT_LL1_ERR_VAL_MAX 4
UTILLIB_ENUM_BEGIN(utillib_ll1_error_kind)
UTILLIB_ENUM_ELEM(UT_LL1_EFIRST)
UTILLIB_ENUM_ELEM(UT_LL1_EFOLLOW)
UTILLIB_ENUM_END(utillib_ll1_error_kind);

struct utillib_ll1_builder_error {
  int kind;
  struct utillib_json_value_t *values[UT_LL1_ERR_VAL_MAX];
};

/**
 * \struct utillib_ll1_set
 * A set used for both `FIRST_SET' and `FOLLOW_SET'.
 * The flag is for testing existence of special symbol
 * like `epsilon' or `eof'.
 */
struct utillib_ll1_set {
  bool flag;
  /* The set of terminal symbol values */
  struct utillib_bitset bitset;
};

void utillib_ll1_set_init(struct utillib_ll1_set *self, size_t symbols_size);

void utillib_ll1_set_destroy(struct utillib_ll1_set *self);

bool utillib_ll1_set_union_updated(struct utillib_ll1_set *self,
                           struct utillib_ll1_set const *other);

bool utillib_ll1_set_insert_updated(struct utillib_ll1_set *self, 
    struct utillib_symbol const *symbol);

bool utillib_ll1_set_contains(struct utillib_ll1_set const *self, size_t value);

bool utillib_ll1_set_equal(struct utillib_ll1_set const *self,
                           struct utillib_ll1_set const *other);

bool utillib_ll1_set_intersect(struct utillib_ll1_set const *lhs,
                               struct utillib_ll1_set const *rhs,
                               bool about_flag);

#endif /* UTILLIB_LL1_BUILDER_IMPL_H */

