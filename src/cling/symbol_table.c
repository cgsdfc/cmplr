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

#include "symbol_table.h"
#include <utillib/strhash.h>
#include <string.h>

static const struct utillib_hashmap_callback symbol_hash_callback={
  .hash_handler=mysql_strhash,
  .compare_handler=strcmp,
};

void cling_symbol_table_init(struct cling_symbol_table *self)
{
  utillib_hashmap_init(&self->const_table,&symbol_hash_callback);
  utillib_hashmap_init(&self->var_table,&symbol_hash_callback);
  utillib_hashmap_init(&self->func_table,&symbol_hash_callback);
}

void cling_symbol_table_destroy(struct cling_symbol_table *self)
{
  utillib_hashmap_destroy(&self->const_table);
  utillib_hashmap_destroy(&self->var_table);
  utillib_hashmap_destroy(&self->func_table);
}

