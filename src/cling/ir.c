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
#include "symbols.h"
#include "symbol_table.h"
#include <utillib/json.h>

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

UTILLIB_ETAB_BEGIN(cling_ast_opcode_kind)
UTILLIB_ETAB_ELEM_INIT(OP_DEFVAR, "var")
UTILLIB_ETAB_ELEM(OP_DEFUNC)
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

static void set_text(struct cling_ast_ir *self, int index, char const* text)
{
  self->operands[index].text=strdup(text);
}

int emit_wide(int type) {
  if (type & CL_INT)
    return CL_WORD;
  if (type & CL_CHAR)
    return CL_BYTE;
  if (type & CL_VOID)
    return CL_NULL;
  switch(type) {
    case SYM_INTEGER:
    case SYM_KW_INT:
    case SYM_UINT:
      return CL_WORD;
    case SYM_CHAR:
    case SYM_KW_CHAR:
      return CL_BYTE;
    default:
      return CL_NULL;
  }
}

void emit_factor(
    struct cling_ast_ir *self, int index,
    struct utillib_json_value const* var,
    struct cling_symbol_table const* symbol_table)
{
  struct utillib_json_value *value, *type;
  struct cling_symbol_entry *entry;
  char const *imme, *name;
  int wide;
  int scope_bit;

  /*
   * We distinguish between name, imme.
   * A literal and a named constant is imme
   * while name is the variable.
   * Notes the term imme does not mean it must
   * goes into instruction field.
   */
  type=utillib_json_object_at(var, "type");
  value=utillib_json_object_at(var, "value");
  switch(type->as_size_t) {
    case SYM_IDEN:
      entry=cling_symbol_table_find(symbol_table, value->as_ptr, CL_LEXICAL);
      if (entry->kind & CL_CONST) {
        /*
         * Here, we transfer named constant 
         * to imme.
         */
        value=utillib_json_object_at(entry->value, "value");
        imme=value->as_ptr;
        wide=emit_wide(entry->kind);
        goto handle_imme;
      }
      if (cling_symbol_table_find(symbol_table, value->as_ptr, CL_GLOBAL))
        scope_bit=CL_GLBL;
      else
        scope_bit=CL_LOCL;
      name=value->as_ptr;
      wide=emit_wide(entry->kind);
      goto handle_name;
    case SYM_INTEGER:
    case SYM_UINT:
      wide=CL_WORD;
      imme=value->as_ptr;
      goto handle_imme;
    case SYM_CHAR:
      wide=CL_BYTE;
      imme=value->as_ptr;
      goto handle_imme;
  }
handle_name:
  self->info[index]=CL_NAME|wide|scope_bit;
  set_text(self, index, name);
  return;

handle_imme:
  self->info[index]=CL_IMME|wide;
  set_text(self, index, imme);
  return;
}

inline struct cling_ast_ir *
emit_nop(void) {
  return emit_ir(OP_NOP);
}

struct cling_ast_ir *
emit_ir(int opcode) {
  struct cling_ast_ir *self=malloc(sizeof *self);
  self->opcode=opcode;
  return self;
}

struct cling_ast_ir *
emit_call(int type, int value, char const *name)
{
  struct cling_ast_ir *self=emit_ir(OP_CAL);
  set_text(self, 0, name);
  self->info[0]=CL_NAME;
  self->info[1]=type;
  if (self->info[1] == CL_NULL)
    return self;
  self->info[1] |= CL_TEMP;
  self->operands[1].scalar=value;
  return self;
}


struct cling_ast_ir *
emit_read(char const *name) {
  struct cling_ast_ir *self=emit_ir(OP_READ);
  set_text(self, 0, name);
  return self;
}

struct cling_ast_ir *
emit_defcon(int type, char const *name, 
    int scope_bit, char const *value) {
  struct cling_ast_ir *self=emit_ir(OP_DEFCON);
  self->info[0]=emit_wide(type) | scope_bit;
  set_text(self, 0, name);
  set_text(self, 1, value);
  return self;
}

struct cling_ast_ir *
emit_defvar(int type, char const* name, int scope_bit,
    char const *extend) {
  struct cling_ast_ir *self=emit_ir(OP_DEFVAR);
  self->info[0]=emit_wide(type) | scope_bit;
  self->info[1]=extend?1:0;
  set_text(self, 0, name);
  if (extend) {
    self->info[1]=1;
    set_text(self, 1, extend);
  } else {
    self->info[1]=0;
  }
  return self;
}

struct cling_ast_ir *
emit_defunc(int type, char const *name) {
  struct cling_ast_ir *self=emit_ir(OP_DEFUNC);
  self->info[0]=emit_wide(type);
  set_text(self, 0, name);
  return self;
}

struct cling_ast_ir *
emit_para(int type, char const *name) {
  struct cling_ast_ir *self=emit_ir(OP_PARA);
  self->info[0]=emit_wide(type);
  set_text(self, 0, name);
  return self;
}

void cling_ast_ir_destroy(struct cling_ast_ir *self) {
  free(self);
}

static char const* wide_tostring(int wide) {
  if (wide & CL_WORD)
    return "int";
  if (wide & CL_BYTE)
    return "char";
  return "void";
}

static void defcon_tostring(struct cling_ast_ir const *self,
    struct utillib_string *string)
{
  utillib_string_append(string, "const ");
  utillib_string_append(string, wide_tostring(self->info[0]));
  utillib_string_append(string, " ");
  utillib_string_append(string, self->operands[0].text);
  utillib_string_append(string, " = ");
  utillib_string_append(string, self->operands[1].text);
}

static void defvar_tostring(struct cling_ast_ir const *self,
    struct utillib_string *string)
{
  utillib_string_append(string, "var ");
  utillib_string_append(string, wide_tostring(self->info[0]));
  utillib_string_append(string, " ");
  utillib_string_append(string, self->operands[0].text);
  if (self->info[1]) {
    utillib_string_append(string, "[");
    utillib_string_append(string, self->operands[1].text);
    utillib_string_append(string, "]");
  }
}

static void defunc_tostring(struct cling_ast_ir const *self,
    struct utillib_string *string)
{
  int type=self->info[0];
  utillib_string_append(string,wide_tostring(self->info[0]));
  utillib_string_append(string, " ");
  utillib_string_append(string, self->operands[0].text);
  utillib_string_append(string, "()");
}

static void scalar_tostring(struct cling_ast_ir const *self,
    int index,
    struct utillib_string *string)
{
  char buf[128];
  snprintf(buf, sizeof buf, "%d", self->operands[index].scalar);
  utillib_string_append(string, buf);
}

static void factor_tostring(struct cling_ast_ir const *self,
    int index,
    struct utillib_string *string)
{
  int info=self->info[index];
  if (info & CL_TEMP) {
    utillib_string_append(string, "t");
    scalar_tostring(self, index, string);
    return;
  }
  if (info & CL_IMME || info & CL_NAME) {
    utillib_string_append(string, self->operands[index].text);
    return;
  }
  if (info == CL_STRG) {
    utillib_string_append(string, "\"");
    utillib_string_append(string, self->operands[index].text);
    utillib_string_append(string, "\"");
    return;
  }

  printf("%d\n", self->info[index]);
  assert(false);
}
      
static void binary_tostring(struct cling_ast_ir const *self,
    struct utillib_string *string)
{
  int op=self->opcode;
  char const *opstr=cling_ast_opcode_kind_tostring(op);
  /*
   * To suit the weird need of the ir,
   * asking for different forms unreasonably,
   * we have to judge the op a little bit.
   */
  switch(op) {
    case OP_CAL:
      utillib_string_append(string, "call ");
      factor_tostring(self, 0, string);
      if (self->info[1] == CL_NULL)
        return;
      /*
       * If it has something to return,
       * a `t1 = RET' will be printed'.
       */
      utillib_string_append(string, " RET = ");
      factor_tostring(self, 1, string);
      return;
    case OP_IDX:
      /*
       * idx t2 A index => t2 = A[index].
       */
      factor_tostring(self, 0, string);
      utillib_string_append(string, " = ");
      utillib_string_append(string, self->operands[1].text);
      utillib_string_append(string, "[");
      factor_tostring(self, 2, string);
      utillib_string_append(string, "]");
      return;
    default:
      /*
       * OP_DIV t2 X t1 => t2 = X / t1.
       */
      factor_tostring(self, 0, string);
      utillib_string_append(string, " = ");
      factor_tostring(self, 1, string);
      utillib_string_append(string, opstr);
      factor_tostring(self, 2, string);
      return;
  }
}

static void push_tostring(struct cling_ast_ir const* self,
    struct utillib_string *string)
{
  utillib_string_append(string, "push ");
  factor_tostring(self, 0, string);
}

static void para_tostring(struct cling_ast_ir const* self,
    struct utillib_string *string)
{
  utillib_string_append(string, "para ");
  utillib_string_append(string, wide_tostring(self->info[0]));
  utillib_string_append(string, " ");
  utillib_string_append(string, self->operands[0].text);
}

/*
 * If no expression, info[0]=CL_NULL,
 * else info[0] decripts the operand
 * and operands[0] contains real stuff.
 */
static void ret_tostring(struct cling_ast_ir const* self,
    struct utillib_string *string)
{
  utillib_string_append(string, "ret ");
  if (self->info[0]!=CL_NULL) {
    factor_tostring(self, 0, string);
  }
}

static void branch_tostring(struct cling_ast_ir const* self,
    struct utillib_string *string) {
  char const* buf;
  utillib_string_append(string,
      cling_ast_opcode_kind_tostring(self->opcode));
  utillib_string_append(string, " ");
  switch(self->opcode) {
    case OP_BEZ:
    case OP_BNZ:
      factor_tostring(self, 0, string);
      utillib_string_append(string, " ");
      scalar_tostring(self, 1, string);
      return;
    case OP_BNE:
      factor_tostring(self, 0, string);
      utillib_string_append(string, " ");
      factor_tostring(self, 1, string);
      utillib_string_append(string, " ");
      scalar_tostring(self, 2, string);
      return;
    case OP_JMP:
      scalar_tostring(self, 0, string);
      return;
  }
}

static void read_tostring(struct cling_ast_ir const *self,
    struct utillib_string *string) {
  utillib_string_append(string, "read ");
  utillib_string_append(string, self->operands[0].text);
}

static void write_tostring(struct cling_ast_ir const* self,
    struct utillib_string *string) {
  utillib_string_append(string, "write ");
  factor_tostring(self, 0, string);
}

/*
 * store addr | name value
 * Stores the value to name or addr.
 */
static void store_tostring(struct cling_ast_ir const* self,
    struct utillib_string *string) {
  utillib_string_append(string, "store ");
  factor_tostring(self, 0, string);
  utillib_string_append(string, " ");
  factor_tostring(self, 1, string);
}

void cling_ast_ir_tostring(struct cling_ast_ir const* self,
    struct utillib_string *string)
{
  switch(self->opcode) {
    case OP_DEFCON:
      defcon_tostring(self, string);
      return;
    case OP_DEFVAR:
      defvar_tostring(self, string);
      return;
    case OP_ADD:
    case OP_SUB:
    case OP_IDX:
    case OP_CAL:
    case OP_DIV:
    case OP_MUL:
    case OP_EQ:
    case OP_NE:
    case OP_LT:
    case OP_LE:
    case OP_GT:
    case OP_GE:
      binary_tostring(self, string);
      return;
    case OP_BEZ:
    case OP_BNZ:
    case OP_BNE:
    case OP_JMP:
      branch_tostring(self, string);
      return;
    case OP_DEFUNC:
      defunc_tostring(self, string);
      return;
    case OP_PARA:
      para_tostring(self, string);
      return;
    case OP_RET:
      ret_tostring(self, string);
      return;
    case OP_NOP:
      utillib_string_append(string, "nop");
      return;
    case OP_READ:
      read_tostring(self, string);
      return;
    case OP_WRITE:
      write_tostring(self, string);
      return;
    case OP_STORE:
      store_tostring(self, string);
      return;
    case OP_PUSH:
      push_tostring(self, string);
      return;
    default:
      puts(cling_ast_opcode_kind_tostring(self->opcode));
      assert(false);

  }
}

void cling_ast_ir_print(struct utillib_vector const *instrs) {
  int i=0;
  struct cling_ast_ir const *ir;
  struct utillib_string buffer;
  utillib_string_init(&buffer);
  UTILLIB_VECTOR_FOREACH(ir, instrs) {
    cling_ast_ir_tostring(ir, &buffer);
    printf("%d: %s\n", i, utillib_string_c_str(&buffer));
    ++i;
    utillib_string_clear(&buffer);
  }
  utillib_string_destroy(&buffer);
}
