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

/* 三、 中间代码格式 */
/* 原则上按照中缀表达式格式输出中间代码，
 * 即，形如x = y op z，其中x为结果，y为左操作数，z为右操作数，
 * op为操作符。以下根据基本语法现象举例说明。 */
/* 1. 函数声明 */
/* 源码形如： */
/* 	int foo( int a, int b, int c, int d) */
/* 中间代码： */
/* 	int foo() */
/* 	para int a */
/* 	para int b */
/* 	para int c */
/* 	para int d */
/* 2. 函数调用 */
/* 源码形如： */
/* 	i = tar(x,y) */
/* 中间代码： */
/* 	push x */
/* 	push y */
/* 	call tar */
/* 	i = RET */
/* 3. 函数返回 */
/* 源码形如： */
/* 	return (x) */
/* 中间代码： */
/* 	ret x */
/* 4. 变量声明 */
/* 源码形如： */
/* 	int i, j; */
/* 中间代码（符号表信息输出，程序中可不生成真正的中间代码）： */
/* 	var int i */
/* 	var int j */
/* 5. 常数声明 */
/* 源码形如： */
/* 	const int c = 10 */
/* 中间代码（符号表信息输出，程序中可不生成真正的中间代码）： */
/* 	const int c = 10 */
/* 6. 表达式 */
/* 源码形如： */
/* 	x = a * (b + c) */
/* 中间代码（可优化）： */
/* 	t1 = b + c */
/* 	t2 = a * t1 */
/* 	x = t2 */
/* 7. 条件判断 */
/* 源码形如： */
/* 	x == y */
/* 中间代码： */
/* 	x == y */
/* 8. 条件或无条件跳转 */
/* 中间代码： */
/* GOTO LABEL1 //无条件跳转到LABEL1 */
/* BNZ LABEL1 //满足条件跳转到LABEL1 */
/* BZ LABEL1 //不满足条件跳转到LABEL1 */
/* 9. 带标号语句 */
/* 源码形如： */
/* 	Label_1: x = a + b */
/* 中间代码： */
/* 	Label_1 : */
/* 	x = a + b */
/* 10. 数组赋值或取值 */
/* 源码形如： */
/* 	a[i] = b * c[j] */
/* 中间代码： */
/* 	t1 = c[j] */
/* t2 = b * t1 */
/* a[i] = t2 */
/* 11. 其他本文档未涉及到的语法现象，或者程序员自行定义的四元式操作，
 * 原则上均按照“x = y op z”形式的中缀表达式进行表达。 */

UTILLIB_ENUM_BEGIN(cling_ast_opcode_kind)
UTILLIB_ENUM_ELEM(OP_DEFVAR)
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
UTILLIB_ENUM_ELEM_INIT(CL_NULL,0)
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
 */
struct cling_ast_ir_global {
  struct cling_symbol_table const *symbol_table;
  unsigned int temps;
};

/*
 * text maybe a name, a imme
 * while scalar maybe an address,
 * a temp. Their meanings are decided
 * by the corresponding info field.
 */
union cling_ast_operand {
  char const *text;
  int scalar;
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
  union cling_ast_operand operands[CLING_AST_IR_MAX];
  int info[CLING_AST_IR_MAX];
};


/*
 * IR emission functions used by ast_ir.
 * Their operands are doced as well.
 */
struct cling_ast_ir * 
emit_ir(int type);

/*
 * read name
 */
struct cling_ast_ir *
emit_read(char const *name);

/*
 * write string | temp
 */

/*
 * defcon name(wide|scope) value
 */
struct cling_ast_ir *
emit_defcon(int type, char const *name,
   int scope_bit, char const *value);

/*
 * defvar name(wide|scope) [extend]
 */
struct cling_ast_ir *
emit_defvar(int type, char const* name, int scope_bit,
    char const *extend);

int emit_wide(int type);

/*
 * defunc name(wide)
 */
struct cling_ast_ir *
emit_defunc(int type, char const *name);

/*
 * para name(wide)
 */
struct cling_ast_ir *
emit_para(int type, char const *name);

/*
 * call name [temp = RET] 
 */
struct cling_ast_ir *
emit_call(int type, int value, char const *name);

/*
 * SYM_XXX => CL_WORD | CL_BYTE
 */
int emit_type(size_t type);

/*
 * nop
 */
struct cling_ast_ir *
emit_nop(void);

/*
 * push name | temp | imme(wide|scope)
 */

/*
 * jmp address-in-function
 */

/*
 * bez name | temp | imme address-in-function
 */
void cling_ast_ir_destroy(struct cling_ast_ir *self);

void cling_ast_ir_print(struct utillib_vector const *instrs);


#endif /* CLING_IR_H */
