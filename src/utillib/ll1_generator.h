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

#ifndef UTILLIB_LL1_GENERATOR_H
#define UTILLIB_LL1_GENERATOR_H
#include "ll1_builder.h"
#include "rule.h"
#include "vector2.h"

struct utillib_ll1_generator {
  struct utillib_rule_index rule_index;
  struct utillib_ll1_builder builder;
  struct utillib_vector2 table;
};

void utillib_ll1_generator_init_from_code(
    struct utillib_ll1_generator *self, struct utillib_symbol const *symbols,
    struct utillib_rule_literal const *rules);

bool utillib_ll1_generator_generate(struct utillib_ll1_generator *self,
                                    char const *filename);

void utillib_ll1_generator_destroy(struct utillib_ll1_generator *self);

#endif /* UTILLIB_LL1_GENERATOR_H */
