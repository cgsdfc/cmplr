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
#include "ast_build.h"
#include "scanner.h"
#include "symbols.h"

#include <assert.h>
#include <stdlib.h>
/*
 * This whole file is full of hack...
 * Because OPG is in fact not powerful enough
 * to parse complicated expression grammar.
 * Maybe Recursive-Decent will do better in terms of
 * less hack and more coherence logic...
 * Packrat Parsing may also fit in better. If we get the
 * time and interest, we will implement a Packrat Parser
 * in utillib and use it as an alternative to RD+OPG in
 * cling. In fact, after so many BNF-like thinking, it is
 * time to clean your brain.
 * Notes that this opg_parser can recognize all the constructs
 * noted in opg_parser.h just in a stupid way.
 *
 */
enum { CL_OPG_GT, CL_OPG_LT, CL_OPG_ERR };

static void opg_parser_init(struct cling_opg_parser *self, size_t eof_symbol) {
  /*
   * We do not want eof_symbol to conflict
   * with any other one. Simply use negative
   * to distinguish both.
   */
  self->eof_symbol = eof_symbol;
  utillib_vector_init(&self->stack);
  utillib_vector_init(&self->opstack);
  utillib_vector_push_back(&self->opstack, self->eof_symbol);
}

/**
 * \function opg_parser_compare
 * This is a hard-coded Precedence Matrix.
 * Compares the precedence of the stack-top symbol(lhs)
 * and the lookahead symbol(rhs).
 * If it is `CL_OPG_GT', we should reduce. (read, begin hacking)
 * If it is `CL_OPG_LT', we should shift rhs into stack.
 * If it is `CL_OPG_ERR', we hit an error.
 * Hack: The judgements are kept from low to high precedence, which means
 * the code can be less and cleaner.
 * What's worse, the order within the same group (lhs==..., rhs==...) matters.
 */
static size_t opg_parser_compare(struct cling_opg_parser *self, size_t lhs,
                                 size_t rhs) {
  const size_t eof_symbol = self->eof_symbol;
  const bool lhs_is_relop = opg_parser_is_relop(lhs);
  const bool rhs_is_relop = opg_parser_is_relop(rhs);

  if (lhs == eof_symbol && utillib_vector_size(&self->opstack) == 1)
    /*
     * Make sure it is a true eof_symbol on the stacktop.
     */
    return CL_OPG_LT;

  if (lhs == SYM_LP || rhs == SYM_LP)
    /*
     * Give expression more chances than eof_symbol.
     */
    return CL_OPG_LT;

  if (lhs == SYM_RP || rhs == SYM_RP)
    return CL_OPG_GT;

  if (rhs == eof_symbol)
    /*
     * But if it is a true eof_symbol...
     */
    return CL_OPG_GT;

  if (lhs == SYM_LK || rhs == SYM_LK)
    return CL_OPG_LT;

  if (lhs == SYM_RK || rhs == SYM_RK)
    return CL_OPG_GT;

  if (lhs == SYM_COMMA) /* shift in as much SYM_COMMA as possible */
    return CL_OPG_LT;
  if (rhs == SYM_COMMA)
    return CL_OPG_GT;

  if (rhs == SYM_EQ)
    return CL_OPG_GT;
  if (lhs == SYM_EQ)
    return CL_OPG_LT;

  if (rhs_is_relop)
    return CL_OPG_GT;
  if (lhs_is_relop)
    return CL_OPG_LT;

  if (rhs == SYM_ADD || rhs == SYM_MINUS)
    return CL_OPG_GT;
  if (lhs == SYM_ADD || lhs == SYM_MINUS)
    return CL_OPG_LT;

  if (rhs == SYM_MUL || rhs == SYM_DIV)
    return CL_OPG_GT;
  if (lhs == SYM_MUL || lhs == SYM_DIV)
    return CL_OPG_LT;

  if (lhs == SYM_IDEN && rhs == SYM_IDEN)
    return CL_OPG_ERR;
  if (lhs == SYM_IDEN)
    return CL_OPG_GT;
  if (rhs == SYM_IDEN)
    return CL_OPG_LT;

  return CL_OPG_ERR;
}

/*
 * Two debugging functions.
 * Show you the stack and opstack, repectedly.
 */
static void opg_parser_show_opstack(struct cling_opg_parser const *self) {
  size_t op;
  UTILLIB_VECTOR_FOREACH(op, &self->opstack) {
    printf("%s ", cling_symbol_kind_tostring(op));
  }
  puts("");
}

static void opg_parser_show_stack(struct cling_opg_parser const *self) {
  struct utillib_json_value *val;

  UTILLIB_VECTOR_FOREACH(val, &self->stack) { utillib_json_pretty_print(val); }
}

static void opg_parser_show_lookahead(size_t lookahead, size_t stacktop) {
  printf("stacktop %s\n", cling_symbol_kind_tostring(stacktop));
  printf("lookahead %s\n", cling_symbol_kind_tostring(lookahead));
}

/*
 * The name of the operator is kept as
 * that of the `SYM_XXX' (instead of `OP_XXX').
 * Specially, subscription is `SYM_RK'.
 * And call is `SYM_RP'.
 */
static int opg_parser_reduce(struct cling_opg_parser *self, size_t lookahead) {
  struct utillib_json_value *lhs;
  struct utillib_json_value *rhs;
  struct utillib_json_value *object;
  struct utillib_vector *stack = &self->stack;
  struct utillib_vector *opstack = &self->opstack;
  struct utillib_vector argstack;
  size_t stacktop = utillib_vector_back(opstack);
  size_t op = stacktop;

  /*
   * Since relop contains too much cases,
   * it is separated from the small switch
   * below.
   */
  if (opg_parser_is_relop(stacktop)) {
    goto make_binary;
  }

  switch (stacktop) {
  case SYM_COMMA:
    goto make_arglist;
  case SYM_RP:
    /*
     * Hack: We must drill into the opstack
     * to see whether this is an expression
     * group or a call_expr.
     * So the SYM_IDEN matters.
     */
    if (utillib_vector_size(opstack) < 2)
      return 1;
    utillib_vector_pop_back(opstack);
    utillib_vector_pop_back(opstack);
    if (utillib_vector_size(opstack) < 1)
      return 0;
    stacktop = utillib_vector_back(opstack);
    if (stacktop != SYM_IDEN)
      return 0;
    /*
     * Hack:
     * Just in case if we are in single argument
     * case, we popped that single argument into
     * the second top array. For example:
     * [] 1 => [1]
     */
    rhs = utillib_vector_back(stack);
    if (rhs->kind != UT_JSON_ARRAY) {
      utillib_vector_pop_back(stack);
      lhs = utillib_vector_back(stack);
      utillib_json_array_push_back(lhs, rhs);
    }
    goto make_binary;
  case SYM_RK:
    if (utillib_vector_size(opstack) < 2)
      return 1;
    utillib_vector_pop_back(opstack);
    utillib_vector_pop_back(opstack);
    goto make_binary;
  case SYM_IDEN:
    utillib_vector_pop_back(opstack);
    return 0;
  case SYM_ADD:
  case SYM_MINUS:
  case SYM_DIV:
  case SYM_MUL:
  case SYM_EQ: /* Assignment Op */
    goto make_binary;
  default:
    goto error;
  }

make_arglist:
  /*
   * Hack: Following the SYM_COMMA in the opstack
   * we pop out the argument in the stack.
   * We also use a auxiliary vector to help reversing
   * the order of the arguments.
   */
  utillib_vector_init(&argstack);
  while (true) {
    stacktop = utillib_vector_back(opstack);
    if (stacktop != SYM_LP && stacktop != SYM_COMMA)
      goto error;
    if (utillib_vector_size(stack) < 1)
      goto error;
    rhs = utillib_vector_back(stack);
    utillib_vector_pop_back(stack);
    utillib_vector_push_back(&argstack, rhs);
    if (stacktop == SYM_LP) {
      lhs = utillib_vector_back(stack);
      size_t size = utillib_vector_size(&argstack);
      for (int i = size - 1; i >= 0; --i) {
        utillib_json_array_push_back(lhs, utillib_vector_at(&argstack, i));
      }
      utillib_vector_destroy(&argstack);
      return 0;
    }
    utillib_vector_pop_back(opstack);
  }

make_binary:
  /*
   * There is no Hack.
   * Pops out lhs and rhs from stack
   * and pops out op from opstack.
   * Combines them to a new node and
   * pushes it back to the stack.
   */
  utillib_vector_pop_back(opstack);
  if (utillib_vector_size(stack) < 2)
    goto error;
  rhs = utillib_vector_back(stack);
  utillib_vector_pop_back(stack);
  lhs = utillib_vector_back(stack);
  utillib_vector_pop_back(stack);
  object = utillib_json_object_create_empty();
  cling_ast_set_op(object, op);
  cling_ast_set_lhs(object, lhs);
  cling_ast_set_rhs(object, rhs);
  utillib_vector_push_back(stack, object);
  return 0;
error:
  return 1;
}

void cling_opg_parser_init(struct cling_opg_parser *self, size_t eof_symbol) {
  opg_parser_init(self, eof_symbol);
}

void cling_opg_parser_destroy(struct cling_opg_parser *self) {
  utillib_vector_destroy_owning(&self->stack, utillib_json_value_destroy);
  utillib_vector_destroy(&self->opstack);
}

/*
 * Keep strange things out of touch.
 */
static inline bool good_token(size_t lookahead)
{
  switch(lookahead) {
    case SYM_ADD:
    case SYM_MUL:
    case SYM_IDEN:
    case SYM_LK:
    case SYM_RK:
    case SYM_INTEGER:
    case SYM_CHAR:
    case SYM_EQ:
    case SYM_NE:
    case SYM_DEQ:
    case SYM_LT:
    case SYM_LE:
    case SYM_GT:
    case SYM_GE:
    case SYM_DIV:
    case SYM_MINUS:
    case SYM_COMMA:
    case SYM_LP:
    case SYM_RP:
      return true;
    default:
      return false;
  }
}

/*
 * Currently, it return null as indicator of error.
 * The client should check this and look at the last_error.
 */
struct utillib_json_value *
cling_opg_parser_parse(struct cling_opg_parser *self,
                       struct cling_scanner *scanner) {

  size_t lookahead;
  size_t stacktop;
  size_t cmp;
  struct utillib_json_value *val;
  struct utillib_vector *stack = &self->stack;
  struct utillib_vector *opstack = &self->opstack;
  const size_t eof_symbol = self->eof_symbol;

  while (!utillib_vector_empty(opstack)) {
    /* opg_parser_show_stack(self); */
    lookahead = cling_scanner_lookahead(scanner);
    /* if (!good_token(lookahead)) */
    /*   goto error; */
    if (utillib_vector_size(opstack) == 1 && lookahead == eof_symbol) {
      /*
       * We will catch any success before the precedence is computed
       * since the success condition is correct and thus have higher
       * priority.
       * I don't know what will happen if 2 SYM_RP mix together in
       * opg_parser_compare.
       */
      if (utillib_vector_size(stack) != 1)
        goto error;
      /*
       * And we must pop this successful result
       * since opg_parser no longer owns it but
       * in case of failure, all the partial nodes
       * will be destroyed using utillib_vector_destroy_owning.
       */
      val = utillib_vector_back(stack);
      utillib_vector_pop_back(stack);
      return val;
    }

    stacktop = utillib_vector_back(opstack);
    if (lookahead == SYM_IDEN || lookahead == SYM_INTEGER || lookahead == SYM_CHAR) {
      utillib_vector_push_back(
          stack, cling_ast_factor(lookahead, cling_scanner_semantic(scanner)));
      if (lookahead == SYM_IDEN)
        /*
         * SYM_IDEN matters in call_expr
         * so pushes it.
         */
        utillib_vector_push_back(opstack, lookahead);
      cling_scanner_shiftaway(scanner);
      continue;
    }
    /*
     * Hack: This is a special case we must handle:
     * 'foo(' when we should push an empty array
     * onto the stack or it will be too late.
     * But once again, this '(' needs to be shifted in.
     */
    if (stacktop == SYM_IDEN && lookahead == SYM_LP) {
      utillib_vector_push_back(stack, utillib_json_array_create_empty());
      goto shiftin;
    }
    cmp = opg_parser_compare(self, stacktop, lookahead);
    switch (cmp) {
      case CL_OPG_LT:
shiftin:
        utillib_vector_push_back(opstack, lookahead);
        cling_scanner_shiftaway(scanner);
        break;
      case CL_OPG_GT:
        if (0 != opg_parser_reduce(self, lookahead)) {
          goto error;
        }
        break;
      default:
      case CL_OPG_ERR:
        goto error;
    }
  }
error:
  return NULL;
}

/*
 * In some case reuseing a opg_parser is desirable.
 * After reinited, it can be used as inited.
 */
void cling_opg_parser_reinit(struct cling_opg_parser *self, size_t eof_symbol) {
  cling_opg_parser_destroy(self);
  opg_parser_init(self, eof_symbol);
}
