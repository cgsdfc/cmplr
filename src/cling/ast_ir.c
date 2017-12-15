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
 * AST-ir creators.
 */

/*
 *  0 stands for global,
 *  1 stands for local.
 */
static inline int emit_scope(int scope_kind) {
  return scope_kind ? CL_LOCL : CL_GLBL;
}

static inline void init_null(struct cling_ast_operand *self) {
  self->info = CL_NULL;
}

static void init_temp(struct cling_ast_operand *self, int temp, int type) {
  self->info = CL_TEMP | cling_type_to_wide(type);
  self->scalar = temp;
}

static void init_name(struct cling_ast_operand *self, int scope, int type,
                      char const *name) {
  self->info = CL_NAME | cling_type_to_wide(type) | emit_scope(scope);
  self->text = strdup(name);
}

static void init_imme(struct cling_ast_operand *self, int type,
                      char const *value) {
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

static void init_string(struct cling_ast_operand *self, char const *string) {
  self->text = strdup(string);
  self->info = CL_STRG;
}

/*
 * End of initializing different fields.
 */

static struct cling_ast_ir *emit_ir(int opcode) {
  struct cling_ast_ir *self = calloc(sizeof *self, 1);
  self->opcode = opcode;
  return self;
}

/*
 * ldstr temp string
 * load the address of `string' into `temp'
 */
static struct cling_ast_ir *emit_ldstr(int temp, char const *string) {
  struct cling_ast_ir *self = emit_ir(OP_LDSTR);
  init_temp(&self->operands[0], temp, CL_INT);
  init_string(&self->operands[1], string);
  return self;
}

/*
 * wrstr temp
 * write the string whose address is in `temp'
 */
static struct cling_ast_ir *emit_wrstr(int temp) {
  struct cling_ast_ir *self = emit_ir(OP_WRSTR);
  init_temp(&self->operands[0], temp, CL_INT);
  return self;
}

/*
 * wrint temp
 * write the integer in `temp'
 */
static struct cling_ast_ir *emit_wrint(int temp) {
  struct cling_ast_ir *self = emit_ir(OP_WRINT);
  init_temp(&self->operands[0], temp, CL_INT);
  return self;
}

/*
 * push temp(wide)
 * push the value held in `temp' into Argument Region.
 */
static struct cling_ast_ir *emit_push(int temp, int type) {
  struct cling_ast_ir *self = emit_ir(OP_PUSH);
  init_temp(&self->operands[0], temp, type);
  return self;
}

/*
 * load temp(is_rvalue) name(scope|wide)
 * If is_rvalue, load the value of `name' into `temp'
 * else load the address of name into temp.
 */
static struct cling_ast_ir *emit_load(char const *name, int scope, int type,
                                      int temp, bool is_rvalue) {
  struct cling_ast_ir *self = emit_ir(OP_LOAD);
  init_temp(&self->operands[0], temp, CL_INT);
  init_name(&self->operands[1], scope, type, name);
  self->operands[2].info = is_rvalue;
  return self;
}

static inline struct cling_ast_ir *ast_ir_emit_load(char const *name, struct cling_symbol_entry const *entry, int temp) {
  return emit_load(name, entry->scope, entry->kind, temp, true);
}

/*
 * defarr name(scope|base_wide) extend
 * define an array at scope with element of wide `base_wide'
 * and extend.
 */
static struct cling_ast_ir *emit_defarr(char const *name, int scope, int type,
                                        char const *extend) {
  struct cling_ast_ir *self = emit_ir(OP_DEFARR);
  init_name(&self->operands[0], scope, type, name);
  init_imme(&self->operands[1], CL_INT, extend);
  return self;
}

/*
 * call name [temp]
 * call function `name' and if it returns a value,
 * store it at `temp'. if maybe_temp is -1, no value is returned.
 */
static struct cling_ast_ir *emit_call(char const *name, int maybe_temp) {
  struct cling_ast_ir *self = emit_ir(OP_CAL);
  init_name(&self->operands[0], CL_GLBL, CL_FUNC, name);
  if (maybe_temp == -1) {
    init_null(&self->operands[1]);
    return self;
  }
  init_temp(&self->operands[1], maybe_temp, CL_INT);
  return self;
}

/*
 * read temp(wide)
 * Read an integer and store it in the address held
 * by temp.
 */
static struct cling_ast_ir *emit_read(int temp, int type) {
  struct cling_ast_ir *self = emit_ir(OP_READ);
  init_temp(&self->operands[0], temp, type);
  return self;
}

/*
 * store temp1(wide) temp2
 * store the value in temp2 in the address held by temp1.
 */
static struct cling_ast_ir *emit_store(int temp1, int temp2, int type) {
  struct cling_ast_ir *self = emit_ir(OP_STORE);
  init_temp(&self->operands[0], temp1, type);
  init_temp(&self->operands[1], temp2, CL_INT);
  return self;
}

/*
 * defcon name(scope|wide) value
 * define a constant of wide at scope with value.
 */
static struct cling_ast_ir *emit_defcon(char const *name, int scope, int type,
                                        char const *value) {
  struct cling_ast_ir *self = emit_ir(OP_DEFCON);
  init_name(&self->operands[0], scope, type, name);
  init_imme(&self->operands[1], type, value);
  return self;
}

/*
 * binop temp0 temp1 temp2.
 * do a binary operation and store result into temp0.
 */
static struct cling_ast_ir *emit_binop(int opcode, int temp0, int temp1, int temp2) {
  struct cling_ast_ir *self=emit_ir(opcode);
  init_temp(&self->operands[0], temp0, CL_INT);
  init_temp(&self->operands[1], temp1, CL_INT);
  init_temp(&self->operands[2], temp2, CL_INT);
  return self;
}

/*
 * index result(base_wide) temp1(is_rvalue) temp2
 * If is_rvalue, store the value of array temp1 at index temp2
 * into result (a temp).
 * else store the address of array at index into result.
 */
static struct cling_ast_ir *emit_index(int result, int type, int temp1,
                                       int temp2, bool is_rvalue) {
  struct cling_ast_ir *self = emit_ir(OP_IDX);
  init_temp(&self->operands[0], result, type);
  init_temp(&self->operands[1], temp1, CL_INT);
  init_temp(&self->operands[2], temp2, CL_INT);
  self->operands[1].info = is_rvalue;
  return self;
}

/*
 * defvar name(wide|scope)
 * define a single variable called name of wide at scope.
 */
static struct cling_ast_ir *emit_defvar(char const *name, int type, int scope) {
  struct cling_ast_ir *self = emit_ir(OP_DEFVAR);
  init_name(&self->operands[0], scope, type, name);
  return self;
}

/*
 * defunc name(wide)
 * define a function called name returning value of type.
 */
static struct cling_ast_ir *emit_defunc(char const *name, int type) {
  struct cling_ast_ir *self = emit_ir(OP_DEFUNC);
  init_name(&self->operands[0], CL_GLBL, type, name);
  return self;
}

/*
 * para name(wide)
 * define a parameter for a function.
 */
static struct cling_ast_ir *emit_para(char const *name, int type) {
  struct cling_ast_ir *self = emit_ir(OP_PARA);
  init_name(&self->operands[0], CL_LOCL, type, name);
  return self;
}

/*
 * ldimm temp(wide) value
 * load an immediate having `value' into temp.
 */
static struct cling_ast_ir *emit_ldimm(int temp, int type, char const *value) {
  struct cling_ast_ir *self = emit_ir(OP_LDIMM);
  init_temp(&self->operands[0], temp, type);
  init_imme(&self->operands[1], type, value);
  return self;
}

static void cling_ast_ir_destroy(struct cling_ast_ir *self) {
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

static char const *cling_ast_ir_tostring(struct cling_ast_ir const *self) {
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
    if (operand[0].info != CL_NULL)
      snprintf(buffer, AST_IR_BUFSIZ, "ret t%d", operand[0].scalar);
    else
      snprintf(buffer, AST_IR_BUFSIZ, "ret");
    break;
  case OP_NOP:
    return opstr;
  case OP_READ:
    snprintf(buffer, AST_IR_BUFSIZ, "read t%d", operand[0].scalar);
    break;
  case OP_WRINT:
  case OP_WRSTR:
    snprintf(buffer, AST_IR_BUFSIZ, "%s t%d", opstr, operand[0].scalar);
    break;
  case OP_STORE:
    snprintf(buffer, AST_IR_BUFSIZ, "store t%d t%d", operand[0].scalar,
             operand[1].scalar);
    break;
  case OP_PUSH:
    snprintf(buffer, AST_IR_BUFSIZ, "push t%d", operand[0].scalar);
    break;
  case OP_LDSTR:
    snprintf(buffer, AST_IR_BUFSIZ, "ldstr t%d \"%s\"", 
             operand[0].scalar, operand[1].text);
    break;
  case OP_LDIMM:
    snprintf(buffer, AST_IR_BUFSIZ, "ldimm t%d %s", 
             operand[0].scalar, imme_tostring(&operand[1]));
    break;
  case OP_LOAD:
    snprintf(buffer, AST_IR_BUFSIZ, "load t%d %s", operand[0].scalar, operand[1].text);
    break;
  default:
    cling_default_assert(self->opcode, cling_symbol_kind_tostring);
  }
  return buffer;
}

static void cling_ast_ir_print(struct utillib_vector const *instrs) {
  int i = 0;
  struct cling_ast_ir const *ir;
  UTILLIB_VECTOR_FOREACH(ir, instrs) {
    printf("%d: %s\n", i, cling_ast_ir_tostring(ir));
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

static void polish_ir_emit_factor(struct utillib_json_value const *var,
                                  struct cling_ast_ir *ir, int index) {
  assert(var->kind == UT_JSON_INT);
  init_temp(&ir->operands[index], var->as_int, CL_WORD);
}

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
  } else if (op->as_size_t == SYM_EQ || op->as_size_t == SYM_LK) {
    /*
     * Assignment is right associative.
     * We want the array iden to be loaded _after_ the index expr
     * is evaluated so that we can peek its type/wide by looking
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

static void polish_ir_emit_index(struct cling_polish_ir *self,
                                 struct utillib_vector *instrs) {
  struct utillib_json_value const *lhs, *rhs, *temp;
  struct cling_ast_ir *ir;
  int type;

  /*
   * lhs is the address of the array.
   * rhs is the temp holding index expr.
   */
  temp = polish_ir_make_temp(self);
  ir = utillib_vector_back(instrs);
  assert(ir->opcode == OP_LOAD);
  type = ir->operands[0].info;
  lhs = utillib_vector_back(&self->opstack);
  utillib_vector_pop_back(&self->opstack);
  rhs = utillib_vector_back(&self->opstack);
  utillib_vector_pop_back(&self->opstack);
  utillib_vector_push_back(
      instrs,
      /*
       * result, type, array_base_address, index_expr_temp, is_rvalue
       */
      emit_index(temp->as_int, type, lhs->as_int, rhs->as_int, true));
  /*
   * The result represents content or address depending on is_rvalue.
   */
  utillib_vector_push_back(&self->opstack, temp);
  polish_ir_maybe_release_temp(lhs);
  polish_ir_maybe_release_temp(rhs);
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
  struct utillib_json_value *lhs, *arg;
  struct utillib_json_value *value, *temp;
  struct cling_ast_ir *ir;

  /*
   * Fetch all the field about function.
   */
  struct cling_symbol_entry const *entry;
  unsigned int argc;
  int return_type, *argv_types;

  lhs = utillib_vector_back(&self->opstack);
  value = utillib_json_object_at(lhs, "value");
  entry = cling_symbol_table_find(self->global->symbol_table, value->as_ptr,
                                  CL_GLOBAL);
  assert(entry && entry->kind == CL_FUNC);
  return_type = entry->function.return_type;
  argc = entry->function.argc;
  argv_types = entry->function.argv_types;
  utillib_vector_pop_back(&self->opstack);

  /*
   * Only pop argc elements.
   */
  for (int i = 0; i < argc; ++i) {
    arg = utillib_vector_back(&self->opstack);
    utillib_vector_pop_back(&self->opstack);
    utillib_vector_push_back(instrs, emit_push(arg->as_int, argv_types[i]));
    polish_ir_maybe_release_temp(arg);
  }
  if (return_type != CL_VOID) {
    /*
     * If function has a return value,
     * the call will have a temp to hold that.
     * Otherwise, no temp is made or the temp
     * is simply CL_NULL.
     */
    temp = polish_ir_make_temp(self);
    ir = emit_call(value->as_ptr, temp->as_int);
    utillib_vector_push_back(&self->opstack, temp);
  } else {
    ir = emit_call(value->as_ptr, -1);
  }
  utillib_vector_push_back(instrs, ir);
}

static void polish_ir_emit_binary(struct cling_polish_ir *self, size_t op,
                                  struct utillib_vector *instrs) {
  struct utillib_json_value *lhs, *rhs, *temp;
  int opcode;

  rhs = utillib_vector_back(&self->opstack);
  utillib_vector_pop_back(&self->opstack);
  lhs = utillib_vector_back(&self->opstack);
  utillib_vector_pop_back(&self->opstack);
  switch (op) {
  case SYM_ADD:
    opcode=OP_ADD;
    break;
  case SYM_MINUS:
    opcode = OP_SUB;
    break;
  case SYM_MUL:
    opcode = OP_MUL;
    break;
  case SYM_DIV:
    opcode = OP_DIV;
    break;
  case SYM_DEQ:
    opcode = OP_EQ;
    break;
  case SYM_NE:
    opcode = OP_NE;
    break;
  case SYM_LT:
    opcode = OP_LT;
    break;
  case SYM_LE:
    opcode = OP_LE;
    break;
  case SYM_GT:
    opcode = OP_GT;
    break;
  case SYM_GE:
    opcode = OP_GE;
    break;
  default:
    cling_default_assert(op, cling_symbol_kind_tostring);
  }
  temp = polish_ir_make_temp(self);
  utillib_vector_push_back(instrs, 
      emit_binop(opcode, temp->as_int, lhs->as_int, rhs->as_int));
  utillib_vector_push_back(&self->opstack, temp);
  polish_ir_maybe_release_temp(lhs);
  polish_ir_maybe_release_temp(rhs);
}

static void polish_ir_emit_assign(struct cling_polish_ir *self,
                                  struct utillib_vector *instrs) {

  struct utillib_json_value *lhs, *rhs;
  struct cling_ast_ir *ir;
  int type;

  lhs = utillib_vector_back(&self->opstack);
  utillib_vector_pop_back(&self->opstack);
  rhs = utillib_vector_back(&self->opstack);
  utillib_vector_pop_back(&self->opstack);

  /*
   * Perform the Hack.
   */
  ir = utillib_vector_back(instrs);
  switch (ir->opcode) {
  case OP_IDX:
    ir->operands[2].info = false;
    type = ir->operands[1].info;
    break;
  case OP_LOAD:
    ir->operands[1].info = false;
    type = ir->operands[0].info;
    break;
  default:
    cling_default_assert(ir->opcode, cling_ast_opcode_kind_tostring);
  }
  utillib_vector_push_back(instrs, emit_store(lhs->as_int, rhs->as_int, type));
  /*
   * According to the grammar, assign_expr
   * has no value so no need allocate temp.
   */
  polish_ir_maybe_release_temp(lhs);
  polish_ir_maybe_release_temp(rhs);
}

/*
 * Emits a load when the factor is iden, char or int literal
 * or named constant so that they become individual ir rather
 * than mixing up with other commands.
 * Doing so ensures all the operands of add or branch are temps
 * which will greatly simplify the mips codegen.
 */
static void polish_ir_emit_load(struct cling_polish_ir *self,
                                struct utillib_json_value const *object,
                                struct utillib_vector *instrs) {
  struct utillib_json_value const *type, *temp, *value;
  struct cling_symbol_entry const *entry;
  struct cling_ast_ir const *ir;

  value = utillib_json_object_at(object, "value");
  type = utillib_json_object_at(object, "type");
  switch (type->as_size_t) {
  case SYM_IDEN:
    entry = cling_symbol_table_find(self->global->symbol_table, value->as_ptr,
                                    CL_LEXICAL);
    if (entry->kind == CL_FUNC) {
      /*
       * For function we call it directly without loading.
       */
      utillib_vector_push_back(&self->opstack, object);
      return;
    }
    temp = polish_ir_make_temp(self);
    switch (entry->kind) {
    case CL_CONST:
      ir =
          emit_ldimm(temp->as_int, entry->constant.type, entry->constant.value);
      break;
    case CL_ARRAY:
      ir = emit_load(value->as_ptr, entry->scope, entry->array.base_type, 
          temp->as_int, true);
      break;
    default:
      /*
       * Assume every iden is rvalue and fix up
       * later in `polish_ir_emit_assign'.
       */
      ir = emit_load(value->as_ptr, entry->scope, entry->kind, temp->as_int,
                     true);
      break;
    }
    break;
  case SYM_CHAR:
    temp = polish_ir_make_temp(self);
    ir = emit_ldimm(temp->as_int, CL_CHAR, value->as_ptr);
    break;
  case SYM_INTEGER:
  case SYM_UINT:
    temp = polish_ir_make_temp(self);
    ir = emit_ldimm(temp->as_int, CL_INT, value->as_ptr);
    break;
  default:
    cling_default_assert(type->as_size_t, cling_symbol_kind_tostring);
  }
  utillib_vector_push_back(instrs, ir);
  utillib_vector_push_back(&self->opstack, temp);
}

static void cling_polish_ir_emit(struct cling_polish_ir *self,
                                 struct utillib_vector *instrs) {
  struct utillib_json_value const *object, *op, *lhs, *rhs;
  struct cling_ast_ir *ir;
  struct utillib_json_value *temp;
  struct utillib_vector *stack = &self->stack;
  struct utillib_vector *opstack = &self->opstack;

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

static void cling_polish_ir_result(struct cling_polish_ir const *self,
                                   struct cling_ast_ir *ir, int index) {
  struct utillib_json_value *result = utillib_vector_back(&self->opstack);
  polish_ir_emit_factor(result, ir, index);
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
  int temp;

  arglist = utillib_json_object_at(self, "arglist");
  UTILLIB_JSON_ARRAY_FOREACH(object, arglist) {
    value = utillib_json_object_at(object, "value");
    entry = cling_symbol_table_find(global->symbol_table, value->as_ptr,
                                    CL_LEXICAL);
    temp = make_temp(global);
    utillib_vector_push_back(instrs, emit_load(value->as_ptr, entry->scope,
                                               entry->kind, temp, false));
    utillib_vector_push_back(instrs, emit_read(temp, entry->kind));
  }
}

static void emit_printf_stmt(struct utillib_json_value const *self,
                             struct cling_ast_ir_global *global,
                             struct utillib_vector *instrs) {
  struct utillib_json_value const *value, *arglist;
  struct utillib_json_value const *type, *object;
  int temp;
  struct cling_polish_ir polish_ir;
  arglist = utillib_json_object_at(self, "arglist");

  UTILLIB_JSON_ARRAY_FOREACH(object, arglist) {
    type = utillib_json_object_at(object, "type");
    if (type && type->as_size_t == SYM_STRING) {
      value = utillib_json_object_at(object, "value");
      temp = make_temp(global);
      utillib_vector_push_back(instrs, emit_ldstr(temp, value->as_ptr));
      utillib_vector_push_back(instrs, emit_wrstr(temp));
    } else {
      cling_polish_ir_init(&polish_ir, object, global);
      cling_polish_ir_emit(&polish_ir, instrs);
      temp = polish_ir_result(&polish_ir);
      utillib_vector_push_back(instrs, emit_wrint(temp));
      cling_polish_ir_destroy(&polish_ir);
    }
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
  int loop_jump_jta, tricky_jump_jta;

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
  cling_polish_ir_result(&polish_ir, cond_test, 0);
  utillib_vector_push_back(instrs, cond_test);
  /*
   * The JTA of tricky_jump is the next instr of
   * cond_test, which is also the beginning of
   * body.
   */
  tricky_jump->operands[0].scalar = utillib_vector_size(instrs);
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
  loop_jump->operands[0].scalar = loop_jump_jta;
  utillib_vector_push_back(instrs, loop_jump);
  cond_test->operands[1].scalar = utillib_vector_size(instrs);

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
static struct cling_ast_ir *
emit_case_gaurd(size_t symbol, char const *value,
                struct cling_polish_ir const *polish_ir) {
  struct cling_ast_ir *case_gaurd = emit_ir(OP_BNE);
  init_imme(&case_gaurd->operands[0], cling_symbol_to_type(symbol), value);
  init_temp(&case_gaurd->operands[1], polish_ir_result(polish_ir), CL_WORD);
  return case_gaurd;
}

static void emit_switch_stmt(struct utillib_json_value const *self,
                             struct cling_ast_ir_global *global,
                             struct utillib_vector *instrs) {
  struct cling_polish_ir polish_ir;
  struct utillib_json_value const *object, *case_clause, *expr;
  struct utillib_json_value const *value, *case_, *stmt, *type;
  struct cling_ast_ir *case_gaurd, *break_jump;
  /*
   * break_jumps keeps all the break_jump of
   * the case_clauses to fill their JTA to
   * the end of switch_stmt later.
   */
  struct utillib_vector break_jumps;
  int break_jump_jta;

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
      case_gaurd = emit_case_gaurd(type->as_size_t, value->as_ptr, &polish_ir);
      utillib_vector_push_back(instrs, case_gaurd);
      emit_statement(stmt, global, instrs);
      break_jump = emit_ir(OP_JMP);
      utillib_vector_push_back(instrs, break_jump);
      utillib_vector_push_back(&break_jumps, break_jump);
      /*
       * Fills in the BTA of case_gaurd, which
       * is the next instr of break_jump.
       */
      case_gaurd->operands[2].scalar = utillib_vector_size(instrs);
    } else {
      /*
       * default clause does not need a break_jump.
       */
      emit_statement(stmt, global, instrs);
      break_jump_jta = utillib_vector_size(instrs);
    }
  }

  UTILLIB_VECTOR_FOREACH(break_jump, &break_jumps) {
    break_jump->operands[0].scalar = break_jump_jta;
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
    cling_polish_ir_result(&polish_ir, ir, 0);
    cling_polish_ir_destroy(&polish_ir);
  } else {
    init_null(&ir->operands[0]);
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
  cling_polish_ir_result(&polish_ir, skip_branch, 0);
  utillib_vector_push_back(instrs, skip_branch);

  emit_statement(then_clause, global, instrs);
  else_clause = utillib_json_object_at(self, "else");
  if (else_clause) {
    jump = emit_ir(OP_JMP);
    utillib_vector_push_back(instrs, jump);
    skip_branch->operands[1].scalar = utillib_vector_size(instrs);
    emit_statement(else_clause, global, instrs);
    jump->operands[0].scalar = utillib_vector_size(instrs);
  } else {
    skip_branch->operands[1].scalar = utillib_vector_size(instrs);
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
    cling_default_assert(type->as_size_t, cling_symbol_kind_tostring);
  }
}

/*
 * Emits defvar ir.
 * defvar name(wide|scope) extend(is_array)
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
      ir = emit_defarr(name->as_ptr,  entry->scope,entry->array.base_type,
                       entry->array.extend);
    } else {
      ir = emit_defvar(name->as_ptr, entry->kind, entry->scope);
    }
    utillib_vector_push_back(instrs, ir);
  }
}

/*
 * Emits defcon ir.
 * defcon name(wide|scope) value
 */
static void emit_const_defs(struct utillib_json_value const *self,
                            struct cling_ast_ir_global *global,
                            struct utillib_vector *instrs) {
  struct utillib_json_value const *defs, *decl;
  struct utillib_json_value const *name;
  struct cling_symbol_entry const *entry;

  defs = utillib_json_object_at(self, "const_defs");

  UTILLIB_JSON_ARRAY_FOREACH(decl, defs) {
    name = utillib_json_object_at(decl, "name");
    entry =
        cling_symbol_table_find(global->symbol_table, name->as_ptr, CL_LEXICAL);
    utillib_vector_push_back(instrs,
                             emit_defcon(name->as_ptr,entry->scope, 
                               entry->constant.type,  entry->constant.value));
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
  self->name = strdup(name);
  self->temps = 0;
  utillib_vector_init(&self->instrs);
  utillib_vector_init(&self->init_code);
  return self;
}

static void cling_ast_function_destroy(struct cling_ast_function *self) {
  free(self->name);
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
/*
 * Create an cling_ast_function from the func_node.
 */
static struct cling_ast_function *
cling_ast_ir_emit_function(struct utillib_json_value const *func_node,
                           struct cling_ast_ir_global *global) {
  struct cling_ast_function *self;
  struct utillib_json_value const *type, *name, *arglist, *arg, *comp;
  struct cling_symbol_entry const *entry;

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
  utillib_vector_push_back(
      &self->init_code, emit_defunc(name->as_ptr, entry->function.return_type));
  UTILLIB_JSON_ARRAY_FOREACH(arg, arglist) {
    name = utillib_json_object_at(arg, "name");
    entry =
        cling_symbol_table_find(global->symbol_table, name->as_ptr, CL_LOCAL);
    utillib_vector_push_back(&self->init_code,
                             emit_para(name->as_ptr, entry->kind));
  }
  maybe_emit_decls(comp, global, &self->init_code);

  /*
   * instrs emision
   */
  emit_composite(comp, global, &self->instrs);
  utillib_vector_push_back(&self->instrs, &cling_ast_ir_nop);
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

void cling_ast_program_print(struct cling_ast_program const *self) {
  struct cling_ast_function const *func;

  cling_ast_ir_print(&self->init_code);
  UTILLIB_VECTOR_FOREACH(func, &self->funcs) {
    cling_ast_ir_print(&func->init_code);
    cling_ast_ir_print(&func->instrs);
    puts("");
  }
}

/*
 * C99 inline semantic requires a declaration in translation unit.
 */
int cling_ast_ir_temp(struct cling_ast_ir const *self, int index);
int cling_ast_ir_info(struct cling_ast_ir const *self, int index);
char const *cling_ast_ir_name(struct cling_ast_ir const *self, int index);
int cling_ast_ir_address(struct cling_ast_ir const *self, int index);
bool cling_ast_index_is_rvalue(struct cling_ast_ir const *self);
bool cling_ast_load_is_rvalue(struct cling_ast_ir const *self);
int cling_ast_index_base_wide(struct cling_ast_ir const *self);
int cling_ast_defarr_extend(struct cling_ast_ir const *self);
char const *cling_ast_ir_string(struct cling_ast_ir const *self);
