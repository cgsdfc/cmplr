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

#include "ll1_generator.h"
#include "ll1_builder.h"
#include "print.h"

/**
 * \file utillib/ll1_generator.c
 * Frontend of the Utillib.LL(1).
 */

void utillib_ll1_generator_init_from_code(struct utillib_ll1_generator *self,
    struct utillib_rule_index * rule_index)
{
  utillib_ll1_builder_init(&self->builder, rule_index);
  self->rule_index=rule_index;
  self->name=name;
  utillib_ll1_builder_build_table(&self->builder, &self->table);
}


void utillib_ll1_generator_write(struct utillib_ll1_generator *self,
    const char * filename)
{



}

void utillib_ll1_generator_destroy(struct utillib_ll1_generator *self)
{
  utillib_ll1_builder_destroy(&self->builder);
  utillib_vector2_destroy(&self->table);
}

