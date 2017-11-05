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
#include "ll1_parser.h"

void utillib_ll1_parser_init(struct utillib_ll1_parser *self,
                             struct utillib_rule_index const *rule_index,
                             struct utillib_vector2 const *table) {
  utillib_slist_init(&self->symbol_stack);
  utillib_slist_init(&self->tree_stack);
  utillib_slist_init(&self->error_stack);
  self->table = table;
  self->rule_index = rule_index;
}

void utillib_ll1_parser_destroy(struct utillib_ll1_parser *self) {}
