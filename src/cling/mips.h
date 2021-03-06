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
#ifndef CLING_MIPS_H
#define CLING_MIPS_H

#include <utillib/enum.h>
#include <utillib/hashmap.h>
#include <utillib/vector.h>

#include <stdio.h>

/*
 * We want to be explicit about
 * the bits of the ints.
 */
#include <inttypes.h>
#define CLING_MIPS_REG_MAX 32

        UTILLIB_ENUM_BEGIN(mips_opcode_kind)
        UTILLIB_ENUM_ELEM(MIPS_NOP)
        UTILLIB_ENUM_ELEM(MIPS_SYSCALL)
        UTILLIB_ENUM_ELEM(MIPS_MOVE)
        UTILLIB_ENUM_ELEM(MIPS_ADDI)
        UTILLIB_ENUM_ELEM(MIPS_MFLO)
        UTILLIB_ENUM_ELEM(MIPS_ADDU)
        UTILLIB_ENUM_ELEM(MIPS_SUB)
        UTILLIB_ENUM_ELEM(MIPS_SUBU)
        UTILLIB_ENUM_ELEM(MIPS_JAL)
        UTILLIB_ENUM_ELEM(MIPS_DIV)
        UTILLIB_ENUM_ELEM(MIPS_MULT)
        UTILLIB_ENUM_ELEM(MIPS_ADD)
        UTILLIB_ENUM_ELEM(MIPS_J)
        UTILLIB_ENUM_ELEM(MIPS_JR)
        UTILLIB_ENUM_ELEM(MIPS_BNE)
        UTILLIB_ENUM_ELEM(MIPS_BEQ)
        UTILLIB_ENUM_ELEM(MIPS_BGEZ)
        UTILLIB_ENUM_ELEM(MIPS_BGTZ)
        UTILLIB_ENUM_ELEM(MIPS_BLEZ)
        UTILLIB_ENUM_ELEM(MIPS_BLTZ)
        UTILLIB_ENUM_ELEM(MIPS_LW)
        UTILLIB_ENUM_ELEM(MIPS_SW)
        UTILLIB_ENUM_ELEM(MIPS_LB)
        UTILLIB_ENUM_ELEM(MIPS_SB)
        UTILLIB_ENUM_ELEM(MIPS_LA)
        UTILLIB_ENUM_ELEM(MIPS_LI)
UTILLIB_ENUM_ELEM(MIPS_LABEL)
        UTILLIB_ENUM_END(mips_opcode_kind);

        UTILLIB_ENUM_BEGIN(mips_regster)
        UTILLIB_ENUM_ELEM(MIPS_ZERO)
        UTILLIB_ENUM_ELEM(MIPS_AT)
        UTILLIB_ENUM_ELEM(MIPS_V0)
        UTILLIB_ENUM_ELEM(MIPS_V1)
        UTILLIB_ENUM_ELEM(MIPS_A0)
        UTILLIB_ENUM_ELEM(MIPS_A1)
        UTILLIB_ENUM_ELEM(MIPS_A2)
        UTILLIB_ENUM_ELEM(MIPS_A3)
        UTILLIB_ENUM_ELEM(MIPS_T0)
        UTILLIB_ENUM_ELEM(MIPS_T1)
        UTILLIB_ENUM_ELEM(MIPS_T2)
        UTILLIB_ENUM_ELEM(MIPS_T3)
        UTILLIB_ENUM_ELEM(MIPS_T4)
        UTILLIB_ENUM_ELEM(MIPS_T5)
        UTILLIB_ENUM_ELEM(MIPS_T6)
        UTILLIB_ENUM_ELEM(MIPS_T7)
        UTILLIB_ENUM_ELEM(MIPS_S0)
        UTILLIB_ENUM_ELEM(MIPS_S1)
        UTILLIB_ENUM_ELEM(MIPS_S2)
        UTILLIB_ENUM_ELEM(MIPS_S3)
        UTILLIB_ENUM_ELEM(MIPS_S4)
        UTILLIB_ENUM_ELEM(MIPS_S5)
        UTILLIB_ENUM_ELEM(MIPS_S6)
        UTILLIB_ENUM_ELEM(MIPS_S7)
        UTILLIB_ENUM_ELEM(MIPS_T8)
        UTILLIB_ENUM_ELEM(MIPS_T9)
        UTILLIB_ENUM_ELEM(MIPS_K0)
        UTILLIB_ENUM_ELEM(MIPS_K1)
        UTILLIB_ENUM_ELEM(MIPS_GP)
        UTILLIB_ENUM_ELEM(MIPS_SP)
UTILLIB_ENUM_ELEM(MIPS_RA)
        UTILLIB_ENUM_END(mips_regster);

        UTILLIB_ENUM_BEGIN(mips_syscall_kind)
        UTILLIB_ENUM_ELEM_INIT(MIPS_PRINT_INT, 1)
        UTILLIB_ENUM_ELEM_INIT(MIPS_PRINT_STRING, 4)
        UTILLIB_ENUM_ELEM_INIT(MIPS_PRINT_CHAR, 11)
        UTILLIB_ENUM_ELEM_INIT(MIPS_READ_INT, 5)
        UTILLIB_ENUM_ELEM_INIT(MIPS_READ_STRING, 8)
        UTILLIB_ENUM_ELEM_INIT(MIPS_READ_CHAR, 12)
UTILLIB_ENUM_ELEM_INIT(MIPS_EXIT, 10)
        UTILLIB_ENUM_END(mips_syscall_kind);

        /*
         * These are in fact assembler directives.
         * But all we concern about is the part used
         * to define global data such as .asciiz, .byte
         * and .word, so as the name of it.
         */
        UTILLIB_ENUM_BEGIN(mips_data_kind)
        UTILLIB_ENUM_ELEM(MIPS_ASCIIZ)
        UTILLIB_ENUM_ELEM(MIPS_SPACE)
        UTILLIB_ENUM_ELEM(MIPS_WORD)
UTILLIB_ENUM_ELEM(MIPS_BYTE)
        UTILLIB_ENUM_END(mips_data_kind);

        struct ast_ir;
        struct ast_function;
        struct ast_program;

        struct mips_program {
                struct utillib_vector text;
                struct utillib_vector data;
                struct utillib_hashmap labels;
                struct option const *option;
        };

struct mips_global {
        struct option const *option;
        struct utillib_vector *data;
        struct utillib_hashmap *labels;
        int label_count;
};

struct mips_temp {
        int kind;
        int state;
        uint8_t regid;
        uint32_t offset;
};

struct mips_name {
        int kind;
        uint8_t regid;
        uint32_t offset;
};

/*
 * For temp.state.
 */
enum {
        MIPS_LOCKED,
        MIPS_HAS_REG,
        MIPS_NO_REG,
};

/*
 * For temp.kind.
 */
enum {
        MIPS_SAVED,
        MIPS_UNSAVED,
        MIPS_ARGREG,
        MIPS_ARGMEM,
        MIPS_TEMP,
};

/*
 * For function argument flags
 */
enum {
        MIPS_SAVE, MIPS_LOAD,
        MIPS_READ, MIPS_WRITE,
};

struct mips_function
{
        int last_temp;
        int temp_size;
        int para_size;
        int ast_instrs_size;
        struct mips_temp *temps;

        bool *reg_used;
        struct utillib_hashmap names;
        struct utillib_vector saved;

        uint32_t frame_size;
        uint32_t *address_map;
        struct mips_global *global;
        struct utillib_vector *instrs;
        size_t instr_begin;
};

struct mips_data 
{
        uint8_t type;
        char *label;
        union {
                char *string;
                size_t extend;
        };
};

struct mips_ir 
{
#define CLING_MIPS_OPERAND_MAX 3
        uint8_t opcode;
        union {
                uint8_t regid;
                int16_t offset;
                uint32_t address;
                int16_t imme16;
                int32_t imme32;
                uint32_t uimme32;
                char *label;
        } operands[CLING_MIPS_OPERAND_MAX];
};

void mips_program_init(struct mips_program *self, struct option const *option);
void mips_program_destroy(struct mips_program *self);
void mips_program_emit(struct mips_program *self,
                struct ast_program const *program);
void mips_program_print(struct mips_program const *self,
                FILE *file);
void mips_ir_fprint(struct mips_ir const *self, FILE *file);
void mips_ir_print(struct mips_ir const *self);

#endif /* CLING_MIPS_H */
