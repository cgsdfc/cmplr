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
#ifndef CLING_BASIC_BLOCK
#define CLING_BASIC_BLOCK

#include <utillib/vector.h>
#include <stdio.h>

/*
 * Divide each function into a number of basic_blocks
 * for optimizer to run on.
 */
struct cling_ast_function;
struct cling_basic_block {
  int block_id;
  unsigned int begin;
  unsigned int end;
  struct utillib_vector const *instrs;
};

/*
 * Data Flow Graph
 */
struct cling_data_flow {
  struct utillib_vector * parents;
  struct utillib_vector * children;
  struct utillib_vector basic_blocks;
  struct utillib_vector const *instrs;
  unsigned int * block_map;
  unsigned int blocks_size;
  unsigned int instrs_size;
  unsigned int temps_size;
};

enum {
  CLING_FORWARD,
  CLING_BACKWARD,
};

struct cling_data_flow_callback {
  int direction;
  void (*init)(void *data, struct cling_data_flow const *data_flow);
  bool (*join)(void *data, int block_1, int block_2);
  void (*apply)(void *data, int block_id);
};

struct defpoints {
  unsigned int instr;
  unsigned int target;
  unsigned int def_id;
};

struct cling_block_data {
  unsigned int blocks_size; /* knows how to destroy three arrays */
  struct utillib_bitset* flow_in;
  struct utillib_bitset* flow_out;
  struct utillib_bitset* kill;
};

struct cling_reaching_definition {
  struct cling_block_data block_data;
  struct utillib_vector points;
};

struct cling_live_variable {
  struct cling_block_data block_data;
};

void cling_basic_block_construct(struct utillib_vector *blocks,
                                 struct utillib_vector const *instrs);
void basic_block_display(struct utillib_vector const *basic_blocks, FILE *file);
void cling_data_flow_init(struct cling_data_flow *self, struct cling_ast_function const *ast_func);
void cling_data_flow_destroy(struct cling_data_flow *self);
void cling_data_flow_print(struct cling_data_flow const *self, FILE *file);

#endif /* CLING_BASIC_BLOCK */
