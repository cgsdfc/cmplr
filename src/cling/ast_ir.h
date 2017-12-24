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
#include <utillib/hashmap.h>

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>

UTILLIB_ENUM_BEGIN(cling_ast_opcode_kind)
UTILLIB_ENUM_ELEM(OP_NOP)
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
UTILLIB_ENUM_ELEM(OP_CAL)
UTILLIB_ENUM_ELEM(OP_DIV)
UTILLIB_ENUM_ELEM(OP_MUL)
UTILLIB_ENUM_ELEM(OP_EQ)
UTILLIB_ENUM_ELEM(OP_NE)
UTILLIB_ENUM_ELEM(OP_LT)
UTILLIB_ENUM_ELEM(OP_LE)
UTILLIB_ENUM_ELEM(OP_GT)
UTILLIB_ENUM_ELEM(OP_GE)
UTILLIB_ENUM_ELEM(OP_INDEX)
UTILLIB_ENUM_ELEM(OP_LDIMM)
UTILLIB_ENUM_ELEM(OP_LDSTR)
UTILLIB_ENUM_ELEM(OP_LDADR)
UTILLIB_ENUM_ELEM(OP_LDNAM)
UTILLIB_ENUM_ELEM(OP_DEREF)
UTILLIB_ENUM_ELEM(OP_LOAD)
UTILLIB_ENUM_ELEM(OP_STORE)
UTILLIB_ENUM_ELEM(OP_STADR)
UTILLIB_ENUM_ELEM(OP_STNAM)
UTILLIB_ENUM_ELEM(OP_WRSTR)
UTILLIB_ENUM_ELEM(OP_WRINT)
UTILLIB_ENUM_ELEM(OP_WRCHR)
UTILLIB_ENUM_ELEM(OP_RDCHR)
UTILLIB_ENUM_ELEM(OP_RDINT)
UTILLIB_ENUM_END(cling_ast_opcode_kind);

struct cling_ast_ir {
  int opcode;
  union {
    struct {
      int temp;
      char const *string;
    } ldstr;
    struct {
      int temp;
    } write;
    struct {
      int temp;
    } read;
    struct {
      int temp;
      int size;
    } push;
    struct {
      int temp;
      char const *name;
    } ldadr;
    struct {
      int temp;
      char const *name;
      int size;
    } ldnam;
    struct {
      int addr;
      int size;
    } deref;
    struct {
      int addr;
      int value;
      int size;
    } stadr;
    struct {
      char const *name;
      int value;
      int size;
    } stnam;
    struct {
      int temp;
      bool is_rvalue;
      int size;
      bool is_global;
      char const *name;
    } load;
    struct {
      int temp;
      int value;
      int size;
    } ldimm;
    struct {
      char const *name;
      size_t extend;
      int base_size;
    } defarr;
    struct {
      char const *name;
      int result;
      bool has_result;
    } call;
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
      int addr;
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

/*
 * Holds global information
 * that ast_ir need to access.
 * Notes the symbol_table will
 * be reentered.
 */
struct cling_ast_ir_global {
  struct cling_symbol_table *symbol_table;
  struct cling_option const *option;
  unsigned int temps;
};

struct cling_polish_ir {
  struct utillib_vector stack;
  struct utillib_vector opstack;
  struct cling_ast_ir_global *global;
};

/*
 * Holder of its own instrs.
 * Basic unit of code.
 */
struct cling_ast_function {
  char const *name;
  unsigned int temps;
  struct utillib_vector instrs;
  struct utillib_vector init_code;
  struct utillib_hashmap names;
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
                               struct cling_option const *option,
                               struct cling_symbol_table *symbol_table,
                               struct cling_ast_program *program);
void cling_ast_program_print(struct cling_ast_program const *self, FILE *file);

void cling_ast_ir_destroy(struct cling_ast_ir *self);
void ast_ir_fix_address(struct utillib_vector *instrs,
                        unsigned int const *address_map);
void ast_ir_print(struct cling_ast_ir const *self, FILE *file);
void ast_ir_vector_print(struct utillib_vector const *instrs,
                                FILE *file);

#endif /* CLING_AST_IR_H */
