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
#include <utillib/bitset.h>
#include <utillib/list.h>
#include <assert.h>
#include <stdlib.h>

#define setleader(leader, index, instrs_size)                                  \
  do {                                                                         \
    assert((index) < (instrs_size));                                           \
    leaders[(index)] = true;                                                   \
  } while (0)

static struct cling_basic_block *
basic_block_create(int block_id, struct utillib_vector const *instrs,
                   unsigned int begin) {
  struct cling_basic_block *self = malloc(sizeof *self);
  self->begin = begin;
  self->instrs = instrs;
  self->block_id = block_id;
  return self;
}

void cling_basic_block_construct(struct utillib_vector *blocks,
                                 struct utillib_vector const *instrs) {

  bool *leaders;
  struct cling_ast_ir const *ir;
  struct cling_basic_block *cur_blk;
  const size_t instrs_size = utillib_vector_size(instrs);
  int block_id = 0, state = 0;

  leaders = calloc(sizeof leaders[0], instrs_size);
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
     * But still, it is smallt
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
      ++line;
    }
  }
}

static unsigned int *create_block_map(struct utillib_vector const *basic_blocks, unsigned int instrs_size) {
  unsigned int * block_map;
  struct cling_basic_block const *block;

  block_map=malloc(sizeof block_map[0] * instrs_size);
  UTILLIB_VECTOR_FOREACH(block, basic_blocks) {
    for (int i=block->begin; i<block->end; ++i) {
      block_map[i]=block->block_id;
    }
  }
  return block_map;
}

static void data_flow_bidirection(struct cling_data_flow *self)
{
  struct cling_ast_ir const *ir;
  unsigned int target_addr, source_addr;
  struct cling_basic_block const *source_block, *target_block;

  source_addr=0;
  UTILLIB_VECTOR_FOREACH(ir, self->instrs) {
    if (ast_ir_is_local_jump(ir)) {
      target_addr=ast_ir_get_jump_address(ir);
      source_block=utillib_vector_at(&self->basic_blocks, self->block_map[source_addr]);
      target_block=utillib_vector_at(&self->basic_blocks, self->block_map[target_addr]);
      utillib_vector_push_back(&self->parents[target_addr], source_block);
      utillib_vector_push_back(&self->children[source_block->block_id], target_block);
    }
    ++source_addr;
  }
}

void cling_data_flow_init(struct cling_data_flow *self, struct cling_ast_function const *ast_func)
{
  self->instrs=&ast_func->instrs;
  self->instrs_size=utillib_vector_size(self->instrs);
  utillib_vector_init(&self->basic_blocks);
  cling_basic_block_construct(&self->basic_blocks, self->instrs);
  self->blocks_size=utillib_vector_size(&self->basic_blocks);
  self->parents=malloc(sizeof self->parents[0] * self->blocks_size);
  self->children=malloc(sizeof self->children[0] * self->blocks_size);
  self->block_map=create_block_map(&self->basic_blocks, self->instrs_size);
  for (int i=0; i<self->blocks_size; ++i) {
    utillib_vector_init(&self->parents[i]);
    utillib_vector_init(&self->children[i]);
  }
  data_flow_bidirection(self);
}

void cling_data_flow_destroy(struct cling_data_flow *self)
{
  utillib_vector_destroy_owning(&self->basic_blocks, free);
  for (int i=0; i<self->blocks_size; ++i) {
    utillib_vector_destroy(&self->parents[i]);
    utillib_vector_destroy(&self->children[i]);
  }
  free(self->parents);
  free(self->children);
  free(self->block_map);
}

void  cling_data_flow_print(struct cling_data_flow const *self, FILE *file)
{
  struct cling_basic_block const *block;

  for (int i=0; i<self->blocks_size; ++i) {
    fprintf(file, "block %4d => ", i);
    UTILLIB_VECTOR_FOREACH(block, &self->children[i]) {
      fprintf(file, "%4d ", block->block_id);
    }
    fputs("\n", file);
  }
}

static void definition_create(struct cling_definition *self, unsigned int instrs_size, unsigned int blocks_size)
{
  self->blocks_size=blocks_size;
  size_t size=sizeof self->reach_in[0] * blocks_size;
  /*
   * Help those stupid compilers
   */
  self->reach_in=malloc(size);
  self->reach_out=malloc(size);
  self->kill=malloc(size);
  for (int i=0; i<self->blocks_size; ++i) {
    utillib_bitset_init(&self->reach_in[i], instrs_size);
    utillib_bitset_init(&self->reach_out[i], instrs_size);
    utillib_bitset_init(&self->kill[i], instrs_size);
  }
  utillib_vector_init(&self->points);
}

static void definition_destroy(struct cling_definition *self)
{
  for (int i=0; i<self->blocks_size; ++i) {
    utillib_bitset_destroy(&self->reach_in[i]);
    utillib_bitset_destroy(&self->reach_out[i]);
    utillib_bitset_destroy(&self->kill[i]);
  }
  free(self->reach_in);
  free(self->reach_out);
  free(self->kill);
  utillib_vector_destroy_owning(&self->points, free);
}

struct defpoints * defpoints_create(unsigned int instr, unsigned int target)
{
  struct defpoints *self=malloc(sizeof *self);
  self->instr=instr;
  self->target=target;
  return self;
}

static void definitions_initialize(struct cling_definition *self, struct cling_data_flow *data_flow)
{
  struct cling_basic_block const *block;
  struct cling_ast_ir const *ir;
  unsigned int target;

  UTILLIB_VECTOR_FOREACH(block, &data_flow->basic_blocks) {
    for (int i=block->begin; i<block->end; ++i) {
      ir=utillib_vector_at(data_flow->instrs, i);
      target=ast_ir_get_assign_target(ir);
      if (target < 0)
        continue;
      utillib_vector_push_back(&self->points, defpoints_create(i, target));
      utillib_bitset_insert(&self->reach_out[block->block_id], utillib_vector_size(&self->points));
    }
  }
}

/*
 * reach_in = union (reach_out, parents)
 * reach_out = reach_in - kill
 * change = reach_in.new - reach_in.old
 */
void cling_data_flow_reaching_definitions(struct cling_data_flow *self, struct cling_definition *definitions)
{
  struct utillib_list worklist;
  struct cling_basic_block const *head, *block;
  struct utillib_vector *parents, *children;
  struct utillib_bitset *input, *output, *kill;
  bool changed;

  utillib_list_init(&worklist);
  UTILLIB_VECTOR_FOREACH(block, &self->basic_blocks)
    utillib_list_push_back(&worklist, block);

  while (!utillib_list_empty(&worklist)) {
    head=utillib_list_front(&worklist);
    utillib_list_pop_front(&worklist);
    input=&definitions->reach_in[head->block_id];
    output=&definitions->reach_out[head->block_id];
    kill=&definitions->kill[head->block_id];
    parents=&self->parents[head->block_id];
    children=&self->children[head->block_id];
    changed=false;
    UTILLIB_VECTOR_FOREACH(block, parents)
      if (utillib_bitset_union_updated(input, &definitions->reach_out[block->block_id]))
        changed=true;
    if (!changed)
      continue;
    utillib_bitset_union(output, input);
    utillib_bitset_minus(output, kill);
    UTILLIB_VECTOR_FOREACH(block, children) {
      utillib_list_push_back(&worklist, block);
    }
  }
  utillib_list_destroy(&worklist);
}

