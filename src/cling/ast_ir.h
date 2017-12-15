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
#ifndef CLING_AST_IR_H
#define CLING_AST_IR_H
#include <utillib/enum.h>
#include <utillib/vector.h>

#include <assert.h>
#include <stdio.h>
#include <inttypes.h>

UTILLIB_ENUM_BEGIN(cling_ast_opcode_kind)
UTILLIB_ENUM_ELEM(OP_DEFVAR)
UTILLIB_ENUM_ELEM(OP_DEFARR)
UTILLIB_ENUM_ELEM(OP_DEFUNC)
UTILLIB_ENUM_ELEM(OP_DEFCON)
UTILLIB_ENUM_ELEM(OP_PARA)
UTILLIB_ENUM_ELEM(OP_RET)
UTILLIB_ENUM_ELEM(OP_PUSH)
UTILLIB_ENUM_ELEM(OP_ADD)
UTILLIB_ENUM_ELEM(OP_SUB)
UTILLIB_ENUM_ELEM(OP_BEZ)
UTILLIB_ENUM_ELEM(OP_BNE)
UTILLIB_ENUM_ELEM(OP_JMP)
UTILLIB_ENUM_ELEM(OP_IDX)
UTILLIB_ENUM_ELEM(OP_CAL)
UTILLIB_ENUM_ELEM(OP_DIV)
UTILLIB_ENUM_ELEM(OP_MUL)
UTILLIB_ENUM_ELEM(OP_EQ)
UTILLIB_ENUM_ELEM(OP_NE)
UTILLIB_ENUM_ELEM(OP_LT)
UTILLIB_ENUM_ELEM(OP_LE)
UTILLIB_ENUM_ELEM(OP_GT)
UTILLIB_ENUM_ELEM(OP_GE)
UTILLIB_ENUM_ELEM(OP_STORE)
UTILLIB_ENUM_ELEM(OP_LDIMM)
UTILLIB_ENUM_ELEM(OP_LDSTR)
UTILLIB_ENUM_ELEM(OP_LOAD)
UTILLIB_ENUM_ELEM(OP_WRSTR)
UTILLIB_ENUM_ELEM(OP_WRINT)
UTILLIB_ENUM_ELEM(OP_READ)
UTILLIB_ENUM_END(cling_ast_opcode_kind);

UTILLIB_ENUM_BEGIN(cling_operand_info_kind)
UTILLIB_ENUM_ELEM_INIT(CL_NULL, 0)
UTILLIB_ENUM_ELEM_INIT(CL_BYTE, 1)
UTILLIB_ENUM_ELEM_INIT(CL_WORD, 2)
UTILLIB_ENUM_ELEM_INIT(CL_GLBL, 4)
UTILLIB_ENUM_ELEM_INIT(CL_LOCL, 8)
UTILLIB_ENUM_ELEM_INIT(CL_NAME, 16)
UTILLIB_ENUM_ELEM_INIT(CL_TEMP, 32)
UTILLIB_ENUM_ELEM_INIT(CL_IMME, 64)
UTILLIB_ENUM_ELEM_INIT(CL_LABL, 128)
UTILLIB_ENUM_ELEM_INIT(CL_STRG, 256)
UTILLIB_ENUM_END(cling_operand_info_kind);

/*
 * Holds global information
 * that ast_ir need to access.
 * Notes the symbol_table will
 * be reentered.
 */
struct cling_ast_ir_global {
  struct cling_symbol_table *symbol_table;
  unsigned int temps;
};

struct cling_ast_ir {
  int opcode;
  union {
    struct {
      int temp;
      char *string;
    } ldstr ;
    struct  {
      int temp;
    } write;
    struct {
      int temp;
      int size;
    } push;
    struct {
      int temp;
      bool is_rvalue;
      int size;
      bool is_global;
      char *name;
    } load;
    struct {
      int temp;
      int value;
      int size;
    } ldimm;
    struct {
      char *name;
      size_t extend;
      int base_size;
    } defarr;
    struct {
      char const *name;
      int result;
      bool has_result;
    } call;
    struct {
      int temp;
      int size;
    } read;
    struct {
      int addr;
      int value;
      int size;
    } store;
    struct {
      char const *name;
      int size;
      int value;
    } defcon;
    struct {
      int result;
      int temp1;
      int temp2;
    } binop;
    struct {
      int result;
      bool has_result;
    } ret;
    struct {
      int result;
      int base_size;
      bool is_rvalue;
      int array_addr;
      int index_result;
    } index;
    struct {
      char const *name;
      int size;
    } defvar;
    struct {
      char const *name;
      int return_size;
    } defunc;
    struct {
      char const *name;
      int size;
    } para;
    struct {
      int addr;
    } jmp;
    struct {
      int temp1;
      int temp2;
      int addr;
    } bne;
    struct {
      int temp;
      int addr;
    } bez;
  };

};

struct cling_polish_ir {
  /*
   * Stack holding ast nodes
   */
  struct utillib_vector stack;
  /*
   * Stack holding operands when evaluating.
   */
  struct utillib_vector opstack;
  /*
   * From which temps counter starts
   */
  struct cling_ast_ir_global *global;
};

/*
 * Holder of its own instrs.
 * Basic unit of code.
 */
struct cling_ast_function {
  char *name;
  unsigned int temps;
  struct utillib_vector instrs;
  struct utillib_vector init_code;
};

/*
 * Holder of all the functions
 * and some global init code.
 */
struct cling_ast_program {
  struct utillib_vector init_code;
  struct utillib_vector funcs;
};

void cling_ast_program_init(struct cling_ast_program *self);

void cling_ast_program_destroy(struct cling_ast_program *self);

void cling_ast_ir_emit_program(struct utillib_json_value const *self,
                               struct cling_symbol_table *symbol_table,
                               struct cling_ast_program *program);
void cling_ast_program_print(struct cling_ast_program const *self, FILE *file);

#endif /* CLING_AST_IR_H */
