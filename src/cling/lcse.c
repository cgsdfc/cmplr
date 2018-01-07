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
 * How one instruction is considered computed is based
 * on the following functions
 */

static int operand_compare(struct cling_lcse_ir const *lhs,
                           struct cling_lcse_ir const *rhs) {
  if (lhs->binary.temp1 == rhs->binary.temp1 &&
      lhs->binary.temp2 == rhs->binary.temp2)
    return 0;
  return 1;
}

static int reversed_compare(struct cling_lcse_ir const *lhs,
                            struct cling_lcse_ir const *rhs) {
  if (lhs->binary.temp1 == rhs->binary.temp2 &&
      lhs->binary.temp2 == rhs->binary.temp1)
    return 0;
  return 1;
}

static int binary_compare(struct cling_lcse_ir const *lhs,
                          struct cling_lcse_ir const *rhs) {
  if (lhs->opcode != rhs->opcode)
    return 1;
  return operand_compare(lhs, rhs);
}

/*
 * A communicative_compare means A + B == B + A
 */
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
 * load_addr: name scope
 */
static int load_addr_compare(struct cling_lcse_ir const *lhs,
                               struct cling_lcse_ir const *rhs) {
  if (lhs->opcode != rhs->opcode || lhs->kind != rhs->kind)
    return 1;
  if (0 == strcmp(lhs->load_addr.name, rhs->load_addr.name) &&
      lhs->load_addr.scope == rhs->load_addr.scope)
    return 0;
  return 1;
}

/*
 * load_value: value scope
 */
static int load_value_compare(struct cling_lcse_ir const *lhs,
                               struct cling_lcse_ir const *rhs) {
  if (lhs->opcode != rhs->opcode || lhs->kind != rhs->kind)
    return 1;
  if (lhs->load_value.value == rhs->load_value.value && 
      lhs->load_value.scope == rhs->load_value.scope)
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

static int unary_compare(struct cling_lcse_ir const *lhs,
                         struct cling_lcse_ir const *rhs) {
  if (lhs->opcode == rhs->opcode && lhs->unary.operand == rhs->unary.operand)
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
  case LCSE_UNARY:
    return unary_compare(lhs, rhs);
  case LCSE_LOAD_ADDR:
    return load_addr_compare(lhs, rhs);
  case LCSE_LOAD_VALUE:
    return load_value_compare(lhs, rhs);
  case LCSE_STORE:
    return store_compare(lhs, rhs);
  default:
    assert(false);
  }
}

static size_t lcse_ir_hash(struct cling_lcse_ir const *self) {
  size_t hash = self->opcode;
  switch (self->kind) {
  case LCSE_UNARY:
    return hash + self->unary.operand;
  case LCSE_BINARY:
    return hash + self->binary.temp1 + self->binary.temp2;
  case LCSE_LOAD_ADDR:
    return hash + self->load_addr.scope +
           mysql_strhash(self->load_addr.name);
  case LCSE_LOAD_VALUE:
    return hash + self->load_value.value + self->load_value.scope;
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

/*
 * Lookup a renamed variable or allocate a new one
 */
static unsigned int lookup_variable(struct cling_lcse_optimizer *self,
                                    unsigned int temp) {
  assert(temp < self->variables_size);
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
 * for various mappings. Group them into LCSE_ settings.
 */
static void translate(struct cling_lcse_optimizer *self,
                      struct cling_ast_ir *ast_ir,
                      struct cling_lcse_ir *lcse_ir) {
  unsigned int address;
  lcse_ir->opcode = ast_ir->opcode;
  switch (ast_ir->opcode) {
  case OP_INDEX:
    /*
     * index has a different format from thoer binop
     */
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
  case OP_LDIMM:
    lcse_ir->kind=LCSE_UNARY;
    lcse_ir->unary.operand=ast_ir->ldimm.value;
    break;
  case OP_LDNAM:
      address = lookup_named_address(self, ast_ir->ldnam.name);
      lcse_ir->load_value.value = lookup_value(self, address);
      lcse_ir->load_value.scope=ast_ir->ldnam.scope;
      lcse_ir->kind = LCSE_LOAD_VALUE;
      break;
  case OP_LDADR:
      lcse_ir->load_addr.name = ast_ir->ldadr.name;
      lcse_ir->load_addr.scope = ast_ir->ldadr.scope;
      lcse_ir->kind = LCSE_LOAD_ADDR;
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

/*
 * Check for existence of an lcse_ir, fix up the ast_ir based
 * on that and update the operations set if not existed
 */
static bool insert_operation(struct cling_lcse_optimizer *self,
                             struct cling_ast_ir *ast_ir,
                             struct cling_lcse_ir *lcse_ir) {
  struct cling_lcse_ir *new_ir;

  new_ir = utillib_hashmap_find(&self->operations, lcse_ir);
  if (new_ir) {
    /*
     * If this new_ir already existed, map the temp in ast_ir
     * to those in new_ir.
     */
    switch (new_ir->kind) {
    case LCSE_UNARY:
      /*
       * Currently only ldimm.
       */
      self->variables[ast_ir->ldimm.temp]=new_ir->unary.result;
      break;
    case LCSE_BINARY:
      self->variables[ast_ir->binop.result] = new_ir->binary.result;
      break;
    case LCSE_LOAD_ADDR:
      self->variables[ast_ir->ldadr.temp] = new_ir->load_addr.address;
      break;
    case LCSE_LOAD_VALUE:
      self->variables[ast_ir->ldnam.temp] = new_ir->load_value.value;
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
   * Ops, new stuff!
   */
  new_ir = malloc(sizeof *new_ir);
  memcpy(new_ir, lcse_ir, sizeof *new_ir);
  switch (new_ir->kind) {
  case LCSE_UNARY:
    switch(new_ir->opcode) {
      case OP_LDIMM:
        ast_ir->ldimm.temp=lookup_variable(self, ast_ir->ldimm.temp);
        new_ir->unary.result=ast_ir->ldimm.temp;
        break;
      default: assert(false);
    }
    break;
  case LCSE_BINARY:
    ast_ir->binop.result=lookup_variable(self, ast_ir->binop.result); 
    ast_ir->binop.temp1=new_ir->binary.temp1;
    ast_ir->binop.temp2=new_ir->binary.temp2;
    new_ir->binary.result =ast_ir->binop.result;
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
  case LCSE_LOAD_ADDR:
    ast_ir->ldadr.temp=lookup_variable(self, ast_ir->ldadr.temp);
    new_ir->load_addr.address=ast_ir->ldadr.temp;
    break;
  case LCSE_LOAD_VALUE:
    /*
     * Tricky HERE!
     */
    self->variables[ast_ir->ldnam.temp]=new_ir->load_value.value;
    ast_ir->ldnam.temp = new_ir->load_value.value;
    break;
  default:
    assert(false);
  }
  utillib_hashmap_insert(&self->operations, new_ir, new_ir);
  return true;
}

/*
 * Optimize on a single basic_block
 */
static void lcse_optimize(struct cling_lcse_optimizer *self,
                     struct cling_basic_block const *block,
                     struct utillib_vector *output_instrs) {
  bool add_instr;
  struct cling_ast_ir *ast_ir;
  struct cling_lcse_ir lcse_ir;
  int value, argc;

  for (int i = block->begin; i < block->end; ++i) {
    add_instr = true;
    ast_ir = utillib_vector_at(block->instrs, i);
    self->address_map[i] = utillib_vector_size(output_instrs);
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
    case OP_INDEX:
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
      /*
       * Unary
       */
    case OP_LDIMM:
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
      for (argc=0; argc<ast_ir->call.argc; ++argc) {
        ast_ir->call.argv[argc]=lookup_variable(self, ast_ir->call.argv[argc]);
      }
      if (ast_ir->call.has_result) {
        value = lookup_variable(self, ast_ir->call.result);
        ast_ir->call.result = value;
      }
      break;
    case OP_READ:
      /*
       * read always introduce new variables.
       */
      ast_ir->read.temp = lookup_variable(self, ast_ir->read.temp);
      break;
    case OP_LDSTR:
      ast_ir->ldstr.temp = lookup_variable(self, ast_ir->ldstr.temp);
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
    case OP_NL:
      break;
    default:
      assert(false);
    }
    if (add_instr)
      utillib_vector_push_back(output_instrs, ast_ir);
    else
      cling_ast_ir_destroy(ast_ir);
  }
}

static void lcse_ir_destroy(struct cling_lcse_ir *self) { free(self); }

void cling_lcse_optimizer_init(struct cling_lcse_optimizer *self,
                               struct cling_ast_function const *ast_func) {
  size_t temp_size = ast_func->temps;
  self->var_count = 0;
  self->variables = malloc(sizeof self->variables[0] * temp_size);
  memset(self->variables, -1, sizeof self->variables[0] * temp_size);
  self->variables_size=temp_size;
  self->address_map = malloc(sizeof self->address_map[0] *
                             utillib_vector_size(&ast_func->instrs));
  utillib_hashmap_init(&self->names, &mips_label_strcallback);
}

void cling_lcse_optimizer_destroy(struct cling_lcse_optimizer *self) {
  free(self->address_map);
  free(self->variables);
  utillib_hashmap_destroy_owning(&self->names, NULL, mips_label_destroy);
}

void cling_lcse_optimizer_emit(struct cling_lcse_optimizer *self,
                               struct utillib_vector const *basic_blocks,
                               struct cling_ast_function *ast_func)
{
  struct utillib_vector output_instrs;
  struct cling_basic_block const *block;
  utillib_vector_init(&output_instrs);

  UTILLIB_VECTOR_FOREACH(block, basic_blocks) {
    utillib_hashmap_init(&self->operations, &lcse_ir_callback);
    utillib_hashmap_init(&self->values, &lcse_value_intcallback);
    lcse_optimize(self, block, &output_instrs);
    utillib_hashmap_destroy_owning(&self->operations, NULL, lcse_ir_destroy);
    utillib_hashmap_destroy_owning(&self->values, NULL, lcse_value_destroy);
  }
  ast_func->temps=self->var_count;
  ast_ir_fix_address(&output_instrs, self->address_map);
  utillib_vector_clear(&ast_func->instrs);
  utillib_vector_append(&ast_func->instrs, &output_instrs);
  utillib_vector_destroy(&output_instrs);
}
