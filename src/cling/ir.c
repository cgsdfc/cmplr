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
#include "ir.h"
#include "misc.h"
#include "symbol_table.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

UTILLIB_ETAB_BEGIN(cling_ast_opcode_kind)
UTILLIB_ETAB_ELEM_INIT(OP_DEFVAR, "var")
UTILLIB_ETAB_ELEM(OP_DEFUNC)
UTILLIB_ETAB_ELEM(OP_DEFARR)
UTILLIB_ETAB_ELEM_INIT(OP_DEFCON, "const")
UTILLIB_ETAB_ELEM_INIT(OP_PARA, "para")
UTILLIB_ETAB_ELEM_INIT(OP_RET, "ret")
UTILLIB_ETAB_ELEM_INIT(OP_PUSH, "push")
UTILLIB_ETAB_ELEM_INIT(OP_ADD, "+")
UTILLIB_ETAB_ELEM_INIT(OP_SUB, "-")
UTILLIB_ETAB_ELEM(OP_IDX)
UTILLIB_ETAB_ELEM_INIT(OP_CAL, "call")
UTILLIB_ETAB_ELEM_INIT(OP_DIV, "/")
UTILLIB_ETAB_ELEM_INIT(OP_MUL, "*")
UTILLIB_ETAB_ELEM_INIT(OP_EQ, "==")
UTILLIB_ETAB_ELEM_INIT(OP_NE, "!=")
UTILLIB_ETAB_ELEM_INIT(OP_LT, "<")
UTILLIB_ETAB_ELEM_INIT(OP_LE, "<=")
UTILLIB_ETAB_ELEM_INIT(OP_GT, ">")
UTILLIB_ETAB_ELEM_INIT(OP_GE, ">=")
UTILLIB_ETAB_ELEM_INIT(OP_BNZ, "bnz")
UTILLIB_ETAB_ELEM_INIT(OP_BEZ, "bez")
UTILLIB_ETAB_ELEM_INIT(OP_BNE, "bne")
UTILLIB_ETAB_ELEM_INIT(OP_JMP, "jmp")
UTILLIB_ETAB_ELEM_INIT(OP_STORE, "store")
UTILLIB_ETAB_ELEM_INIT(OP_LDARR, "ldarr")
UTILLIB_ETAB_ELEM_INIT(OP_LDIMM, "ldimm")
UTILLIB_ETAB_ELEM_INIT(OP_LDVAR, "ldvar")
UTILLIB_ETAB_ELEM_INIT(OP_WRITE, "write")
UTILLIB_ETAB_ELEM_INIT(OP_READ, "read")
UTILLIB_ETAB_ELEM_INIT(OP_NOP, "nop")
UTILLIB_ETAB_END(cling_ast_opcode_kind);

UTILLIB_ETAB_BEGIN(cling_operand_info_kind)
UTILLIB_ETAB_ELEM(CL_STRG)
UTILLIB_ETAB_ELEM(CL_NAME)
UTILLIB_ETAB_ELEM(CL_TEMP)
UTILLIB_ETAB_ELEM(CL_IMME)
UTILLIB_ETAB_ELEM(CL_LABL)
UTILLIB_ETAB_ELEM(CL_GLBL)
UTILLIB_ETAB_ELEM(CL_LOCL)
UTILLIB_ETAB_ELEM(CL_BYTE)
UTILLIB_ETAB_ELEM(CL_WORD)
UTILLIB_ETAB_ELEM(CL_NULL)
UTILLIB_ETAB_END(cling_operand_info_kind);

const struct cling_ast_ir cling_ast_ir_nop = {.opcode = OP_NOP};

/*
 *  0 stands for global,
 *  1 stands for local.
 */
static int emit_scope(int scope_kind) { return scope_kind ? CL_LOCL : CL_GLBL; }

void init_null(struct cling_ast_operand *self) { self->info = CL_NULL; }

void init_temp(struct cling_ast_operand *self, int scalar) {
  self->info = CL_TEMP | CL_WORD;
  self->scalar = scalar;
}

void init_name(struct cling_ast_operand *self, int scope, int type,
               char const *name) {
  self->info = CL_NAME | cling_type_to_wide(type) | emit_scope(scope);
  self->text = strdup(name);
}

void init_imme(struct cling_ast_operand *self, int type, char const *value) {
  self->info = CL_IMME | cling_type_to_wide(type);
  switch (type) {
  case CL_INT:
    sscanf(value, "%d", &self->imme_int);
    return;
  case CL_CHAR:
    sscanf(value, "%c", &self->imme_char);
    return;
  }
}

void init_string(struct cling_ast_operand *self, char const *string) {
  self->text = strdup(string);
  self->info = CL_STRG;
}

struct cling_ast_ir *emit_ir(int opcode) {
  struct cling_ast_ir *self = calloc(sizeof *self, 1);
  self->opcode = opcode;
  return self;
}

struct cling_ast_ir *emit_defarr(int type, char const *name, int scope,
                                 char const *extend) {
  struct cling_ast_ir *self = emit_ir(OP_DEFARR);
  init_name(&self->operands[0], scope, type, name);
  init_imme(&self->operands[1], CL_INT, extend);
  return self;
}

struct cling_ast_ir *emit_call(char const *name, int maybe_temp) {
  struct cling_ast_ir *self = emit_ir(OP_CAL);
  init_name(&self->operands[0], CL_GLBL, CL_NULL, name);
  if (maybe_temp == -1) {
    init_null(&self->operands[1]);
    return self;
  }
  init_temp(&self->operands[1], maybe_temp);
  return self;
}

struct cling_ast_ir *emit_read(char const *name, int type, int scope) {
  struct cling_ast_ir *self = emit_ir(OP_READ);
  init_name(&self->operands[0], scope, type, name);
  return self;
}

struct cling_ast_ir *emit_defcon(int type, char const *name, int scope,
                                 char const *value) {
  struct cling_ast_ir *self = emit_ir(OP_DEFCON);
  init_name(&self->operands[0], scope, type, name);
  init_imme(&self->operands[1], type, value);
  return self;
}

struct cling_ast_ir *emit_defvar(int type, char const *name, int scope) {
  struct cling_ast_ir *self = emit_ir(OP_DEFVAR);
  init_name(&self->operands[0], scope, type, name);
  return self;
}

struct cling_ast_ir *emit_defunc(int type, char const *name) {
  struct cling_ast_ir *self = emit_ir(OP_DEFUNC);
  init_name(&self->operands[0], CL_GLBL, type, name);
  return self;
}

struct cling_ast_ir *emit_para(int type, char const *name) {
  struct cling_ast_ir *self = emit_ir(OP_PARA);
  init_name(&self->operands[0], CL_LOCL, type, name);
  return self;
}

struct cling_ast_ir *emit_ldarr(char const *name, int scope, int temp) {
  struct cling_ast_ir *self = emit_ir(OP_LDARR);
  init_name(&self->operands[0], scope, CL_NULL, name);
  init_temp(&self->operands[1], temp);
  return self;
}

struct cling_ast_ir *emit_ldimm(char const *value, int type, int temp) {
  struct cling_ast_ir *self = emit_ir(OP_LDIMM);
  init_imme(&self->operands[0], type, value);
  init_temp(&self->operands[1], temp);
  return self;
}

struct cling_ast_ir *emit_ldvar(char const *name, int scope, int type,
                                int temp) {
  struct cling_ast_ir *self = emit_ir(OP_LDVAR);
  init_name(&self->operands[0], scope, type, name);
  init_temp(&self->operands[1], temp);
  return self;
}

void cling_ast_ir_destroy(struct cling_ast_ir *self) {
  if (self == &cling_ast_ir_nop)
    return;
  for (int i = 0; i < CLING_AST_IR_MAX; ++i) {
    int info = self->operands[i].info;
    if (info & CL_NAME || info & CL_STRG)
      free(self->operands[i].text);
  }
  free(self);
}

static char const *imme_tostring(struct cling_ast_operand const *self) {
  int info = self->info;
  static char buffer[128];
  if (info & CL_WORD) {
    snprintf(buffer, 128, "%d", self->imme_int);
    return buffer;
  }
  if (info & CL_BYTE) {
    snprintf(buffer, 128, "\'%c\'", self->imme_char);
    return buffer;
  }
  assert(false);
}

static char const *wide_tostring(int wide) {
  if (wide & CL_WORD)
    return "int";
  if (wide & CL_BYTE)
    return "char";
  return "void";
}

char const *cling_ast_ir_tostring(struct cling_ast_ir const *self) {
#define AST_IR_BUFSIZ 128
  static char buffer[AST_IR_BUFSIZ];
  char const *opstr = cling_ast_opcode_kind_tostring(self->opcode);
  struct cling_ast_operand const *operand = self->operands;

  switch (self->opcode) {
  case OP_DEFCON:
    snprintf(buffer, AST_IR_BUFSIZ, "const %s %s = %s",
             wide_tostring(operand[0].info), operand[0].text,
             imme_tostring(&operand[1]));
    break;
  case OP_PARA:
  case OP_DEFVAR:
    snprintf(buffer, AST_IR_BUFSIZ, "var %s %s", wide_tostring(operand[0].info),
             operand[0].text);
    break;
  case OP_DEFARR:
    snprintf(buffer, AST_IR_BUFSIZ, "var %s %s[%s]",
             wide_tostring(operand[0].info), operand[0].text,
             imme_tostring(&operand[1]));
    break;
  case OP_CAL:
    if (operand[1].info == CL_NULL)
      snprintf(buffer, AST_IR_BUFSIZ, "call %s", operand[0].text);
    else
      snprintf(buffer, AST_IR_BUFSIZ, "t%d = call %s", operand[1].scalar,
               operand[0].text);
    break;
  case OP_ADD:
  case OP_SUB:
  case OP_IDX:
  case OP_DIV:
  case OP_MUL:
  case OP_EQ:
  case OP_NE:
  case OP_LT:
  case OP_LE:
  case OP_GT:
  case OP_GE:
    snprintf(buffer, AST_IR_BUFSIZ, "t%d = t%d %s t%d", operand[0].scalar,
             operand[1].scalar, opstr, operand[2].scalar);
    break;
  case OP_BEZ:
    snprintf(buffer, AST_IR_BUFSIZ, "bez t%d %d", operand[0].scalar,
             operand[1].scalar);
    break;
  case OP_BNE:
    snprintf(buffer, AST_IR_BUFSIZ, "bne t%d t%d %d", operand[0].scalar,
             operand[1].scalar, operand[2].scalar);
    break;
  case OP_JMP:
    snprintf(buffer, AST_IR_BUFSIZ, "jmp %d", operand[0].scalar);
    break;
  case OP_DEFUNC:
    snprintf(buffer, AST_IR_BUFSIZ, "%s %s()", wide_tostring(operand[0].info),
             operand[0].text);
    break;
  case OP_RET:
  case OP_NOP:
    return opstr;
  case OP_READ:
    snprintf(buffer, AST_IR_BUFSIZ, "read %s", operand[0].text);
    break;
  case OP_WRITE:
    if (operand[0].info == CL_STRG) {
      snprintf(buffer, AST_IR_BUFSIZ, "write \"%s\"", operand[0].text);
    } else if (operand[0].info & CL_TEMP) {
      /*
       * TODO: add ldstr to load this string.
       */
      snprintf(buffer, AST_IR_BUFSIZ, "write t%d", operand[0].scalar);
    }
    break;
  case OP_STORE:
    snprintf(buffer, AST_IR_BUFSIZ, "store t%d t%d", operand[0].scalar,
             operand[1].scalar);
    break;
  case OP_PUSH:
    snprintf(buffer, AST_IR_BUFSIZ, "push t%d", operand[0].scalar);
    break;
  case OP_LDARR:
  case OP_LDVAR:
    snprintf(buffer, AST_IR_BUFSIZ, "%s %s t%d", opstr, operand[0].text,
             operand[1].scalar);
    break;
  case OP_LDIMM:
    snprintf(buffer, AST_IR_BUFSIZ, "ldimm %s t%d", imme_tostring(&operand[0]),
             operand[1].scalar);
    break;
  default:
    puts(cling_ast_opcode_kind_tostring(self->opcode));
    assert(false);
  }
  return buffer;
}

void cling_ast_ir_print(struct utillib_vector const *instrs) {
  int i = 0;
  struct cling_ast_ir const *ir;
  UTILLIB_VECTOR_FOREACH(ir, instrs) {
    printf("%d: %s\n", i, cling_ast_ir_tostring(ir));
    ++i;
  }
}
