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

UTILLIB_ENUM_BEGIN(cling_mips_opcode_kind)
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
UTILLIB_ENUM_END(cling_mips_opcode_kind);

UTILLIB_ENUM_BEGIN(cling_mips_regster)
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
UTILLIB_ENUM_END(cling_mips_regster);

UTILLIB_ENUM_BEGIN(cling_mips_syscall_kind)
UTILLIB_ENUM_ELEM_INIT(MIPS_PRINT_INT, 1)
UTILLIB_ENUM_ELEM_INIT(MIPS_PRINT_STRING, 4)
UTILLIB_ENUM_ELEM_INIT(MIPS_PRINT_CHAR, 11)
UTILLIB_ENUM_ELEM_INIT(MIPS_READ_INT, 5)
UTILLIB_ENUM_ELEM_INIT(MIPS_READ_STRING, 8)
UTILLIB_ENUM_ELEM_INIT(MIPS_READ_CHAR, 12)
UTILLIB_ENUM_ELEM_INIT(MIPS_EXIT, 10)
UTILLIB_ENUM_END(cling_mips_syscall_kind);

/*
 * These are in fact assembler directives.
 * But all we concern about is the part used
 * to define global data such as .asciiz, .byte
 * and .word, so as the name of it.
 */
UTILLIB_ENUM_BEGIN(cling_mips_data_kind)
UTILLIB_ENUM_ELEM(MIPS_ASCIIZ)
UTILLIB_ENUM_ELEM(MIPS_SPACE)
UTILLIB_ENUM_ELEM(MIPS_WORD)
UTILLIB_ENUM_ELEM(MIPS_BYTE)
UTILLIB_ENUM_END(cling_mips_data_kind);

UTILLIB_ENUM_BEGIN(cling_mips_ecode)
UTILLIB_ENUM_ELEM(MIPS_EC_OK)
UTILLIB_ENUM_ELEM(MIPS_EC_EXIT)
UTILLIB_ENUM_ELEM(MIPS_EC_ALIGN)
UTILLIB_ENUM_ELEM(MIPS_EC_BTAKEN)
UTILLIB_ENUM_END(cling_mips_ecode);

struct cling_ast_ir;
struct cling_ast_function;
struct cling_ast_program;

/*
 * Represent a segment of code
 * with a label.
 */
struct cling_address_range {
  union {
    char const *label;
    size_t scalar;
  };
  uint32_t begin;
  uint32_t end;
};

struct cling_mips_label {
  char *label;
  uint32_t address;
};

struct cling_mips_program {
  struct utillib_vector text;
  struct utillib_vector data;
  struct utillib_hashmap labels;
};

struct cling_mips_global {
  struct utillib_vector *data;
  struct utillib_hashmap *labels;
  int label_count;
};

struct cling_mips_name {
  uint8_t regid;
  uint32_t offset;
};

struct cling_mips_temp {
  int age;
  uint8_t regid;
  uint32_t offset;
};

struct cling_mips_function {
  int temp_size;
  int para_size;
  struct cling_mips_temp *temps;

  bool *reg_pool;
  struct utillib_hashmap names;
  struct utillib_vector saved_registers;

  uint32_t frame_size;
  uint32_t *address_map;
  struct cling_mips_global *global;
  struct utillib_vector *instrs;
  size_t instr_begin;
};

struct cling_mips_data {
  uint8_t type;
  char *label;
  union {
    char *string;
    size_t extend;
  };
};

struct cling_mips_ir {
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

void cling_mips_program_init(struct cling_mips_program *self,
                             struct cling_ast_program const *program);
void cling_mips_program_destroy(struct cling_mips_program *self);
void cling_mips_program_emit(struct cling_mips_program *self,
                             struct cling_ast_program const *program);
void cling_mips_program_print(struct cling_mips_program const *self,
                              FILE *file);

/*
 * mips_label
 */
struct cling_mips_label *mips_label_create(char const *label, uint32_t address);
void mips_label_destroy(struct cling_mips_label *self);
struct cling_mips_label *
mips_label_name_find(struct utillib_hashmap const *self, char const *name);
extern const struct utillib_hashmap_callback mips_label_strcallback;

#endif /* CLING_MIPS_H */
