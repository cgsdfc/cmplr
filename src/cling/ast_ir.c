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
#include "ast_ir.h"
#include "misc.h"
#include "symbol_table.h"
#include "symbols.h"
#include <utillib/json_foreach.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
UTILLIB_ETAB_ELEM_INIT(OP_IDX, "index")
UTILLIB_ETAB_ELEM_INIT(OP_CAL, "call")
UTILLIB_ETAB_ELEM_INIT(OP_DIV, "/")
UTILLIB_ETAB_ELEM_INIT(OP_MUL, "*")
UTILLIB_ETAB_ELEM_INIT(OP_EQ, "==")
UTILLIB_ETAB_ELEM_INIT(OP_NE, "!=")
UTILLIB_ETAB_ELEM_INIT(OP_LT, "<")
UTILLIB_ETAB_ELEM_INIT(OP_LE, "<=")
UTILLIB_ETAB_ELEM_INIT(OP_GT, ">")
UTILLIB_ETAB_ELEM_INIT(OP_GE, ">=")
UTILLIB_ETAB_ELEM_INIT(OP_BEZ, "bez")
UTILLIB_ETAB_ELEM_INIT(OP_BNE, "bne")
UTILLIB_ETAB_ELEM_INIT(OP_JMP, "jmp")
UTILLIB_ETAB_ELEM_INIT(OP_STORE, "store")
UTILLIB_ETAB_ELEM_INIT(OP_LDIMM, "ldimm")
UTILLIB_ETAB_ELEM_INIT(OP_LDSTR, "ldstr")
UTILLIB_ETAB_ELEM_INIT(OP_WRINT, "write-int")
UTILLIB_ETAB_ELEM_INIT(OP_WRSTR, "write-str")
UTILLIB_ETAB_ELEM_INIT(OP_WRCHR, "write-chr")
UTILLIB_ETAB_ELEM_INIT(OP_READ, "read")
UTILLIB_ETAB_ELEM_INIT(OP_RDINT, "read-int")
UTILLIB_ETAB_ELEM_INIT(OP_RDCHR, "read-chr")
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

static const struct cling_ast_ir 
cling_ast_ir_nop={.opcode=OP_NOP};

static struct cling_ast_ir *emit_ir(int opcode) {
  struct cling_ast_ir *self = calloc(sizeof *self, 1);
  self->opcode = opcode;
  return self;
}

static void cling_ast_ir_destroy(struct cling_ast_ir *self) {
  if (self->opcode == OP_NOP)
    return;
  free(self);
}

static void ast_ir_print(struct cling_ast_ir const *self, FILE *file) {
  char const *opstr = cling_ast_opcode_kind_tostring(self->opcode);
  char const *size_name;

  switch (self->opcode) {
  case OP_NOP:
    fputs("nop", file);
    break;
  case OP_DEFCON:
    size_name=size_tostring(self->defcon.size);
    if (self->defcon.size == MIPS_WORD_SIZE) {
      fprintf(file,  "const %s %s = %d", self->defcon.name, size_name, self->defcon.value);
    } else {
      fprintf(file,  "const %s %s = '%c'", self->defcon.name, size_name, (char) self->defcon.value);
    }
    break;
  case OP_PARA:
    size_name=size_tostring(self->para.size);
    fprintf(file,  "para %s %s", size_name, self->para.name);
    break;
  case OP_DEFVAR:
    size_name=size_tostring(self->defvar.size);
    fprintf(file,  "var %s %s", size_name, self->defvar.name);
    break;
  case OP_DEFUNC:
    size_name=size_tostring(self->defunc.return_size);
    fprintf(file,  "%s %s()", size_name, self->defunc.name);
    break;
  case OP_DEFARR:
    size_name=size_tostring(self->defarr.base_size);
    fprintf(file,  "var %s %s[%lu]", size_name, self->defarr.name, self->defarr.extend);
    break;
  case OP_CAL:
    if (self->call.has_result)
      fprintf(file,  "t%d = call %s", self->call.result, self->call.name);
    else
      fprintf(file,  "call %s", self->call.name);
    break;
  case OP_IDX:
    fprintf(file, "t%d = t%d [ t%d ]", self->index.result, self->index.array_addr, self->index.index_result);
    break;
  case OP_ADD:
  case OP_SUB:
  case OP_DIV:
  case OP_MUL:
  case OP_EQ:
  case OP_NE:
  case OP_LT:
  case OP_LE:
  case OP_GT:
  case OP_GE:
    fprintf(file, "t%d = t%d %s t%d", 
        self->binop.result, self->binop.temp1, opstr, self->binop.temp2);
    break;
  case OP_BEZ:
    fprintf(file,  "bez t%d %d", self->bez.temp, self->bez.addr);
    break;
  case OP_BNE:
    fprintf(file,  "bne t%d t%d %d", self->bne.temp1, self->bne.temp2, self->bne.addr);
    break;
  case OP_JMP:
    fprintf(file,  "jmp %d", self->jmp.addr);
    break;
  case OP_RET:
    if (self->ret.has_result)
      fprintf(file,  "ret %d t%d", self->ret.result, self->ret.addr);
    else
      fprintf(file,  "ret %d", self->ret.addr);
    break;
  case OP_RDCHR:
  case OP_RDINT:
    fprintf(file,  "%s t%d", opstr, self->read.temp);
    break;
  case OP_WRINT:
  case OP_WRSTR:
  case OP_WRCHR:
    fprintf(file,  "%s t%d", opstr, self->write.temp);
    break;
  case OP_STORE:
    fprintf(file,  "store t%d t%d", self->store.addr, self->store.value);
    break;
  case OP_PUSH:
    size_name=size_tostring(self->push.size);
    fprintf(file,  "push %s t%d", size_name, self->push.temp);
    break;
  case OP_LDSTR:
    fprintf(file,  "ldstr t%d \"%s\"", self->ldstr.temp, self->ldstr.string);
    break;
  case OP_LDIMM:
    if (self->ldimm.size == MIPS_WORD_SIZE)
      fprintf(file,  "ldimm t%d %d", self->ldimm.temp, self->ldimm.value);
    else
      fprintf(file, "ldimm t%d '%c'", self->ldimm.temp, (char) self->ldimm.value);
    break;
  case OP_LOAD:
    fprintf(file,  "load t%d %s", self->load.temp, self->load.name);
    break;
  default:
    assert(false);
  }
}

static void ast_ir_vector_print(struct utillib_vector const *instrs, FILE *file) {
  int i = 0;
  struct cling_ast_ir const *ir;
  UTILLIB_VECTOR_FOREACH(ir, instrs) {
    fprintf(file, "%4d: ", i);
    ast_ir_print(ir, file);
    fputs("\n", file);
    ++i;
  }
}

/*
 * Polish-ir Emission.
 * Hack: we use a trick to tell lvalue from rvalue, which
 * affects the result of a load instruction given an iden.
 * That is, for lvalue iden, we load its address and for
 * rvalue iden, we load its content (the thing at that address).
 * The hack is: assume all the iden as rvalue; when a assignment
 * is seen, we twist the iden in the last instruction to lvalue.
 * The same applies to index operator.
 */

static void polish_ir_post_order(struct cling_polish_ir *self,
                                 struct utillib_json_value const *node) {
  assert(node);
  struct utillib_json_value *op, *lhs, *rhs;
  struct utillib_json_value const *arg;

  op = utillib_json_object_at(node, "op");
  if (!op) {
    /*
     * Trivial case.
     */
    utillib_vector_push_back(&self->stack, node);
    return;
  }
  lhs = utillib_json_object_at(node, "lhs");
  rhs = utillib_json_object_at(node, "rhs");
  if (op->as_size_t == SYM_RP) {
    /*
     * call_expr case
     * first arglist, then name.
     * We reverse the arglist here.
     * FIXME: We should not reverse it
     * in the first place at opg_parser?
     * but that will change ast_check tooo.
     */
    size_t size = utillib_json_array_size(rhs);
    for (int i = size - 1; i >= 0; --i) {
      arg = utillib_json_array_at(rhs, i);
      polish_ir_post_order(self, arg);
    }
    polish_ir_post_order(self, lhs);
  } else if (op->as_size_t == SYM_EQ || op->as_size_t == SYM_RK) {
    /*
     * Assignment is right associative.
     * We want the array iden to be loaded _after_ the index expr
     * is evaluated so that we can peek its type/size by looking
     * at the latest instruction.
     */
    polish_ir_post_order(self, rhs);
    polish_ir_post_order(self, lhs);
  } else {
    /*
     * Other left associative binary operators.
     */
    polish_ir_post_order(self, lhs);
    polish_ir_post_order(self, rhs);
  }
  utillib_vector_push_back(&self->stack, node);
}

static inline int make_temp(struct cling_ast_ir_global *self) {
  return self->temps++;
}

static struct utillib_json_value *
polish_ir_make_temp(struct cling_polish_ir *self) {
  int temp = make_temp(self->global);
  return utillib_json_int_create(&temp);
}

static void polish_ir_maybe_release_temp(struct utillib_json_value *val) {
  if (val->kind == UT_JSON_INT)
    utillib_json_value_destroy(val);
}

/*
 * Hack: steal the elemsz from the precedent load of array_addr.
 * And hack the precedent load to be lvalue.
 */
static void polish_ir_emit_index(struct cling_polish_ir *self,
                                 struct utillib_vector *instrs) {
  struct utillib_json_value *result, *array_temp, *index_temp;
  struct cling_ast_ir *ir;
  int base_size;

  /*
   * Hack: the array iden must be a lvalue.
   */
  ir = utillib_vector_back(instrs);
  assert(ir->opcode == OP_LOAD);
  ir->load.is_rvalue=false;
  base_size=ir->load.size;

  /*
   * Get 2 operands.
   */
  array_temp = utillib_vector_back(&self->opstack);
  utillib_vector_pop_back(&self->opstack);
  index_temp = utillib_vector_back(&self->opstack);
  utillib_vector_pop_back(&self->opstack);

  result = polish_ir_make_temp(self);
  ir=emit_ir(OP_IDX);
  ir->index.result=result->as_int;
  ir->index.array_addr=array_temp->as_int;
  ir->index.index_result=index_temp->as_int;
  ir->index.base_size=base_size;
  ir->index.is_rvalue=true;
  utillib_vector_push_back(instrs,ir);
  /*
   * The result represents content or address depending on is_rvalue.
   */
  utillib_vector_push_back(&self->opstack, result);
  polish_ir_maybe_release_temp(array_temp);
  polish_ir_maybe_release_temp(index_temp);
}

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
static void polish_ir_emit_call(struct cling_polish_ir *self,
                                struct utillib_vector *instrs) {
  struct utillib_json_value *callee, *arg;
  struct utillib_json_value *value, *temp;
  struct cling_ast_ir *ir;

  /*
   * Fetch all the field about function.
   */
  struct cling_symbol_entry const *entry;

  callee = utillib_vector_back(&self->opstack);
  value = utillib_json_object_at(callee, "value");
  entry = cling_symbol_table_find(
      self->global->symbol_table, value->as_ptr, CL_GLOBAL);
  assert(entry && entry->kind == CL_FUNC);
  utillib_vector_pop_back(&self->opstack);
  /*
   * Only pop argc elements.
   */
  for (int i = 0; i < entry->function.argc; ++i) {
    arg = utillib_vector_back(&self->opstack);
    utillib_vector_pop_back(&self->opstack);
    ir=emit_ir(OP_PUSH);
    ir->push.size=cling_type_to_size(entry->function.argv_types[i]);
    ir->push.temp=arg->as_int;
    utillib_vector_push_back(instrs, ir);
    polish_ir_maybe_release_temp(arg);
  }
  ir=emit_ir(OP_CAL);
  ir->call.name=value->as_ptr;
  if (entry->function.return_type != CL_VOID) {
    temp = polish_ir_make_temp(self);
    ir->call.has_result=true;
    ir->call.result=temp->as_int;
    utillib_vector_push_back(&self->opstack, temp);
  } else {
    ir->call.has_result=false;
  }
  utillib_vector_push_back(instrs, ir);
}

static void polish_ir_emit_binary(struct cling_polish_ir *self, size_t op,
                                  struct utillib_vector *instrs) {
  struct utillib_json_value *lhs, *rhs, *temp;
  register struct cling_ast_ir *ir;

  rhs = utillib_vector_back(&self->opstack);
  utillib_vector_pop_back(&self->opstack);
  lhs = utillib_vector_back(&self->opstack);
  utillib_vector_pop_back(&self->opstack);
  temp = polish_ir_make_temp(self);
  ir=emit_ir(symbol_to_ast_opcode(op));
  ir->binop.result=temp->as_int;
  ir->binop.temp1=lhs->as_int;
  ir->binop.temp2=rhs->as_int;
  utillib_vector_push_back(instrs, ir);
  utillib_vector_push_back(&self->opstack, temp);
  polish_ir_maybe_release_temp(lhs);
  polish_ir_maybe_release_temp(rhs);
}

/*
 * Hack: Modify the precedent instr to be of lvalue and
 * steal size from it.
 */
static void polish_ir_emit_assign(struct cling_polish_ir *self,
                                  struct utillib_vector *instrs) {

  struct utillib_json_value * assignee, * assigner;
  struct cling_ast_ir *ir;
  int size;

  assignee = utillib_vector_back(&self->opstack);
  utillib_vector_pop_back(&self->opstack);
  assigner = utillib_vector_back(&self->opstack);
  utillib_vector_pop_back(&self->opstack);

  /*
   * Perform the Hack.
   * load/index becomes lvalue.
   */
  ir = utillib_vector_back(instrs);
  switch (ir->opcode) {
  case OP_IDX:
    assert(ir->index.is_rvalue);
    ir->index.is_rvalue=false;
    size=ir->index.base_size;
    break;
  case OP_LOAD:
    assert(ir->load.is_rvalue);
    ir->load.is_rvalue=false;
    size=ir->load.size;
    break;
  default:
    /*
     * We must have something to assign and the polish_ir_post_order
     * ensures the loading of assignee is right in front of us.
     */
    assert(false);
  }
  ir=emit_ir(OP_STORE);
  ir->store.addr=assignee->as_int;
  ir->store.value=assigner->as_int;
  ir->store.size=size;
  utillib_vector_push_back(instrs, ir);
  /*
   * According to the grammar, assign_expr
   * has no value so no need allocate temp.
   */
  polish_ir_maybe_release_temp(assignee);
  polish_ir_maybe_release_temp(assigner);
}

/*
 * Assume every iden is rvalue and fix up
 * later in `polish_ir_emit_assign'.
 */
static void polish_ir_emit_load(struct cling_polish_ir *self,
                                struct utillib_json_value const *object,
                                struct utillib_vector *instrs) {
  struct utillib_json_value const *type, *temp, *json_value;
  struct cling_symbol_entry const *entry;
  struct cling_ast_ir *ir;
  bool is_global;
  char const *name;
  int size;
  int value;

  json_value = utillib_json_object_at(object, "value");
  type = utillib_json_object_at(object, "type");
  switch (type->as_size_t) {
  case SYM_IDEN:
    entry = cling_symbol_table_find(
        self->global->symbol_table, json_value->as_ptr, CL_LEXICAL);
    if (entry->kind == CL_FUNC) {
      utillib_vector_push_back(&self->opstack, object);
      return;
    }
    switch (entry->kind) {
    case CL_CONST:
      size=cling_type_to_size(entry->constant.type);
      value=entry->constant.value;
      goto make_ldimm;
    case CL_ARRAY:
      name=json_value->as_ptr;
      size=cling_type_to_size(entry->array.base_type);
      is_global=entry->scope == 0;
      goto make_load;
    case CL_INT:
    case CL_CHAR:
      name=json_value->as_ptr;
      size=cling_type_to_size(entry->kind);
      is_global=entry->scope == 0;
      goto make_load;
    default:
      assert(false);
    }
  case SYM_STRING:
    goto make_ldstr;
  case SYM_CHAR:
  case SYM_INTEGER:
  case SYM_UINT:
    size=cling_symbol_to_size(type->as_size_t);
    value=cling_symbol_to_immediate(type->as_size_t, json_value->as_ptr);
    goto make_ldimm;
  default:
    assert(false);
  }
make_ldimm:
  temp = polish_ir_make_temp(self);
  ir=emit_ir(OP_LDIMM);
  ir->ldimm.size=size;
  ir->ldimm.value=value;
  ir->ldimm.temp=temp->as_int;
  goto done;

make_load:
  temp = polish_ir_make_temp(self);
  ir=emit_ir(OP_LOAD);
  ir->load.is_rvalue=true;
  ir->load.name=name;
  ir->load.is_global=is_global;
  ir->load.size=size;
  ir->load.temp=temp->as_int;
  goto done;

make_ldstr:
  temp=polish_ir_make_temp(self);
  ir=emit_ir(OP_LDSTR);
  ir->ldstr.temp=temp->as_int;
  ir->ldstr.string=json_value->as_ptr;
  /*
   * You never know when to add a label here.
   */
  goto done;

done:
  utillib_vector_push_back(instrs, ir);
  utillib_vector_push_back(&self->opstack, temp);
}

static void cling_polish_ir_emit(struct cling_polish_ir *self,
                                 struct utillib_vector *instrs) {

  struct utillib_json_value const *object, *op;
  UTILLIB_VECTOR_FOREACH(object, &self->stack) {
    op = utillib_json_object_at(object, "op");
    if (!op) {
      /*
       * iden, char, int, uint should be loaded.
       */
      polish_ir_emit_load(self, object, instrs);
      continue;
    }
    switch (op->as_size_t) {
    case SYM_RK:
      polish_ir_emit_index(self, instrs);
      break;
    case SYM_RP:
      polish_ir_emit_call(self, instrs);
      break;
    case SYM_EQ:
      polish_ir_emit_assign(self, instrs);
      break;
    default:
      /*
       * polish_ir_emit_binary handles all the
       * rest of cases.
       */
      polish_ir_emit_binary(self, op->as_size_t, instrs);
      break;
    }
  }
}

static int polish_ir_result(struct cling_polish_ir const *self) {
  struct utillib_json_value *result = utillib_vector_back(&self->opstack);
  assert(result->kind == UT_JSON_INT);
  return result->as_int;
}

static void cling_polish_ir_init(struct cling_polish_ir *self,
                                 struct utillib_json_value const *root,
                                 struct cling_ast_ir_global *global) {
  self->global = global;
  utillib_vector_init(&self->stack);
  utillib_vector_init(&self->opstack);
  polish_ir_post_order(self, root);
}

static void cling_polish_ir_destroy(struct cling_polish_ir *self) {
  utillib_vector_destroy(&self->stack);
  utillib_vector_destroy_owning(&self->opstack, polish_ir_maybe_release_temp);
}

/*
 * AST visitor functions.
 */

static void emit_statement(struct utillib_json_value const *self,
                           struct cling_ast_ir_global *global,
                           struct utillib_vector *instrs);

static void emit_composite(struct utillib_json_value const *self,
                           struct cling_ast_ir_global *global,
                           struct utillib_vector *instrs);

static void emit_scanf_stmt(struct utillib_json_value const *self,
                            struct cling_ast_ir_global *global,
                            struct utillib_vector *instrs) {
  struct utillib_json_value *value, *arglist;
  struct utillib_json_value const *object;
  struct cling_symbol_entry const *entry;
  struct cling_ast_ir *ir;
  int temp;
  int size;

  arglist = utillib_json_object_at(self, "arglist");
  UTILLIB_JSON_ARRAY_FOREACH(object, arglist) {
    value = utillib_json_object_at(object, "value");
    entry = cling_symbol_table_find(global->symbol_table, value->as_ptr,
                                    CL_LEXICAL);
    temp = make_temp(global);
    size=cling_type_to_size(entry->kind);
    ir=emit_ir(OP_LOAD);
    ir->load.name=value->as_ptr;
    ir->load.size=size;
    ir->load.is_rvalue=false;
    ir->load.is_global=entry->scope == 0;
    ir->load.temp=temp;
    utillib_vector_push_back(instrs, ir);
    ir=emit_ir(cling_type_to_read(entry->kind));
    ir->read.temp=temp;
    utillib_vector_push_back(instrs, ir);
  }
}

static void emit_printf_stmt(struct utillib_json_value const *self,
                             struct cling_ast_ir_global *global,
                             struct utillib_vector *instrs) {
  struct utillib_json_value const  *arglist;
  struct utillib_json_value const  *object;
  int opcode;
  struct cling_ast_ir *ir;
  struct cling_polish_ir polish_ir;
  arglist = utillib_json_object_at(self, "arglist");

  UTILLIB_JSON_ARRAY_FOREACH(object, arglist) {
    cling_polish_ir_init(&polish_ir, object, global);
    cling_polish_ir_emit(&polish_ir, instrs);
      /*
       * Hack: look at the precedent instruction to 
       * steal possible type so that we can have
       * const char a='a'; printf(a);
       * int b; b=1; printf(b);
       * char c[10]; c[0]='a'; printf(c[0]);
       * shows, 'a' 1 'a'.
       */
      ir=utillib_vector_back(instrs);
      switch(ir->opcode) {
        case OP_LDSTR:
          opcode=OP_WRSTR;
          break;
        case OP_LDIMM:
          opcode=cling_size_to_write(ir->ldimm.size);
          break;
        case OP_LOAD:
          opcode=cling_size_to_write(ir->load.size);
          break;
        case OP_IDX:
          opcode=cling_size_to_write(ir->index.base_size);
          break;
        default:
          /*
           * Result of expression is int.
           */
          opcode=OP_WRINT;
      }
      ir=emit_ir(opcode);
      ir->write.temp=polish_ir_result(&polish_ir);
      utillib_vector_push_back(instrs, ir);
      cling_polish_ir_destroy(&polish_ir);
    }
}

/*
 * An ordinary for-stmt is:
 * <init>
 * tricky_jump <body>
 * <cond>
 * cond_test: bez cond_expr <endfor>
 * <body>
 * <step>
 * loop_jump <cond>
 * <endfor>
 *
 * Notes, tricky_jump is inserted
 * for special purpose, see below.
 */
static void emit_for_stmt(struct utillib_json_value const *self,
                          struct cling_ast_ir_global *global,
                          struct utillib_vector *instrs) {
  struct utillib_json_value const *init, *cond, *step, *stmt;
  /*
   * Since the grammar asks for an abnormal interpretation
   * of the ordinary for-statement, we add one `tricky_jmp'
   * to fulfill its odd need.
   * Essentially it asks us to execute the for-body once before
   * the for-cond is ever considered, which is achieved by inserting
   * an unconditional jump between for-init and for-cond jumping
   * directly to for-body.
   * The JTA is refilled when the cond was emitted, so we keep a ref
   * to tricky_jmp.
   */
  struct cling_ast_ir *tricky_jump, *cond_test, *loop_jump;
  struct cling_polish_ir polish_ir;
  int loop_jump_jta;

  init = utillib_json_object_at(self, "init");
  cond = utillib_json_object_at(self, "cond");
  step = utillib_json_object_at(self, "step");
  stmt = utillib_json_object_at(self, "stmt");

  cling_polish_ir_init(&polish_ir, init, global);
  cling_polish_ir_emit(&polish_ir, instrs);
  tricky_jump = emit_ir(OP_JMP);
  utillib_vector_push_back(instrs, tricky_jump);
  /*
   * The JTA of loop_jump
   * is the next instr of the tricky_jump,
   * which is also the beginning of cond_test.
   */
  loop_jump_jta = utillib_vector_size(instrs);
  cling_polish_ir_destroy(&polish_ir);

  /*
   * cond.
   */
  cling_polish_ir_init(&polish_ir, cond, global);
  cling_polish_ir_emit(&polish_ir, instrs);
  cond_test = emit_ir(OP_BEZ);
  /*
   * The result of cond is the judgement of cond_test.
   */
  cond_test->bez.temp=polish_ir_result(&polish_ir);
  utillib_vector_push_back(instrs, cond_test);
  /*
   * The JTA of tricky_jump is the next instr of
   * cond_test, which is also the beginning of
   * body.
   */
  tricky_jump->jmp.addr=utillib_vector_size(instrs);
  cling_polish_ir_destroy(&polish_ir);

  /*
   * body
   */
  emit_statement(stmt, global, instrs);

  /*
   * step
   */
  cling_polish_ir_init(&polish_ir, step, global);
  cling_polish_ir_emit(&polish_ir, instrs);
  /*
   * The JTA of cond_test is the next instr
   * of loop_jump.
   */
  loop_jump = emit_ir(OP_JMP);
  loop_jump->jmp.addr=loop_jump_jta;
  utillib_vector_push_back(instrs, loop_jump);
  cond_test->bez.addr= utillib_vector_size(instrs);

  /*
   * Clean up
   */
  cling_polish_ir_destroy(&polish_ir);
}

/*
 * A case_gaurd is the first instr of a
 * case clause.
 * It is a OP_BNE taking the switch-expr and
 * case_label as operands to compare and its
 * BTA is the next case_gaurd.
 * The function creates a case_gaurd without
 * BTA which should be filled in later.
 */
/*
 * Ordinary switch-case statement
 * can be interpreted as:
 * <eval-switch-expr>
 * bne <case-label-1> <switch-expr> <next-bne>
 * <case-1-stmt>
 * break_jump <endswitch>
 * bne <case-label-2> <switch-expr> <default>
 * <case-2-stmt>
 * break_jump <endswitch>
 * <default>:
 * <default-stmt>
 * <endswitch>
 */

static void emit_switch_stmt(struct utillib_json_value const *self,
                             struct cling_ast_ir_global *global,
                             struct utillib_vector *instrs) {
  struct cling_polish_ir polish_ir;
  struct utillib_json_value const *object, *case_clause, *expr;
  struct utillib_json_value const *value, *case_, *stmt, *type;
  struct cling_ast_ir *case_gaurd, *break_jump, *load_label;
  /*
   * break_jumps keeps all the break_jump of
   * the case_clauses to fill their JTA to
   * the end of switch_stmt later.
   */
  struct utillib_vector break_jumps;
  int break_jump_jta;
  int loaded_const;

  utillib_vector_init(&break_jumps);
  expr = utillib_json_object_at(self, "expr");
  case_clause = utillib_json_object_at(self, "cases");
  cling_polish_ir_init(&polish_ir, expr, global);
  cling_polish_ir_emit(&polish_ir, instrs);

  UTILLIB_JSON_ARRAY_FOREACH(object, case_clause) {
    case_ = utillib_json_object_at(object, "case");
    stmt = utillib_json_object_at(object, "stmt");
    if (case_) {
      value = utillib_json_object_at(case_, "value");
      type = utillib_json_object_at(case_, "type");

      /*
       * Load label constant.
       */
      load_label = emit_ir(OP_LDIMM);
      loaded_const=make_temp(global);
      load_label->ldimm.temp=loaded_const;
      load_label->ldimm.value=cling_symbol_to_immediate(type->as_size_t, value->as_ptr);
      load_label->ldimm.size=cling_symbol_to_size(type->as_size_t);
      utillib_vector_push_back(instrs, load_label);

      /*
       * Case gaurd.
       */
      case_gaurd = emit_ir(OP_BNE);
      case_gaurd->bne.temp1=loaded_const;
      case_gaurd->bne.temp2=polish_ir_result(&polish_ir);
      utillib_vector_push_back(instrs, case_gaurd);

      /*
       * Statement.
       */
      emit_statement(stmt, global, instrs);
      break_jump = emit_ir(OP_JMP);
      utillib_vector_push_back(instrs, break_jump);
      utillib_vector_push_back(&break_jumps, break_jump);
      /*
       * Fills in the BTA of case_gaurd, which
       * is the next instr of break_jump.
       */
      case_gaurd->bne.addr = utillib_vector_size(instrs);
    } else {
      /*
       * default clause does not need a break_jump.
       */
      emit_statement(stmt, global, instrs);
      break_jump_jta = utillib_vector_size(instrs);
    }
  }

  UTILLIB_VECTOR_FOREACH(break_jump, &break_jumps) {
    break_jump->jmp.addr = break_jump_jta;
  }

  utillib_vector_destroy(&break_jumps);
  cling_polish_ir_destroy(&polish_ir);
}

static void emit_expr_stmt(struct utillib_json_value const *self,
                           struct cling_ast_ir_global *global,
                           struct utillib_vector *instrs) {
  struct utillib_json_value const *expr;
  struct cling_polish_ir polish_ir;

  expr = utillib_json_object_at(self, "expr");
  cling_polish_ir_init(&polish_ir, expr, global);
  cling_polish_ir_emit(&polish_ir, instrs);
  cling_polish_ir_destroy(&polish_ir);
}

/*
 * ret just transfter control flow to the clean up
 * code of the current function.
 */
static void emit_return_stmt(struct utillib_json_value const *self,
                             struct cling_ast_ir_global *global,
                             struct utillib_vector *instrs) {
  struct utillib_json_value const *expr;
  struct cling_polish_ir polish_ir;
  struct cling_ast_ir *ir;

  expr = utillib_json_object_at(self, "expr");
  ir = emit_ir(OP_RET);
  if (expr) {
    cling_polish_ir_init(&polish_ir, expr, global);
    cling_polish_ir_emit(&polish_ir, instrs);
    ir->ret.has_result=true;
    ir->ret.result=polish_ir_result(&polish_ir);
    cling_polish_ir_destroy(&polish_ir);
  } else {
    ir->ret.has_result=false;
  }
  utillib_vector_push_back(instrs, ir);
}

static void emit_if_stmt(struct utillib_json_value const *self,
                         struct cling_ast_ir_global *global,
                         struct utillib_vector *instrs) {
  struct utillib_json_value const *expr, *then_clause, *else_clause;
  struct cling_polish_ir polish_ir;
  struct cling_ast_ir *skip_branch, *jump;

  expr = utillib_json_object_at(self, "expr");
  cling_polish_ir_init(&polish_ir, expr, global);
  cling_polish_ir_emit(&polish_ir, instrs);

  then_clause = utillib_json_object_at(self, "then");
  skip_branch = emit_ir(OP_BEZ);
  skip_branch->bez.temp=polish_ir_result(&polish_ir);
  utillib_vector_push_back(instrs, skip_branch);

  emit_statement(then_clause, global, instrs);
  else_clause = utillib_json_object_at(self, "else");
  if (else_clause) {
    jump = emit_ir(OP_JMP);
    utillib_vector_push_back(instrs, jump);
    skip_branch->bez.addr = utillib_vector_size(instrs);
    emit_statement(else_clause, global, instrs);
    jump->jmp.addr = utillib_vector_size(instrs);
  } else {
    skip_branch->bez.addr = utillib_vector_size(instrs);
  }
  cling_polish_ir_destroy(&polish_ir);
}

static void emit_statement(struct utillib_json_value const *self,
                           struct cling_ast_ir_global *global,
                           struct utillib_vector *instrs) {
  if (self == &utillib_json_null)
    return;
  struct utillib_json_value const *type = utillib_json_object_at(self, "type");
  switch (type->as_size_t) {
  case SYM_PRINTF_STMT:
    emit_printf_stmt(self, global, instrs);
    return;
  case SYM_SCANF_STMT:
    emit_scanf_stmt(self, global, instrs);
    return;
  case SYM_FOR_STMT:
    emit_for_stmt(self, global, instrs);
    return;
  case SYM_IF_STMT:
    emit_if_stmt(self, global, instrs);
    return;
  case SYM_SWITCH_STMT:
    emit_switch_stmt(self, global, instrs);
    return;
  case SYM_RETURN_STMT:
    emit_return_stmt(self, global, instrs);
    return;
  case SYM_EXPR_STMT:
    emit_expr_stmt(self, global, instrs);
    return;
  case SYM_COMP_STMT:
    emit_composite(self, global, instrs);
    return;
  default:
    assert(false);
  }
}

/*
 * Emits defvar ir.
 * defvar name(size|scope) extend(is_array)
 */
static void emit_var_defs(struct utillib_json_value const *self,
                          struct cling_ast_ir_global *global,
                          struct utillib_vector *instrs) {
  struct utillib_json_value const *defs, *decl;
  struct utillib_json_value *name;
  struct cling_ast_ir *ir;
  struct cling_symbol_entry const *entry;

  defs = utillib_json_object_at(self, "var_defs");
  UTILLIB_JSON_ARRAY_FOREACH(decl, defs) {
    name = utillib_json_object_at(decl, "name");
    entry =
        cling_symbol_table_find(global->symbol_table, name->as_ptr, CL_LEXICAL);
    if (entry->kind == CL_ARRAY) {
      ir = emit_ir(OP_DEFARR);
      ir->defarr.name=name->as_ptr;
      ir->defarr.extend=entry->array.extend;
      ir->defarr.base_size=cling_type_to_size(entry->array.base_type);
    } else {
      ir = emit_ir(OP_DEFVAR);
      ir->defvar.name=name->as_ptr;
      ir->defvar.size=cling_type_to_size(entry->kind);
    }
    utillib_vector_push_back(instrs, ir);
  }
}

/*
 * Emits defcon ir.
 * defcon name(size|scope) value
 */
static void emit_const_defs(struct utillib_json_value const *self,
                            struct cling_ast_ir_global *global,
                            struct utillib_vector *instrs) {
  struct utillib_json_value const *defs, *decl;
  struct utillib_json_value const *name;
  struct cling_symbol_entry const *entry;
  struct cling_ast_ir *ir;

  defs = utillib_json_object_at(self, "const_defs");

  UTILLIB_JSON_ARRAY_FOREACH(decl, defs) {
    name = utillib_json_object_at(decl, "name");
    entry =
        cling_symbol_table_find(global->symbol_table, name->as_ptr, CL_LEXICAL);
    ir=emit_ir(OP_DEFCON);
    ir->defcon.name=name->as_ptr;
    ir->defcon.size=cling_type_to_size(entry->constant.type);
    ir->defcon.value=entry->constant.value;
    utillib_vector_push_back(instrs,ir);
  }
}

/*
 * Wrapper around emit_const_defs
 * and emit_var_defs.
 * Notes it will insert const_decls and
 * var_decls into symbol_table.
 */
static void maybe_emit_decls(struct utillib_json_value const *self,
                             struct cling_ast_ir_global *global,
                             struct utillib_vector *instrs) {
  struct utillib_json_value const *object, *const_decls, *var_decls;

  const_decls = utillib_json_object_at(self, "const_decls");
  var_decls = utillib_json_object_at(self, "var_decls");
  if (const_decls) {
    UTILLIB_JSON_ARRAY_FOREACH(object, const_decls) {
      emit_const_defs(object, global, instrs);
    }
  }

  if (var_decls) {
    UTILLIB_JSON_ARRAY_FOREACH(object, var_decls) {
      emit_var_defs(object, global, instrs);
    }
  }
}

static void maybe_insert_decls(struct utillib_json_value const *self,
                               struct cling_symbol_table *symbol_table) {
  struct utillib_json_value const *object, *const_decls, *var_decls;

  const_decls = utillib_json_object_at(self, "const_decls");
  var_decls = utillib_json_object_at(self, "var_decls");
  if (const_decls) {
    UTILLIB_JSON_ARRAY_FOREACH(object, const_decls) {
      cling_symbol_table_insert_const(symbol_table, object);
    }
  }

  if (var_decls) {
    UTILLIB_JSON_ARRAY_FOREACH(object, var_decls) {
      cling_symbol_table_insert_variable(symbol_table, object);
    }
  }
}

static void emit_composite(struct utillib_json_value const *self,
                           struct cling_ast_ir_global *global,
                           struct utillib_vector *instrs) {
  struct utillib_json_value const *object, *stmts;
  stmts = utillib_json_object_at(self, "stmts");
  if (stmts)
    /*
     * Watch out the empty case!
     */
    UTILLIB_JSON_ARRAY_FOREACH(object, stmts) {
      emit_statement(object, global, instrs);
    }
}

static struct cling_ast_function *cling_ast_function_create(char const *name) {
  struct cling_ast_function *self = malloc(sizeof *self);
  self->name = name;
  self->temps = 0;
  utillib_vector_init(&self->instrs);
  utillib_vector_init(&self->init_code);
  return self;
}

static void cling_ast_function_destroy(struct cling_ast_function *self) {
  utillib_vector_destroy_owning(&self->instrs, cling_ast_ir_destroy);
  utillib_vector_destroy_owning(&self->init_code, cling_ast_ir_destroy);
  free(self);
}

void cling_ast_program_init(struct cling_ast_program *self) {
  utillib_vector_init(&self->init_code);
  utillib_vector_init(&self->funcs);
}

void cling_ast_program_destroy(struct cling_ast_program *self) {
  utillib_vector_destroy_owning(&self->funcs, cling_ast_function_destroy);
  utillib_vector_destroy_owning(&self->init_code, cling_ast_ir_destroy);
}

static void emit_return_address(struct utillib_vector *instrs) {
  int addr;
  struct cling_ast_ir *ir;

  addr=utillib_vector_size(instrs);
  UTILLIB_VECTOR_FOREACH(ir, instrs)
    if (ir->opcode == OP_RET)
      ir->ret.addr=addr;
  utillib_vector_push_back(instrs, &cling_ast_ir_nop);
}

/*
 * Create an cling_ast_function from the func_node.
 */
static struct cling_ast_function *
cling_ast_ir_emit_function(struct utillib_json_value const *func_node,
                           struct cling_ast_ir_global *global) {
  struct cling_ast_function *self;
  struct utillib_json_value const *type, *name, *arglist, *arg, *comp;
  struct cling_symbol_entry const *entry;
  struct cling_ast_ir *ir;

  /*
   * AST Retrieve
   */
  name = utillib_json_object_at(func_node, "name");
  entry =
      cling_symbol_table_find(global->symbol_table, name->as_ptr, CL_GLOBAL);
  arglist = utillib_json_object_at(func_node, "arglist");
  comp = utillib_json_object_at(func_node, "comp");

  /*
   * symbol_table insersion.
   * Attention! maybe_insert_decls must come
   * before maybe_emit_decls for all those symbols
   * to be reachable!
   */
  maybe_insert_decls(comp, global->symbol_table);
  cling_symbol_table_insert_arglist(global->symbol_table, arglist);

  /*
   * init_code emision
   * defunc ...
   * para ...
   * const
   * var...
   */
  self = cling_ast_function_create(name->as_ptr);
  ir=emit_ir(OP_DEFUNC);
  ir->defunc.name=name->as_ptr;
  ir->defunc.return_size=cling_type_to_size( entry->function.return_type);
  utillib_vector_push_back(&self->init_code, ir);
  UTILLIB_JSON_ARRAY_FOREACH(arg, arglist) {
    name = utillib_json_object_at(arg, "name");
    entry =
        cling_symbol_table_find(global->symbol_table, name->as_ptr, CL_LOCAL);
    ir=emit_ir(OP_PARA);
    ir->para.name=name->as_ptr;
    ir->para.size=cling_type_to_size(entry->kind);
    utillib_vector_push_back(&self->init_code, ir);
  }
  maybe_emit_decls(comp, global, &self->init_code);

  /*
   * instrs emision
   */
  emit_composite(comp, global, &self->instrs);
  emit_return_address(&self->instrs);
  return self;
}

static inline void ast_ir_global_init(struct cling_ast_ir_global *self,
                                      struct cling_symbol_table *symbol_table) {
  self->temps = 0;
  self->symbol_table = symbol_table;
}

void cling_ast_ir_emit_program(struct utillib_json_value const *self,
                               struct cling_symbol_table *symbol_table,
                               struct cling_ast_program *program) {
  struct utillib_json_value const *func_decls, *object;
  struct cling_ast_function *func;
  struct cling_ast_ir_global global;
  int temps;

  ast_ir_global_init(&global, symbol_table);
  /*
   * Enters these names to global-scope.
   */
  maybe_emit_decls(self, &global, &program->init_code);
  func_decls = utillib_json_object_at(self, "func_decls");

  /*
   * All functions.
   */
  UTILLIB_JSON_ARRAY_FOREACH(object, func_decls) {
    /*
     * Clear the temps counter.
     */
    global.temps = 0;
    cling_symbol_table_enter_scope(symbol_table);
    func = cling_ast_ir_emit_function(object, &global);
    cling_symbol_table_leave_scope(symbol_table);
    func->temps = global.temps;
    utillib_vector_push_back(&program->funcs, func);
  }
}

void cling_ast_program_print(struct cling_ast_program const *self, FILE *file) {
  struct cling_ast_function const *func;
  ast_ir_vector_print(&self->init_code, file);
  fputs("\n", file);
  UTILLIB_VECTOR_FOREACH(func, &self->funcs) {
    ast_ir_vector_print(&func->init_code, file);
    ast_ir_vector_print(&func->instrs, file);
    fputs("\n", file);
  }
}
