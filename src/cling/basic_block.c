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
#include <assert.h>
#include <stdlib.h>

#define setleader(leader, index, instrs_size)                                  \
        do {                                                                         \
                assert((index) < (instrs_size));                                           \
                leaders[(index)] = true;                                                   \
        } while (0)


static void reaching_definitions_initialize(struct cling_reaching_definition *self,
                struct cling_data_flow const *data_flow);

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

void basic_block_display(struct utillib_vector const *basic_blocks, FILE *file) {
        struct cling_basic_block const *block;
        int line = 0;
        UTILLIB_VECTOR_FOREACH(block, basic_blocks) {
                fprintf(file, "Block %d\n", block->block_id);
                for (int i = block->begin; i < block->end; ++i) {
                        fprintf(file, "%4d\t", line);
                        ast_ir_print(utillib_vector_at(block->instrs, i), file);
                        ++line;
                }
        }
}

void basic_block_destroy(struct cling_basic_block *self) { free(self); }

/*
 * Create a mapping from each instr id to block_id
 */
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

static void data_flow_add_link(struct cling_data_flow *self, struct cling_basic_block const *source_block,
                struct cling_basic_block const *target_block)
{
        utillib_vector_push_back(&self->parents[target_block->block_id], source_block);
        utillib_vector_push_back(&self->children[source_block->block_id], target_block);
}

/*
 * Fill the parents and children array in a bidirection fashion
 */
static void data_flow_bidirection(struct cling_data_flow *self)
{
        struct cling_ast_ir const *ir;
        unsigned int target_addr, source_addr;
        struct cling_basic_block const *source_block, *target_block;

        source_addr=0;
        UTILLIB_VECTOR_FOREACH(source_block, &self->basic_blocks) {
                for (int i=source_block->begin; i<source_block->end; ++i) {
                        ir=utillib_vector_at(self->instrs, i);
                        if (ast_ir_is_local_jump(ir)) {
                                target_addr=ast_ir_get_jump_address(ir);
                                target_block=utillib_vector_at(&self->basic_blocks, self->block_map[target_addr]);
                                switch(ir->opcode) {
                                        case OP_BEZ:
                                        case OP_BNE:
                                                data_flow_add_link(self, source_block, target_block);
                                                /*
                                                 * those are 2 ways jumps
                                                 */ 
                                                target_block=utillib_vector_at(&self->basic_blocks, source_block->block_id+1);
                                                data_flow_add_link(self, source_block, target_block);
                                                break;
                                        case OP_RET:
                                        case OP_JMP:
                                                /*
                                                 * one way jumps
                                                 */
                                                data_flow_add_link(self, source_block, target_block);
                                                break;
                                        default: assert(false);
                                }
                        } else if (i == source_block->end-1 && source_block->block_id != self->blocks_size-1) {
                                /*
                                 * The last instr in a block fail through to the next
                                 */
                                target_block=utillib_vector_at(&self->basic_blocks, source_block->block_id+1);
                                data_flow_add_link(self, source_block, target_block);
                        }
                        ++source_addr;
                }
        }
}

/*
 * Initialize the data flow graph. All necessary info contained
 */
void cling_data_flow_init(struct cling_data_flow *self, struct cling_ast_function const *ast_func)
{
        self->instrs=&ast_func->instrs;
        self->instrs_size=utillib_vector_size(self->instrs);
        self->temps_size=ast_func->temps;
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

void cling_data_flow_print(struct cling_data_flow const *self, FILE *file)
{
        struct cling_basic_block const *block;
        fprintf(file, "data_flow of %p\n", self);
        fprintf(file, "blocks_size %u\n", self->blocks_size);
        fprintf(file, "temps_size %u\n", self->temps_size);
        fprintf(file, "instrs_size %u\n", self->instrs_size);
        fputs("only parents are displayed\n", file);

        fputs("all basic_blocks\n", file);
        basic_block_display(&self->basic_blocks, file);
        fputs("flow graph\n", file);
        for (int i=0; i<self->blocks_size; ++i) {
                if (i == self->blocks_size-1) {
                        fprintf(file, "block %4d <end>\n", i);
                        continue;
                }
                fprintf(file, "block %4d => ", i);
                UTILLIB_VECTOR_FOREACH(block, &self->children[i]) {
                        fprintf(file, "%4d ", block->block_id);
                }
                fputs("\n", file);
        }
        fputs("\n", file);
}

/*
 * Join the blocks on cur_blk
 * return any change happened
 */
static bool data_flow_join(void *data,
                struct cling_data_flow_callback const *algorithm,
                int block_id,
                struct utillib_vector const *blocks) {
        bool changed = false;
        struct cling_basic_block const *block;

        UTILLIB_VECTOR_FOREACH(block, blocks)
                if (algorithm->join(data, block_id, block->block_id))
                        changed = true;
        return changed;
}

static int find_block(bool *workset, unsigned int blocks_size)
{
        for (int i=0; i<blocks_size; ++i)
                if (!workset[i])
                        return i;
        return -1;
}

/*
 * Use a worklist algorithm to run different data flow algorithm
 */
void cling_data_flow_analyze(struct cling_data_flow const *self, void *data,
                struct cling_data_flow_callback const *algorithm) {
        /*
         * true: _not_ in the workset
         * false _in_ the workset
         * Initial, all in the workset
         */
        bool *workset;
        int head;
        struct cling_basic_block const *block;
        struct utillib_vector const *join_blocks, *next_blocks;

        workset=calloc(sizeof workset[0], self->blocks_size);
        algorithm->init(data, self);

        while (true) {
                head=find_block(workset, self->blocks_size);
                if (head < 0)
                        /*
                         * the workset becomes empty
                         */
                        break;
                workset[head]=true;
                switch (algorithm->direction) {
                        case CLING_FORWARD:
                                join_blocks = &self->parents[head];
                                next_blocks = &self->children[head];
                                break;
                        case CLING_BACKWARD:
                                join_blocks = &self->children[head];
                                next_blocks = &self->parents[head];
                                break;
                        default:
                                assert(false);
                }
                if (!data_flow_join(data, algorithm, head, join_blocks))
                        continue;
                algorithm->apply(data, head);
                UTILLIB_VECTOR_FOREACH(block, next_blocks) {
                        workset[block->block_id]=false;
                }
        }
        free(workset);
}

/*
 * Put those mallocs in one place
 */
static void block_data_init(struct cling_block_data *self, unsigned int blocks_size, unsigned int bitset_N)
{
        size_t size;
        self->blocks_size=blocks_size;
        size=sizeof self->flow_in[0] * blocks_size;
        self->flow_in=malloc(size);
        self->flow_out=malloc(size);
        self->kill=malloc(size);
        for (int i=0; i<self->blocks_size; ++i) {
                utillib_bitset_init(&self->flow_out[i], bitset_N);
                utillib_bitset_init(&self->flow_in[i], bitset_N);
                utillib_bitset_init(&self->kill[i], bitset_N);
        }
}

static void block_data_destroy(struct cling_block_data *self)
{
        for (int i=0; i<self->blocks_size; ++i) {
                utillib_bitset_destroy(&self->flow_in[i]);
                utillib_bitset_destroy(&self->flow_out[i]);
                utillib_bitset_destroy(&self->kill[i]);
        }
        free(self->flow_in);
        free(self->flow_out);
        free(self->kill);
}

/*
 * Reaching Definition
 * reach_in = union (reach_out, parents)
 * reach_out = reach_in - kill
 * change = reach_in.new - reach_in.old
 */
static void reaching_definition_init(struct cling_reaching_definition *self, struct cling_data_flow const *data_flow)
{
        block_data_init(&self->block_data, data_flow->blocks_size, data_flow->instrs_size); 
        utillib_vector_init(&self->points);
        reaching_definitions_initialize(self, data_flow);
}

void reaching_definition_destroy(struct cling_reaching_definition *self)
{
        block_data_destroy(&self->block_data);
        utillib_vector_destroy_owning(&self->points, free);
}

struct defpoints *defpoints_create(unsigned int address, unsigned int target,
                unsigned int def_id) {
        struct defpoints *self=malloc(sizeof *self);
        self->address=address;
        self->target=target;
        self->def_id = def_id;
        return self;
}

static void reaching_definitions_initialize(struct cling_reaching_definition *self,
                struct cling_data_flow const *data_flow) {
        struct cling_ast_ir const *ir;
        int target, address, def_id;
        struct utillib_bitset *reach_out;
        struct defpoints *killed_point, *point;

        address = 0;
        /*
         * generate all the defpoints while filling the reach_out
         */
        UTILLIB_VECTOR_FOREACH(ir, data_flow->instrs) {
                target = ast_ir_get_assign_target(ir);
                if (target >= 0) {
                        def_id = utillib_vector_size(&self->points);
                        reach_out = &self->block_data.flow_out[data_flow->block_map[address]];
                        utillib_bitset_insert(reach_out, def_id);
                        utillib_vector_push_back(&self->points,
                                        defpoints_create(address, target, def_id));
                }
                ++address;
        }
        /*
         * Fill the kill
         */
        for (int i = 0; i < data_flow->blocks_size; ++i) {
                reach_out = &self->block_data.flow_out[i];
                UTILLIB_BITSET_FOREACH(def_id, reach_out) {
                        point = utillib_vector_at(&self->points, def_id);
                        UTILLIB_VECTOR_FOREACH(killed_point, &self->points) {
                                if (killed_point->target == point->target &&
                                                /*
                                                 * One cannot kill himself
                                                 */
                                                killed_point->def_id != point->def_id) {
                                        utillib_bitset_insert(&self->block_data.kill[i], killed_point->def_id);
                                }
                        }
                }
        }
}

/*
 * Join: in=union(out of parents) 
 */
static bool reaching_definition_join(struct cling_reaching_definition *self, int block_1,
                int block_2) {
        return utillib_bitset_union_updated(&self->block_data.flow_in[block_1],
                        &self->block_data.flow_out[block_2]);
}

/*
 * Apply the data_flow operator out=in-kill
 */
static void reaching_definition_apply(struct cling_reaching_definition *self, int block_id) {
        struct utillib_bitset *output;
        output = &self->block_data.flow_out[block_id];
        utillib_bitset_union(output, &self->block_data.flow_in[block_id]);
        utillib_bitset_minus(output, &self->block_data.kill[block_id]);
}

static const struct cling_data_flow_callback reaching_definition_callback = {
        .direction = CLING_FORWARD,
        .init = reaching_definition_init,
        .join = reaching_definition_join,
        .apply = reaching_definition_apply,
};

static void block_data_print(struct cling_block_data const *self, FILE *file)
{
        unsigned int id;
        for (int i=0; i<self->blocks_size; ++i) {
                fprintf(file, "block %d\n", i);
                fputs("flow_in ", file);
                UTILLIB_BITSET_FOREACH(id, &self->flow_in[i]) {
                        fprintf(file, "%u ", id);
                }
                fputs("\n", file);
                fputs("flow_out ", file);
                UTILLIB_BITSET_FOREACH(id, &self->flow_out[i]) {
                        fprintf(file, "%u ", id);
                }
                fputs("\n", file);
                fputs("kill ", file);
                UTILLIB_BITSET_FOREACH(id, &self->kill[i]) {
                        fprintf(file, "%u ", id);
                }
                fputs("\n", file);
        }
        fputs("\n", file);
}

void cling_reaching_definition_print(struct cling_reaching_definition const *self, FILE *file)
{
        struct defpoints const * point;

        fprintf(file, "definition %p\n", self);
        fputs("defpoints\n", file);
        UTILLIB_VECTOR_FOREACH(point, &self->points) {
                fprintf(file, "defpoint %u at address %u target is t%u\n", point->def_id, point->address, point->target);
        }
        block_data_print(&self->block_data, file);
}

inline void cling_reaching_definition_analyze(struct cling_reaching_definition *self, struct cling_data_flow const *data_flow)
{
        cling_data_flow_analyze(data_flow, self, &reaching_definition_callback);
}

/*
 * Live Variable
 * in=out-kill
 * out=union(children of in)
 */

static void live_variable_fill(struct cling_live_variable *self, int block_id, int temp, int flag)
{
        switch(flag) {
                case UDS_USE:
                        utillib_bitset_insert(&self->block_data.flow_in[block_id], temp);
                        break;
                case UDS_DEF:
                        utillib_bitset_insert(&self->block_data.kill[block_id], temp);
                        break;
                default: assert(false);
        }
}
static void live_variable_init(struct cling_live_variable *self, struct cling_data_flow const *data_flow)
{
        struct cling_ast_ir const *ir;
        unsigned int address, block_id, argc;

        address=0;
        block_data_init(&self->block_data, data_flow->blocks_size, data_flow->temps_size);
        utillib_vector_init(&self->intervals);
        self->use_def_state=calloc(sizeof self->use_def_state[0], data_flow->temps_size);
        UTILLIB_VECTOR_FOREACH(ir, data_flow->instrs) {
                block_id=data_flow->block_map[address];
                switch(ir->opcode) {
                        case OP_RET:
                                if (ir->ret.has_result)
                                        live_variable_fill(self, block_id, ir->ret.result, UDS_USE);
                                break;
                        case OP_ADD:
                        case OP_SUB:
                        case OP_DIV:
                        case OP_MUL:
                        case OP_EQ:
                        case OP_NE:
                        case OP_LT:
                        case OP_LE:
                        case OP_GT:
                        case OP_GE:
                                live_variable_fill(self, block_id, ir->binop.temp1, UDS_USE);
                                live_variable_fill(self, block_id, ir->binop.temp2, UDS_USE);
                                live_variable_fill(self, block_id, ir->binop.result, UDS_DEF);
                                break;
                        case OP_BEZ:
                                live_variable_fill(self, block_id, ir->bez.temp, UDS_USE);
                                break;
                        case OP_BNE:
                                live_variable_fill(self, block_id, ir->bne.temp1, UDS_USE);
                                live_variable_fill(self, block_id, ir->bne.temp2, UDS_USE);
                                break;
                        case OP_CAL:
                                for (argc=0; argc<ir->call.argc; ++argc)
                                        live_variable_fill(self, block_id, ir->call.argv[argc], UDS_USE);
                                if (ir->call.has_result)
                                        live_variable_fill(self, block_id, ir->call.result, UDS_DEF);
                                break;
                        case OP_INDEX:
                                live_variable_fill(self, block_id, ir->index.array_addr, UDS_USE);
                                live_variable_fill(self, block_id, ir->index.index_result, UDS_USE);
                                live_variable_fill(self, block_id, ir->index.result, UDS_DEF);
                                break;
                        case OP_LDIMM:
                                live_variable_fill(self, block_id, ir->ldimm.temp, UDS_DEF);
                                break;
                        case OP_LDSTR:
                                live_variable_fill(self, block_id, ir->ldstr.temp, UDS_DEF);
                                break;
                        case OP_DEREF:
                                live_variable_fill(self, block_id, ir->deref.addr, UDS_USE);
                                break;
                        case OP_LDADR:
                                live_variable_fill(self, block_id, ir->ldadr.temp, UDS_DEF);
                                break;
                        case OP_LDNAM:
                                live_variable_fill(self, block_id, ir->ldnam.temp, UDS_DEF);
                                break;
                        case OP_STADR:
                                live_variable_fill(self, block_id, ir->stadr.addr, UDS_USE);
                                live_variable_fill(self, block_id, ir->stadr.value, UDS_USE);
                                break;
                        case OP_STNAM:
                                live_variable_fill(self, block_id, ir->stnam.value, UDS_USE);
                                break;
                        case OP_READ:
                                live_variable_fill(self, block_id, ir->read.temp, UDS_DEF);
                                break;
                        case OP_WRITE:
                                live_variable_fill(self, block_id, ir->write.temp, UDS_USE);
                                break;
                        default: break;
                }
                ++address;
        }
}

static struct live_interval * live_interval_create(unsigned int temp, unsigned int begin, unsigned int end)
{
        struct live_interval *self=malloc(sizeof *self);
        self->temp=temp;
        self->begin=begin;
        self->end=end;
        return self;
}

void live_variable_destroy(struct cling_live_variable *self)
{
        block_data_destroy(&self->block_data);
        free(self->use_def_state);
        utillib_vector_destroy_owning(&self->intervals, free);
}

static bool live_variable_join(struct cling_live_variable *self, int block_1, int block_2)
{
        return utillib_bitset_union_updated(&self->block_data.flow_out[block_1], &self->block_data.flow_in[block_2]);
}

static void live_variable_apply(struct cling_live_variable *self, int block_id)
{
        utillib_bitset_union(&self->block_data.flow_in[block_id], &self->block_data.flow_out[block_id]);
        utillib_bitset_minus(&self->block_data.flow_in[block_id], &self->block_data.kill[block_id]);
}

static const struct cling_data_flow_callback live_variable_callback = {
        .direction = CLING_BACKWARD,
        .init = live_variable_init,
        .join = live_variable_join,
        .apply = live_variable_apply,
};

void cling_live_variable_print(struct cling_live_variable const *self, FILE *file)
{
        fprintf(file, "cling_live_variable %p\n", self);
        block_data_print(&self->block_data, file);
}

void cling_live_variable_analyze(struct cling_live_variable *self, struct cling_data_flow const *data_flow)
{
        cling_data_flow_analyze(data_flow, self, &live_variable_callback);
}
