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
#include "basic_block.h"
#include "ast_ir.h"
#include <assert.h>
#include <stdlib.h>

#define setleader(leader, index, instrs_size)                                  \
  do {                                                                         \
    assert((index) < (instrs_size));                                           \
    leaders[(index)] = true;                                                   \
  } while (0)

static struct cling_basic_block *
basic_block_create(int block_id, struct utillib_vector *instrs,
                   unsigned int begin) {
  struct cling_basic_block *self = malloc(sizeof *self);
  self->begin = begin;
  self->instrs = instrs;
  self->block_id = block_id;
  return self;
}

void cling_basic_block_construct(struct utillib_vector *blocks,
                                 struct utillib_vector *instrs) {

  bool *leaders;
  struct cling_ast_ir const *ir;
  struct cling_basic_block *cur_blk;
  const size_t instrs_size = utillib_vector_size(instrs);
  int block_id = 0, state = 0;

  leaders = calloc(instrs_size,sizeof leaders[0] );
  /*
   * Set the first and last instr as leader.
   */
  setleader(leaders, 0, instrs_size);
  setleader(leaders, instrs_size - 1, instrs_size);
  for (int i = 0; i < instrs_size; ++i) {
    ir = utillib_vector_at(instrs, i);
    switch (ir->opcode) {
    case OP_JMP:
      setleader(leaders, ir->jmp.addr, instrs_size);
      setleader(leaders, i + 1, instrs_size);
      break;
    case OP_BEZ:
      setleader(leaders, ir->bez.addr, instrs_size);
      setleader(leaders, i + 1, instrs_size);
      break;
    case OP_BNE:
      setleader(leaders, ir->bne.addr, instrs_size);
      setleader(leaders, i + 1, instrs_size);
      break;
    case OP_RET:
      setleader(leaders, ir->ret.addr, instrs_size);
      setleader(leaders, i+1, instrs_size);
      break;
    }
  }
  for (int i = 0; i < instrs_size;) {
    /*
     * State Machine is good at pattern matching.
     */
    switch (state) {
    case 0:
      if (!leaders[i] || i==instrs_size-1) {
        ++i;
      } else {
        cur_blk = basic_block_create(block_id, instrs, i);
        ++i;
        state = 1;
      }
      break;
    case 1:
      if (leaders[i]) {
        cur_blk->end = i;
        ++block_id;
        utillib_vector_push_back(blocks, cur_blk);
        state = 0;
      } else {
        ++i;
      }
      break;
    default:
      assert(false);
    }
  }
  /*
   * The Exit Block.
   */
  cur_blk = basic_block_create(block_id, instrs, instrs_size - 1);
  cur_blk->end = instrs_size;
  utillib_vector_push_back(blocks, cur_blk);
  free(leaders);
}

void basic_block_display(struct utillib_vector const *basic_blocks) {
  struct cling_basic_block const *block;
  int line = 0;
  UTILLIB_VECTOR_FOREACH(block, basic_blocks) {
    printf("Block %d\n", block->block_id);
    for (int i = block->begin; i < block->end; ++i) {
      printf("%4d\t", line);
      ast_ir_print(utillib_vector_at(block->instrs, i), stdout);
      puts("");
      ++line;
    }
    puts("");
  }
}
