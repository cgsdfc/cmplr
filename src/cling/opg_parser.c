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

#include "opg_parser.h"
#include "error.h"
#include "ir.h"

#include <assert.h>
#include <stdlib.h>
enum {
  CL_OPG_GT,
  CL_OPG_LT,
  CL_OPG_EQ,
  CL_OPG_ERR
};

static void opg_parser_init(struct cling_opg_parser *self)
{
  utillib_vector_init(&self->stack);
  utillib_vector_init(&self->opstack);
  utillib_vector_push_back(&self->opstack, self->eof_symbol);
}

void cling_opg_parser_init(struct cling_opg_parser *self, 
    size_t eof_symbol, struct utillib_vector *elist) {
  self->eof_symbol=eof_symbol;
  self->elist=elist;
  opg_parser_init(self);
}

void cling_opg_parser_destroy(struct cling_opg_parser *self) {
  utillib_vector_destroy_owning(&self->stack, utillib_json_value_destroy);
  utillib_vector_destroy(&self->opstack);
}

/**
 * \function opg_parser_compare
 * Compares the precedence of the stack-top symbol(lhs)
 * and the lookahead symbol(rhs).
 * \return If it is `CL_OPG_GT', we should reduce.
 * If it is `CL_OPG_LT', we should shift rhs into stack.
 * If it is `CL_OPG_EQ', we should pop the stack and shift off lookahead.
 * If it is `CL_OPG_EQ', we hit an error.
 */
static size_t opg_parser_compare(struct cling_opg_parser *self, size_t lhs, size_t rhs)
{
  const size_t eof_symbol=self->eof_symbol;
  if (lhs == eof_symbol)
    return CL_OPG_LT;

  if (rhs == eof_symbol)
    return CL_OPG_GT;

  if (lhs == SYM_LP && rhs == SYM_RP)
    return CL_OPG_EQ;

  if (lhs == SYM_LP || rhs == SYM_LP)
    return CL_OPG_LT;

  if (lhs == SYM_RP || rhs == SYM_RP)
    return CL_OPG_GT;

  if (lhs == SYM_IDEN) {
    if (rhs == SYM_LP || rhs == SYM_LK)
      return CL_OPG_LT;
    if (rhs == SYM_RP || rhs == SYM_RK)
      return CL_OPG_GT;
    return CL_OPG_GT;
  }

  if (lhs == SYM_LK || rhs == SYM_LK)
    return CL_OPG_LT;

  if (lhs == SYM_RK || rhs == SYM_RK)
    return CL_OPG_GT;
  
  if (lhs == SYM_ADD || lhs == SYM_MINUS) {
    if (rhs == SYM_ADD || rhs == SYM_MINUS)
      return CL_OPG_GT;
    if (rhs == SYM_MUL || rhs == SYM_DIV)
      return CL_OPG_LT;
    if (rhs == SYM_LP)
      return CL_OPG_LT;
    if (rhs == SYM_RP)
      return CL_OPG_GT;
  }
  if (lhs == SYM_MUL || lhs == SYM_DIV) {
    if (rhs == SYM_MUL || rhs == SYM_DIV)
      return CL_OPG_GT;
    if (rhs == SYM_ADD || rhs == SYM_MINUS)
      return CL_OPG_GT;
  }
  return CL_OPG_ERR;
}

static struct utillib_json_value *
opg_parser_factor_create(size_t code, void const * semantic)
{
  switch (code) {
  case SYM_IDEN:
    return utillib_json_string_create(&semantic);
  case SYM_CHAR:
  case SYM_UINT:
    return utillib_json_size_t_create(&semantic);
  default:
    assert(false);
  }
}

static void opg_parser_show_opstack(struct cling_opg_parser const *self)
{
  UTILLIB_VECTOR_FOREACH(size_t , op, &self->opstack) {
    printf("%s, ", cling_symbol_kind_tostring(op));
  }
  puts("");
}

static int opg_parser_reduce(struct cling_opg_parser *self, size_t stacktop)
{
  assert (stacktop == (size_t) utillib_vector_back(&self->opstack));
  struct utillib_json_value * lhs;
  struct utillib_json_value * rhs;
  struct utillib_json_value * object;
  size_t op;
  struct utillib_vector * stack=&self->stack;
  struct utillib_vector * opstack=&self->opstack;
  utillib_vector_pop_back(opstack);
#ifndef NDEBUG
  char const * opstr;
#endif

  switch (stacktop) {
  case SYM_RK:
    if (utillib_vector_size(opstack) < 2)
      return 1;
    utillib_vector_pop_back(opstack);
    utillib_vector_pop_back(opstack);
    op=OP_IDX;
    goto make_binary;
  case SYM_IDEN:
    /*
     * This iden will be popped above
     */
    return 0;
  case SYM_ADD:
    op=OP_ADD;
    goto make_binary;
  case SYM_MINUS:
    op=OP_SUB;
    goto make_binary;
  case SYM_DIV:
    op=OP_DIV;
    goto make_binary;
  case SYM_MUL:
    op=OP_MUL;
    goto make_binary;
  default:
    return 1;
  }
make_binary:
    if (utillib_vector_size(stack) < 2)
      return 1;
    rhs=utillib_vector_back(stack);
    utillib_vector_pop_back(stack);
    lhs=utillib_vector_back(stack);
    utillib_vector_pop_back(stack);
    object=utillib_json_object_create_empty();
#ifndef NDEBUG
    opstr=cling_opcode_kind_tostring(op);
    utillib_json_object_push_back(object, "op", 
        utillib_json_string_create(&opstr));
#else
    utillib_json_object_push_back(object, "op",
        utillib_json_size_t_create(&op));
#endif
    utillib_json_object_push_back(object, "lhs", lhs);
    utillib_json_object_push_back(object, "rhs", rhs);
    utillib_vector_push_back(stack, object);
    return 0;
}

struct utillib_json_value *
cling_opg_parser_parse(struct cling_opg_parser *self, 
                       struct utillib_token_scanner *input) {

  size_t lookahead;
  size_t stacktop;
  size_t cmp;
  char const *errmsg;
  struct utillib_json_value * val;
  struct utillib_vector * stack=&self->stack;
  struct utillib_vector * opstack=&self->opstack;
  const size_t eof_symbol=self->eof_symbol;

  while (true) {
    lookahead=utillib_token_scanner_lookahead(input);
    stacktop=utillib_vector_back(opstack);
    if (stacktop == eof_symbol && lookahead == eof_symbol) {
      break;
    }
    if (lookahead == SYM_IDEN || lookahead == SYM_UINT
        || lookahead == SYM_CHAR) {
     utillib_vector_push_back(stack, 
         opg_parser_factor_create(lookahead, 
          utillib_token_scanner_semantic(input)));
     if (lookahead == SYM_IDEN)
       utillib_vector_push_back(opstack, lookahead);
     utillib_token_scanner_shiftaway(input);
     continue;
    } 
    cmp=opg_parser_compare(self, stacktop, lookahead);
    switch (cmp) {
    case CL_OPG_LT:
      utillib_vector_push_back(opstack, lookahead);
      utillib_token_scanner_shiftaway(input);
      break;
    case CL_OPG_EQ:
      utillib_vector_pop_back(opstack);
      utillib_token_scanner_shiftaway(input);
      break;
    case CL_OPG_GT:
      if (0 != opg_parser_reduce(self, stacktop)) {
        errmsg="imcomplete expression";
        goto vague_error;
      }
      break;
    case CL_OPG_ERR:
      errmsg="invalid expression";
      goto vague_error;
    }
  }
  if (utillib_vector_size(stack) != 1 || utillib_vector_size(opstack) != 1) {
    errmsg="invalid expression";
    goto vague_error;
  }
  val=utillib_vector_back(stack);
  utillib_vector_pop_back(stack);
  return val;
vague_error:
  utillib_vector_push_back(self->elist, cling_vague_error(input, errmsg));
  return utillib_json_null_create();
}

void cling_opg_parser_reinit(struct cling_opg_parser *self)
{
  /*
   * Clean up and init == reinit
   */
  cling_opg_parser_destroy(self);
  opg_parser_init(self);
}

