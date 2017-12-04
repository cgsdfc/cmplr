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

#include "ast_check.h"
#include "error.h"
#include <assert.h>

int cling_ast_check_assignable(char const *name, 
    struct cling_symbol_table const * symbol_table) {
  struct cling_symbol_entry * entry;
  int kind;

  entry=cling_symbol_table_find(symbol_table, name, CL_LEXICAL);
  if (!entry)
    return CL_EUNDEFINED;
  kind=entry->kind;
  if (kind & CL_FUNC || kind & CL_ARRAY || 
      kind & CL_CONST)
    return CL_ENOTLVALUE;
  return 0;
}

int cling_ast_check_expression(struct utillib_json_value *self,
    struct cling_rd_parser *parser)
{
}

int cling_ast_check_returnness(struct cling_rd_parser const *parser, 
    int expr_type, int *return_type)
{
  assert(parser->curfunc);
  struct cling_symbol_entry * entry;
  int kind;

  entry=cling_symbol_table_find(parser->symbol_table, parser->curfunc, CL_GLOBAL);
  assert(entry);
  kind=entry->kind;
  assert(kind & CL_FUNC);
  *return_type=kind & (~CL_FUNC);
  if (*return_type == expr_type)
    return 0;

  switch(expr_type) {
  case CL_VOID:
    return CL_RET_GARBAGE;
  case CL_INT:
  case CL_CHAR:
    if (*return_type == CL_VOID)
      return CL_RET_DISCARD;
    return 0;
  default:
    assert(false);
  }
}
