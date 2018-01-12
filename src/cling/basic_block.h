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
struct ast_function;
struct basic_block
{
        int block_id;
        unsigned int begin;
        unsigned int end;
        struct utillib_vector const *instrs;
};

/*
 * Data Flow Graph
 */
struct data_flow 
{
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

struct data_flow_callback 
{
        int direction;
        void (*init)(void *data, struct data_flow const *data_flow);
        bool (*join)(void *data, int block_1, int block_2);
        void (*apply)(void *data, int block_id);
};

struct defpoints {
        unsigned int address;
        unsigned int target;
        unsigned int def_id;
};

struct block_data
{
        unsigned int blocks_size; /* knows how to destroy three arrays */
        struct utillib_bitset* flow_in;
        struct utillib_bitset* flow_out;
        struct utillib_bitset* kill;
};

struct reaching_definition {
        struct block_data block_data;
        struct utillib_vector points;
};

enum {
        UDS_INIT=0,
        UDS_USE,
        UDS_DEF,
};

struct live_interval {
        unsigned int temp;
        unsigned int begin;
        unsigned int end;
};

struct live_variable {
        struct block_data block_data;
        unsigned int * use_def_state;
        struct utillib_vector intervals;
};

void basic_block_construct(struct utillib_vector *blocks,
                struct utillib_vector const *instrs);
void basic_block_destroy(struct basic_block *self);
void basic_block_display(struct utillib_vector const *basic_blocks, FILE *file);

void data_flow_init(struct data_flow *self, struct ast_function const *ast_func);
void data_flow_destroy(struct data_flow *self);
void data_flow_print(struct data_flow const *self, FILE *file);

void reaching_definition_analyze(struct reaching_definition *self, struct data_flow const *data_flow);
void reaching_definition_destroy(struct reaching_definition *self);
void reaching_definition_print(struct reaching_definition const *self, FILE *file);

void live_variable_print(struct live_variable const *self, FILE *file);
void live_variable_destroy(struct live_variable *self);
void live_variable_analyze(struct live_variable *self, struct data_flow const *data_flow);

#endif /* CLING_BASIC_BLOCK */
