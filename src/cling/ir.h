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
#ifndef CLING_IR_H
#define CLING_IR_H
#include <utillib/enum.h>
#include <utillib/vector.h>

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
UTILLIB_ENUM_ELEM(OP_BNZ)
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
UTILLIB_ENUM_ELEM(OP_LDARR)
UTILLIB_ENUM_ELEM(OP_LDIMM)
UTILLIB_ENUM_ELEM(OP_LDVAR)
UTILLIB_ENUM_ELEM(OP_LDADR)
UTILLIB_ENUM_ELEM(OP_LDVAL)
UTILLIB_ENUM_ELEM(OP_WRITE)
UTILLIB_ENUM_ELEM(OP_READ)
UTILLIB_ENUM_ELEM(OP_NOP)
UTILLIB_ENUM_END(cling_ast_opcode_kind);

/*
 * This ir is still very far from
 * actual ASM code since its preference
 * to label and iden rather than number
 * and address. It might not be possible
 * to execute the ir but it is good at
 * decripting the structure of the program
 * in a form similar to instruction.
 */

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

/*
 * text maybe a name, a imme
 * while scalar maybe an address,
 * a temp. Their meanings are decided
 * by the corresponding info field.
 */
struct cling_ast_operand {
  int info;
  union {
    char *text;
    int scalar;
    char imme_char;
    int imme_int;
  };
};

/*
 * operands holds content which
 * is only meaningful under the
 * description of info, which takes
 * value from `cling_operand_info_kind'.
 */
struct cling_ast_ir {
#define CLING_AST_IR_MAX 3
  int opcode;
  struct cling_ast_operand operands[CLING_AST_IR_MAX];
};

/*
 * Init routines for various kinds of operands.
 */

void init_imme(struct cling_ast_operand *self, int wide, char const *value);

void init_null(struct cling_ast_operand *self);

void init_string(struct cling_ast_operand *self, char const *string);

void init_temp(struct cling_ast_operand *self, int scalar);

/*
 * IR emission functions used by ast_ir.
 * Their operands are doced as well.
 */
struct cling_ast_ir *emit_ir(int type);

/*
 * read name
 */
struct cling_ast_ir *emit_read(char const *name, int wide, int scope_bit);

/*
 * write string | temp
 */

/*
 * defcon name(wide|scope) value
 */
struct cling_ast_ir *emit_defcon(int wide, char const *name, int scope_bit,
                                 char const *value);

/*
 * defvar name(wide|scope)
 */
struct cling_ast_ir *emit_defvar(int wide, char const *name, int scope_bit);

/*
 * defarr name(wide|scope) extend
 */
struct cling_ast_ir *emit_defarr(int wide, char const *name, int scope_bit,
                                 char const *extend);

/*
 * defunc name(wide)
 */
struct cling_ast_ir *emit_defunc(int wide, char const *name);

/*
 * para name(wide)
 */
struct cling_ast_ir *emit_para(int wide, char const *name);

/*
 * call name [temp|name = RET]
 */

struct cling_ast_ir *emit_call(char const *name, int maybe_temp);

/*
 * nop
 */
extern const struct cling_ast_ir cling_ast_ir_nop;

/*
 * push temp
 */

/*
 * jmp address-in-function
 */

/*
 * bez temp address-in-function
 */

/*
 * ldarr name scope
 */
struct cling_ast_ir *emit_ldarr(char const *name, int scope, int temp);

/*
 * ldimm value(wide) temp
 */
struct cling_ast_ir *emit_ldimm(char const *value, int wide, int temp);

/*
 * ldvar name(scope|wide) temp
 */
struct cling_ast_ir *emit_ldvar(char const *name, int scope, int wide,
                                int temp);
/*
 * ldadr name temp
 */
struct cling_ast_ir *emit_ldadr(char const *name, int scope, int wide,
                                int temp);

/*
 * ldval name temp
 */
struct cling_ast_ir *emit_ldval(char const *name, int scope, int wide,
                                int temp);

void cling_ast_ir_destroy(struct cling_ast_ir *self);

void cling_ast_ir_print(struct utillib_vector const *instrs);

#endif /* CLING_IR_H */
