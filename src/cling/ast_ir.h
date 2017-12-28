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
UTILLIB_ENUM_ELEM(OP_STADR)
UTILLIB_ENUM_ELEM(OP_STNAM)
UTILLIB_ENUM_ELEM(OP_READ)
UTILLIB_ENUM_ELEM(OP_WRITE)
UTILLIB_ENUM_END(cling_ast_opcode_kind);

enum { CL_LVALUE, CL_RVALUE, };
enum {
  OP_WRSTR,
  OP_WRINT,
  OP_WRCHR,
  OP_RDCHR,
  OP_RDINT,
};

/*
 * Holds global information
 * that ast_ir need to access.
 * Notes the symbol_table will
 * be reentered.
 */
struct cling_ast_ir_global {
  struct cling_symbol_table *symbol_table;
  struct utillib_vector *instrs;
  unsigned int temps;
};

/*
 * Opposed to __cdecl, MIPS requires the first argument be
 * accessed from the top of the stack and so on.
 * +---------+
 * |         | argn sp+8
 * +---------+ ...
 * |         | arg1 sp+4
 * +---------+
 * |         | arg0 <-- sp
 * +---------+
 */
struct cling_ast_ir {
  int opcode;
  union {
    struct {
    /*
     * load string into temp
     */
      int temp;
      char const *string;
    } ldstr;
    struct {
      /*
       * write temp to stdout depending on opcode
       */
      int temp;
      int kind;
    } write;
    struct {
      /*
       * read into temp depending on opcode
       */
      int temp;
      int kind;
    } read;
    struct {
      /*
       * load address of name into temp
       */
      int temp;
      char const *name;
      int scope;
    } ldadr;
    struct {
      /*
       * load value of name into temp
       */
      int temp;
      char const *name;
      int size;
      int scope;
    } ldnam;
    struct {
      /*
       * load value of addr into addr
       */
      int addr;
      int size;
    } deref;
    struct {
      /*
       * store value into addr
       */
      int addr;
      int value;
      int size;
    } stadr;
    struct {
      /*
       * store value into named addr
       */
      char const *name;
      int value;
      int size;
    } stnam;
    struct {
      /*
       * load immediate value into temp
       */
      int temp;
      int value;
      int size;
    } ldimm;
    struct {
      /*
       * define an array
       * later referred as temp
       */
      char const *name;
      size_t extend;
      int base_size;
      int temp;
    } defarr;
    struct {
      /*
       * call name and store the result
       * if any
       */
      char const *name;
      int argc;
      int *argv;
      int result;
      bool has_result;
    } call;
    struct {
      /*
       * define a const having no
       * effect on codegen
       */
      char const *name;
      int size;
      int value;
    } defcon;
    struct {
      /*
       * do a binary op
       */
      int result;
      int temp1;
      int temp2;
    } binop;
    struct {
      /*
       * jump to addr (exit) and store a value
       * in result
       */
      int result;
      bool has_result;
      int addr;
    } ret;
    struct {
      /*
       * get the addr of an element from array
       */
      int result;
      int base_size;
      bool is_rvalue;
      int array_addr;
      int index_result;
    } index;
    struct {
      /*
       * define a variable referred as temp
       */
      int temp;
      char const *name;
      int size;
    } defvar;
    struct {
      /*
       * define a function having no meaning
       * to codegen.
       */
      char const *name;
      int return_size;
    } defunc;
    struct {
      /*
       * define a parameter of this function
       * referred as temp
       */
      int temp;
      char const *name;
      int size;
    } para;
    struct {
      /*
       * jump to function addr
       */
      int addr;
    } jmp;
    struct {
      /*
       * jump to function addr if temp1 != temp2
       */
      int temp1;
      int temp2;
      int addr;
    } bne;
    struct {
      /*
       * jump to function addr if temp==0
       */
      int temp;
      int addr;
    } bez;
  };
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

void cling_ast_ir_emit_program(struct cling_ast_program *self,
                               struct utillib_json_value const *object,
                               struct cling_symbol_table *symbol_table);

void cling_ast_program_print(struct cling_ast_program const *self, FILE *file);

void cling_ast_ir_destroy(struct cling_ast_ir *self);
void ast_ir_fix_address(struct utillib_vector *instrs,
                        unsigned int const *address_map);
void ast_ir_print(struct cling_ast_ir const *self, FILE *file);
void ast_ir_vector_print(struct utillib_vector const *instrs,
                                FILE *file);

#endif /* CLING_AST_IR_H */
