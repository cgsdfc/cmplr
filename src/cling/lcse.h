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
#ifndef CLING_LCSE_H
#define CLING_LCSE_H
#include <utillib/hashmap.h> /* Record the history instructions */
#include <utillib/vector.h>

/*
 * Local Common Subexpr Elimination
 * Applies only to a basic_block.
 */

/*
 * Forward decl
 */
struct cling_ast_function;
struct cling_ast_program;
struct cling_ast_ir;
struct cling_basic_block;

struct cling_lcse_optimizer {
  unsigned int var_count; /* Rename the temps in ast_func */
  struct utillib_hashmap operations; /* Effective operations done in the past */
  struct utillib_hashmap names; /* Map name to their address counted by var_count */
  struct utillib_hashmap values; /* Map address to value again counted by var_count */
  unsigned int variables_size; /* number of renamed variables, the length of variables */
  int *variables; /* Map the old names to new names (variables are named in 1 2 3...) */
  unsigned int *address_map; /* Since some of the instructions are deleted, map old jump
                                address to the new ones */
};

/*
 * Used in values
 */
struct cling_lcse_value {
  unsigned int address;
  unsigned int value;
};

/*
 * Opcode that participates in lcse
 */
enum {
  LCSE_LOAD_ADDR, /* ldadr */
  LCSE_LOAD_VALUE, /* ldnam */
  LCSE_BINARY, /* binop */
  LCSE_STORE, /* stadr, stnam */
  LCSE_UNARY,
};

struct cling_lcse_ir {
  int opcode; /* Original ast_ir opcode */
  int kind; /* LCSE_ see above */
  union {
    struct {
      int result;
      int temp1;
      int temp2;
    } binary;
    struct {
      char const *name;
      int scope;
      int address;
    } load_addr;
    struct {
      int value;
      int scope;
    } load_value;
    struct {
      int value;
      int address;
    } store;
    struct {
      int result;
      int operand;
    } unary;
  };
};

void cling_lcse_optimizer_init(struct cling_lcse_optimizer *self,
                               struct cling_ast_function const *ast_func);
void cling_lcse_optimizer_destroy(struct cling_lcse_optimizer *self);

void cling_lcse_optimizer_emit(struct cling_lcse_optimizer *self,
                               struct utillib_vector const *basic_blocks,
                               struct cling_ast_function *ast_func);

#endif /* CLING_LCSE_H */
