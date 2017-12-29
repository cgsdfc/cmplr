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
#include "lcse.h"
#include "ast_ir.h"
#include "basic_block.h"
#include "misc.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <utillib/strhash.h>

/*
 * cling_lcse_ir hash and compare
 */

static int operand_compare(struct cling_lcse_ir const *lhs,
                           struct cling_lcse_ir const *rhs) {
  if (lhs->binary.temp1 == rhs->binary.temp1 &&
      lhs->binary.temp2 == rhs->binary.temp2)
    return 0;
  return 1;
}

static int binary_compare(struct cling_lcse_ir const *lhs,
                          struct cling_lcse_ir const *rhs) {
  if (lhs->opcode != rhs->opcode)
    return 1;
  return operand_compare(lhs, rhs);
}

static int reversed_compare(struct cling_lcse_ir const *lhs,
                            struct cling_lcse_ir const *rhs) {
  if (lhs->binary.temp1 == rhs->binary.temp2 &&
      lhs->binary.temp2 == rhs->binary.temp1)
    return 0;
  return 1;
}

static int communicative_compare(struct cling_lcse_ir const *lhs,
                                 struct cling_lcse_ir const *rhs) {
  if (lhs->opcode != rhs->opcode)
    return 1;
  if (0 == operand_compare(lhs, rhs))
    return 0;
  if (0 == reversed_compare(lhs, rhs))
    return 0;
  return 1;
}

/*
 * A interchange of operands and opcode will match if their
 * outputs are the same. Instances are a <= b means b > a.
 */
static int interchangable_compare(struct cling_lcse_ir const *lhs,
                                  struct cling_lcse_ir const *rhs) {
  if (lhs->opcode == rhs->opcode)
    return operand_compare(lhs, rhs);
  if (lhs->opcode == OP_GE && rhs->opcode == OP_LT ||
      lhs->opcode == OP_GT && rhs->opcode == OP_LE ||
      lhs->opcode == OP_LE && rhs->opcode == OP_GT ||
      lhs->opcode == OP_LT && rhs->opcode == OP_GE)
    return reversed_compare(lhs, rhs);
  return 1;
}

/*
 * load_lvalue: name scope
 */
static int load_lvalue_compare(struct cling_lcse_ir const *lhs,
                               struct cling_lcse_ir const *rhs) {
  if (lhs->opcode != rhs->opcode || lhs->kind != rhs->kind)
    return 1;
  if (0 == strcmp(lhs->load_lvalue.name, rhs->load_lvalue.name) &&
      lhs->load_lvalue.scope == rhs->load_lvalue.scope)
    return 0;
  return 1;
}

/*
 * load_rvalue: value scope
 */
static int load_rvalue_compare(struct cling_lcse_ir const *lhs,
                               struct cling_lcse_ir const *rhs) {
  if (lhs->opcode != rhs->opcode || lhs->kind != rhs->kind)
    return 1;
  if (lhs->load_rvalue.value == rhs->load_rvalue.value && 
      lhs->load_rvalue.scope == rhs->load_rvalue.scope)
    return 0;
  return 1;
}

/*
 * store the same thing to the save location
 */
static int store_compare(struct cling_lcse_ir const *lhs,
                         struct cling_lcse_ir const *rhs) {
  if (lhs->opcode != rhs->opcode)
    return 1;
  if (lhs->store.value == rhs->store.value &&
      lhs->store.address == rhs->store.address)
    return 0;
  return 1;
}

static int lcse_ir_compare(struct cling_lcse_ir const *lhs,
                           struct cling_lcse_ir const *rhs) {
  switch (lhs->kind) {
  case LCSE_BINARY:
    switch (lhs->opcode) {
    case OP_ADD:
    case OP_MUL:
    case OP_EQ:
    case OP_NE:
      return communicative_compare(lhs, rhs);
    case OP_LT:
    case OP_LE:
    case OP_GT:
    case OP_GE:
      return interchangable_compare(lhs, rhs);
    case OP_INDEX:
    case OP_DIV:
    case OP_SUB:
      return binary_compare(lhs, rhs);
    default:
      assert(false);
    }
  case LCSE_LOAD_LVALUE:
    return load_lvalue_compare(lhs, rhs);
  case LCSE_LOAD_RVALUE:
    return load_rvalue_compare(lhs, rhs);
  case LCSE_STORE:
    return store_compare(lhs, rhs);
  default:
    assert(false);
  }
}

static size_t lcse_ir_hash(struct cling_lcse_ir const *self) {
  size_t hash = self->opcode;
  switch (self->kind) {
  case LCSE_BINARY:
    return hash + self->binary.temp1 + self->binary.temp2;
  case LCSE_LOAD_LVALUE:
    return hash + self->load_lvalue.scope +
           mysql_strhash(self->load_lvalue.name);
  case LCSE_LOAD_RVALUE:
    return hash + self->load_rvalue.value + self->load_rvalue.scope;
  case LCSE_STORE:
    return hash + self->store.value + self->store.address;
  default:
    assert(false);
  }
}

static const struct utillib_hashmap_callback lcse_ir_callback = {
    .hash_handler = lcse_ir_hash, .compare_handler = lcse_ir_compare,
};

/*
 * cling_lcse_value an address-value pair.
 */

static struct cling_lcse_value *lcse_value_create(unsigned int address,
                                                  unsigned int value) {
  struct cling_lcse_value *self = malloc(sizeof *self);
  self->address = address;
  self->value = value;
  return self;
}

static void lcse_value_destroy(struct cling_lcse_value *self) { free(self); }

static int lcse_value_compare(struct cling_lcse_value const *lhs,
                              struct cling_lcse_value const *rhs) {
  return lhs->address - rhs->address;
}

static size_t lcse_value_hash(struct cling_lcse_value const *self) {
  return self->address;
}

static const struct utillib_hashmap_callback lcse_value_intcallback = {
    .hash_handler = lcse_value_hash, .compare_handler = lcse_value_compare,
};

/*
 * Lookup the address of this name. Address is just variable counted by
 * var_count.
 */
static unsigned int lookup_named_address(struct cling_lcse_optimizer *self,
                                         char const *name) {
  struct cling_mips_label *label;
  label = mips_label_name_find(&self->names, name);
  if (label) {
    return label->address;
  }
  label = mips_label_create(name, self->var_count);
  utillib_hashmap_insert(&self->names, label, label);
  ++self->var_count;
  return label->address;
}

/*
 * Lookup the value at address. If not found, allocate a new one from var_count.
 */
static unsigned int lookup_value(struct cling_lcse_optimizer *self,
                                 unsigned int address) {
  struct cling_lcse_value *val, v;
  v.address = address;
  val = utillib_hashmap_find(&self->values, &v);
  if (val) {
    return val->value;
  }
  val = lcse_value_create(address, self->var_count);
  utillib_hashmap_insert(&self->values, val, val);
  ++self->var_count;
  return val->value;
}

/*
 * Update the value at address (force insertion)
 */
static void update_value(struct cling_lcse_optimizer *self, unsigned int address, unsigned int value) {
  struct cling_lcse_value *val, v;
  v.address=address;
  val=utillib_hashmap_find(&self->values, &v);
  if (val) {
    val->value=value;
    return;
  }
  val=lcse_value_create(address, value);
  utillib_hashmap_insert(&self->values, val, val);
}

static unsigned int lookup_variable(struct cling_lcse_optimizer *self,
                                    unsigned int temp) {
  int var = self->variables[temp];
  if (var != -1)
    return var;
  var = self->var_count;
  self->variables[temp] = var;
  ++self->var_count;
  return var;
}

/*
 * Prepare operands for a lcse_ir by looking the operands of ast_ir
 * for various maps.
 */
static void translate(struct cling_lcse_optimizer *self,
                      struct cling_ast_ir *ast_ir,
                      struct cling_lcse_ir *lcse_ir) {
  unsigned int address;
  lcse_ir->opcode = ast_ir->opcode;
  switch (ast_ir->opcode) {
    case OP_INDEX:
      lcse_ir->kind=LCSE_BINARY;
      lcse_ir->binary.temp1=lookup_variable(self, ast_ir->index.array_addr);
      lcse_ir->binary.temp2=lookup_variable(self, ast_ir->index.index_result);
      break;
  case OP_LT:
  case OP_LE:
  case OP_GT:
  case OP_GE:
  case OP_ADD:
  case OP_MUL:
  case OP_EQ:
  case OP_NE:
  case OP_DIV:
  case OP_SUB:
    lcse_ir->kind = LCSE_BINARY;
    lcse_ir->binary.temp1 = lookup_variable(self, ast_ir->binop.temp1);
    lcse_ir->binary.temp2 = lookup_variable(self, ast_ir->binop.temp2);
    break;
  case OP_LDNAM:
      address = lookup_named_address(self, ast_ir->ldnam.name);
      lcse_ir->load_rvalue.value = lookup_value(self, address);
      self->variables[ast_ir->ldnam.temp]=lcse_ir->load_rvalue.value;
      lcse_ir->load_rvalue.scope=ast_ir->ldnam.scope;
      lcse_ir->kind = LCSE_LOAD_RVALUE;
  case OP_LDADR:
      /*
       * scope and name
       */
      address=lookup_named_address(self, ast_ir->ldadr.name); 
      lcse_ir->load_lvalue.name = ast_ir->ldadr.name;
      self->variables[ast_ir->ldadr.temp]=address;
      lcse_ir->load_lvalue.scope = ast_ir->ldadr.scope;
      lcse_ir->kind = LCSE_LOAD_LVALUE;
    break;
  case OP_STNAM:
    lcse_ir->store.address=lookup_named_address(self, ast_ir->stnam.name);
    lcse_ir->store.value=lookup_variable(self, ast_ir->stnam.value);
    lcse_ir->kind=LCSE_STORE;
    break;
  case OP_STADR:
    lcse_ir->store.address=lookup_variable(self, ast_ir->stadr.addr);
    lcse_ir->store.value=lookup_variable(self, ast_ir->stadr.value);
    lcse_ir->kind=LCSE_STORE;
    break;
  case OP_DEREF:
    lcse_ir->kind=LCSE_STORE;
    lcse_ir->store.address=lookup_variable(self, ast_ir->deref.addr);
    lcse_ir->store.value=lookup_value(self, lcse_ir->store.address);
    break;
  default:
    assert(false);
  }
}

static bool insert_operation(struct cling_lcse_optimizer *self,
                             struct cling_ast_ir *ast_ir,
                             struct cling_lcse_ir *lcse_ir) {
  struct cling_lcse_ir *new_ir;

  new_ir = utillib_hashmap_find(&self->operations, lcse_ir);
  if (new_ir) {
    switch (new_ir->kind) {
    case LCSE_BINARY:
      self->variables[ast_ir->binop.result] = new_ir->binary.result;
      break;
    case LCSE_LOAD_LVALUE:
      self->variables[ast_ir->ldadr.temp] = new_ir->load_lvalue.address;
      break;
    case LCSE_LOAD_RVALUE:
      self->variables[ast_ir->ldnam.temp] = new_ir->load_rvalue.value;
      break;
    case LCSE_STORE:
      /*
       * store produces no new value so nothing need to be done.
       */
      break;
    }
    return false;
  }

  /*
   * Lookup others part of a lcse_ir and fill the ast_ir based
   * on lcse_ir.
   */
  new_ir = malloc(sizeof *new_ir);
  memcpy(new_ir, lcse_ir, sizeof *new_ir);
  switch (new_ir->kind) {
  case LCSE_BINARY:
    new_ir->binary.result =lookup_variable(self, ast_ir->binop.result); 
    ast_ir->binop.result = new_ir->binary.result;
    ast_ir->binop.temp1=new_ir->binary.temp1;
    ast_ir->binop.temp2=new_ir->binary.temp2;
    break;
  case LCSE_STORE:
    /*
     * The value at address is updated.
     */
    update_value(self, new_ir->store.address, new_ir->store.value);
    switch(new_ir->opcode) {
    case OP_STADR:
      ast_ir->stadr.addr=new_ir->store.address;
      ast_ir->stadr.value=new_ir->store.value;
      break;
    case OP_STNAM:
      ast_ir->stnam.value=new_ir->store.value;
      break;
    case OP_DEREF:
      ast_ir->deref.addr=new_ir->store.address;
      break;
    default: assert(false);
    }
    break;
  case LCSE_LOAD_LVALUE:
    new_ir->load_lvalue.address =lookup_variable(self, ast_ir->ldadr.temp); 
    ast_ir->ldadr.temp = new_ir->load_lvalue.address;
    break;
  case LCSE_LOAD_RVALUE:
    ast_ir->ldnam.temp = lookup_variable(self, ast_ir->ldnam.temp);
    break;
  default:
    assert(false);
  }
  utillib_hashmap_insert(&self->operations, new_ir, new_ir);
  return true;
}

static void optimize(struct cling_lcse_optimizer *self,
                     struct cling_basic_block const *block,
                     struct utillib_vector *instrs) {
  bool add_instr;
  struct cling_ast_ir *ast_ir;
  struct cling_lcse_ir lcse_ir;
  int value, index, array, temp, result;

  for (int i = block->begin; i < block->end; ++i) {
    add_instr = true;
    ast_ir = utillib_vector_at(block->instrs, i);
    self->address_map[i] = utillib_vector_size(instrs);
    switch (ast_ir->opcode) {
    /*
     * relop.
     */
    case OP_LT:
    case OP_LE:
    case OP_GT:
    case OP_GE:
    case OP_EQ:
    case OP_NE:
    /*
     * arithop
     */
    case OP_ADD:
    case OP_MUL:
    case OP_DIV:
    case OP_SUB:
    /*
     * Load/Store
     */
    case OP_LDADR:
    case OP_LDNAM:
    case OP_DEREF:
    case OP_STADR:
    case OP_STNAM:
      translate(self, ast_ir, &lcse_ir);
      add_instr = insert_operation(self, ast_ir, &lcse_ir);
      break;
    case OP_RET:
      if (ast_ir->ret.has_result) {
        value = lookup_variable(self, ast_ir->ret.result);
        ast_ir->ret.result = value;
      }
      break;
    case OP_CAL:
      if (ast_ir->call.has_result) {
        value = lookup_variable(self, ast_ir->call.result);
        ast_ir->call.result = value;
      }
      break;
    case OP_INDEX:
      /*
       * We always do it no matter lvalue or rvalue.
       */
      array = lookup_variable(self, ast_ir->index.array_addr);
      index = lookup_variable(self, ast_ir->index.index_result);
      result = lookup_variable(self, ast_ir->index.result);
      ast_ir->index.array_addr = array;
      ast_ir->index.index_result = index;
      ast_ir->index.result = result;
      break;
    case OP_READ:
      /*
       * read always introduce new variables.
       */
      ast_ir->read.temp = lookup_variable(self, ast_ir->read.temp);
      break;
    case OP_LDIMM:
      /*
       * We also do not track ldimm since that's a matter of
       * constant propergation. We assume every ldimm introduces new value.
       */
      ast_ir->ldimm.temp = lookup_variable(self, ast_ir->ldimm.temp);
      break;
    case OP_LDSTR:
      /*
       * We always use 0 to ldstr since string can only be printed
       * and never produces new value.
       */
      temp = ast_ir->ldstr.temp;
      self->variables[temp] = LCSE_TEMP_ZERO;
      ast_ir->ldstr.temp = LCSE_TEMP_ZERO;
      break;
    case OP_WRITE:
      /*
       * write is just a read-only operation on value.
       */
      ast_ir->write.temp = lookup_variable(self, ast_ir->write.temp);
      break;
    case OP_BEZ:
      ast_ir->bez.temp=lookup_variable(self, ast_ir->bez.temp);
      break;
    case OP_BNE:
      ast_ir->bne.temp1=lookup_variable(self, ast_ir->bne.temp1);
      ast_ir->bne.temp2=lookup_variable(self, ast_ir->bne.temp2);
      break;
    case OP_JMP:
      break;
    case OP_NOP:
      break;
    default:
      puts(cling_ast_opcode_kind_tostring(ast_ir->opcode));
      assert(false);
    }
    if (add_instr) {
      /* ast_ir_print(ast_ir, stdout); */
      /* puts(""); */
      utillib_vector_push_back(instrs, ast_ir);
    }
    else {
      cling_ast_ir_destroy(ast_ir);
    }
  }
}

static void lcse_ir_destroy(struct cling_lcse_ir *self) { free(self); }

void cling_lcse_optimizer_init(struct cling_lcse_optimizer *self,
                               struct cling_ast_function const *ast_func) {
  size_t temp_size = ast_func->temps;
  self->var_count = LCSE_TEMP_ZERO + 1;
  self->variables = malloc(sizeof self->variables[0] * temp_size);
  /*
   * Different basic_blocks do not share temps.
   * So initialize it onece is OK.
   */
  memset(self->variables, -1, sizeof self->variables[0] * temp_size);
  self->address_map = malloc(sizeof self->address_map[0] *
                             utillib_vector_size(&ast_func->instrs));
  /*
   * Different basic_blocks can share address of names
   */
  utillib_hashmap_init(&self->names, &mips_label_strcallback);
}

void cling_lcse_optimizer_destroy(struct cling_lcse_optimizer *self) {
  free(self->address_map);
  free(self->variables);
  utillib_hashmap_destroy_owning(&self->names, NULL, mips_label_destroy);
}

void cling_lcse_optimizer_emit(struct cling_lcse_optimizer *self,
                               struct cling_basic_block const *block,
                               struct utillib_vector *instrs) {
  /*
   * Different basic_blocks should not share operations and values.
   */
  utillib_hashmap_init(&self->operations, &lcse_ir_callback);
  utillib_hashmap_init(&self->values, &lcse_value_intcallback);
  optimize(self, block, instrs);
  utillib_hashmap_destroy_owning(&self->operations, NULL, lcse_ir_destroy);
  utillib_hashmap_destroy_owning(&self->values, NULL, lcse_value_destroy);
}
