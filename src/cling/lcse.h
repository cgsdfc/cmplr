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
#include <utillib/hashmap.h>
#include <utillib/vector.h>

#define LCSE_TEMP_ZERO 0

/*
 * Local Common Subexpr Elimination
 * Applies only to a basic_block.
 */

struct cling_ast_function;
struct cling_ast_program;
struct cling_ast_ir;
struct cling_basic_block;

struct cling_lcse_optimizer {
  unsigned int var_count;
  struct utillib_hashmap operations;
  struct utillib_hashmap names;
  struct utillib_hashmap values;
  unsigned int variables_size;
  int *variables;
  unsigned int *address_map;
};

struct cling_lcse_value {
  unsigned int address;
  unsigned int value;
};

enum {
  LCSE_LOAD_ADDR,
  LCSE_LOAD_VALUE,
  LCSE_BINARY,
  LCSE_STORE,
  LCSE_UNARY,
};

struct cling_lcse_ir {
  int opcode;
  int kind;
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
void cling_lcse_optimizer_fix_address(struct cling_lcse_optimizer const *self,
                                      struct utillib_vector *instrs);
void cling_lcse_optimizer_emit(struct cling_lcse_optimizer *self,
                               struct cling_basic_block const *block,
                               struct utillib_vector *instrs);

#endif /* CLING_LCSE_H */
