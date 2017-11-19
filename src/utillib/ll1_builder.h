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
#ifndef UTILLIB_LL1_BUILDER_H
#define UTILLIB_LL1_BUILDER_H
#include "vector.h"
#include "vector2.h"

UTILLIB_ENUM_BEGIN(utillib_ll1_set_kind)
UTILLIB_ENUM_ELEM(UT_LL1_FIRST)
UTILLIB_ENUM_ELEM(UT_LL1_FOLLOW)
UTILLIB_ENUM_END(utillib_ll1_set_kind);

/**
 * \struct utillib_ll1_builder
 * Builder of the LL(1) parser table.
 * The table is constructed in terms of `FIRST' and `FOLLOW'
 * sets of rules and non-terminal symbols, respectly.
 * <para>For any string, say, string `a', FIRST(a) is the all the first
 * terminal symbols of any sequence of terminal symbols that derives
 * from `a' OR the possible epsilon special symbol.
 * <\para>
 * <para>For any non-terminal symbol `A', FOLLOW(A) is any terminal symbol
 * that can follow A in all the sentencial forms OR the end-of-input special
 * symbol.
 * <\para>
 */
struct utillib_ll1_builder {
  struct utillib_rule_index *rule_index;
  struct utillib_ll1_set *FIRST;
  struct utillib_ll1_set *FIRST_RULE;
  struct utillib_ll1_set *FOLLOW;
  struct utillib_vector errors;
};

void utillib_ll1_builder_init(struct utillib_ll1_builder *self,
                              struct utillib_rule_index *rule_index);

void utillib_ll1_builder_destroy(struct utillib_ll1_builder *self);

void utillib_ll1_builder_build_table(struct utillib_ll1_builder *self,
                                     struct utillib_vector2 *table);

int utillib_ll1_builder_check(struct utillib_ll1_builder *self);

void utillib_ll1_builder_print_errors(struct utillib_ll1_builder *self);

struct utillib_json_value *
utillib_ll1_builder_json_object_create(struct utillib_ll1_builder const *self);

#endif /* UTILLIB_LL1_BUILDER_H */
