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

#include "rd_parser.h"
#include "symbols.h"
#include <utillib/json.h>
#include <utillib/scanner.h>

/**
 * \function single_const_decl
 * Handles a single piece of const declaration.
 * examples:
 * const int i=0;
 * const int i=0, j=1, k=2;
 * const char c='ch';
 */
static struct utillib_json_value *
single_const_decl(struct cling_rd_parser *self,
    struct utillib_token_scanner * input)
{


  return utillib_json_null_create();


}

void cling_rd_parser_init(struct cling_rd_parser *self)
{
  utillib_vector_init(&self->elist);
}

void cling_rd_parser_destroy(struct cling_rd_parser *self)
{
  utillib_vector_destroy(&self->elist);
}

struct utillib_json_value *
cling_rd_parser_parse(struct cling_rd_parser *self,
    struct utillib_token_scanner *input)
{
  return single_const_decl(self, input);
}

