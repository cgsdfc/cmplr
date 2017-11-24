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

#ifndef UTILLIB_LR_BUILDER_IMPL_H
#define UTILLIB_LR_BUILDER_IMPL_H
#include "vector.h"

/**
 * \struct utillib_lr_item
 * An item is a BNF rule decorated
 * with a tiny dot to tell the parser's
 * position after which the next symbol
 * is expected.
 * We use a straight forward way to represent
 * it plus its ID since itemset may store IDs
 * instead of pointers.
 */
struct utillib_lr_item {
  size_t id;
  size_t pos;
  struct utillib_rule * rule;
};

struct utillib_lr_item_factory {
  struct utillib_bitset * 


};

/**
 * \function utillib_lr_maximum_items
 * Counts the maximum possible number of items
 * so that itemset be made from a fixed-sized bitset.
 */
size_t utillib_lr_maximum_items(struct utillib_vector *rules);
#endif /* UTILLIB_LR_BUILDER_IMPL_H */

