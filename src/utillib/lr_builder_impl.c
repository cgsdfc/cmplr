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
#include "lr_builder_impl.h"

size_t utillib_lr_maximum_items(struct utillib_vector *rules)
{
  size_t size=0;
  UTILLIB_VECTOR_FOREACH(struct utillib_rule const *, rule, rules) {
    if (utillib_rule_purely_epsilon(rule)) {
      /* Form of A := eps takes only 1 item */
      ++size;
      continue;
    }
    /* Plus the past-the-rule item */
    size += utillib_vector_size(&rule->RHS) + 1;
  }
  /* Plus the past-the-eof item */
  return size + 1;
}


