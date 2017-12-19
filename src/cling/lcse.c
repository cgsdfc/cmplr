#include "lcse.h"
#include "ast_ir.h"
#include "mips.h"
#include "basic_block.h"
#include <utillib/strhash.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

/*
 * cling_lcse_ir hash and compare
 */

static int operand_compare(struct cling_lcse_ir const *lhs, struct cling_lcse_ir const *rhs)
{
  if (lhs->binary.temp1 == rhs->binary.temp1 &&
      lhs->binary.temp2 == rhs->binary.temp2)
    return 0;
  return 1;
}

static int binary_compare(struct cling_lcse_ir const *lhs, struct cling_lcse_ir const *rhs)
{
  if (lhs->opcode != rhs->opcode)
    return 1;
  return operand_compare(lhs, rhs);
}

static int reversed_compare(struct cling_lcse_ir const *lhs, struct cling_lcse_ir const *rhs)
{
  if (lhs->binary.temp1 == rhs->binary.temp2 &&
      lhs->binary.temp2 == rhs->binary.temp1)
    return 0;
  return 1;
}

static int communicative_compare(struct cling_lcse_ir const *lhs, struct cling_lcse_ir const *rhs)
{
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
static int interchangable_compare(struct cling_lcse_ir const *lhs, struct cling_lcse_ir const *rhs)
{
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
 * load_lvalue s equals when the variable is the same one, so they have the same address.
 */
static int load_lvalue_compare(struct cling_lcse_ir const *lhs, struct cling_lcse_ir const *rhs)
{
  if (lhs->opcode != rhs->opcode)
    return 1;
  if (0 == strcmp(lhs->load_lvalue.name, rhs->load_lvalue.name) &&
      lhs->load_lvalue.scope == rhs->load_lvalue.scope)
    return 0;
  return 1;
}

/*
 * value and name both need to be equal.
 */
static int load_rvalue_compare(struct cling_lcse_ir const *lhs, struct cling_lcse_ir const *rhs)
{
  if (lhs->opcode != rhs->opcode)
    return 1;
  if (0 == strcmp(lhs->load_rvalue.name, rhs->load_rvalue.name) &&
      lhs->load_rvalue.value == rhs->load_rvalue.value)
    return 0;
  return 1;
}

/*
 * store the same thing to the save location
 */
static int store_compare(struct cling_lcse_ir const *lhs, struct cling_lcse_ir const *rhs) {
  if (lhs->opcode != rhs->opcode)
    return 1;
  if (lhs->store.value == rhs->store.value && lhs->store.address == rhs->store.address)
    return 0;
  return 1;
}

static int lcse_ir_compare(struct cling_lcse_ir const *lhs, struct cling_lcse_ir const *rhs)
{
  switch(lhs->kind) {
    case LCSE_BINARY:
      switch(lhs->opcode) {
        case OP_ADD:
        case OP_MUL:
        case OP_EQ:
        case OP_NE:
          return communicative_compare(lhs, rhs);
        case OP_LT:
        case OP_LE:
        case OP_GT:
        case OP_GE:
          return reflective_compare(lhs, rhs);
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
  switch(self->opcode) {
    case LCSE_BINARY:
      return self->binary.temp1 + self->binary.temp2;
    case LCSE_LOAD_LVALUE:
      return self->load_lvalue.scope + self->load_lvalue.address + mysql_strhash(self->load_lvalue.name);
    case LCSE_LOAD_RVALUE:
      return self->load_rvalue.value + mysql_strhash(self->load_rvalue.name);
    case LCSE_STORE:
      return self->store.value + self->store.address;
    default:
      assert(false);
  }
}

static const struct utillib_hashmap_callback lcse_ir_callback={
  .hash_handler=lcse_ir_hash, .compare_handler=lcse_ir_compare,
};

/*
 * cling_lcse_value an address-value pair.
 */

static struct cling_lcse_value * lcse_value_create(unsigned int address, unsigned int value) {
  struct cling_lcse_value *self=malloc(sizeof *self);
  self->address=address;
  self->value=value;
  return self;
}

static void lcse_value_destroy(struct cling_lcse_value *self) { free(self); }

static int lcse_value_compare(struct cling_lcse_value const *lhs, struct cling_lcse_value const *rhs) {
  return lhs->address - rhs->address;
}

static size_t lcse_value_hash(struct cling_lcse_value const *self) {
  return self->address;
}

static const struct utillib_hashmap_callback lcse_value_intcallback={
  .hash_handler=lcse_value_hash, .compare_handler=lcse_value_compare,
};

/*
 * Lookup the address of this name. Address is just variable counted by var_count.
 */
static unsigned int lookup_named_address(struct cling_lcse_optimizer *self, char const *name) {
  struct cling_mips_label *label;
  label=mips_label_name_find(&self->names, name);
  if (label) {
    return label->address;
  }
  label=mips_label_create(name, self->var_count);
  utillib_hashmap_insert(&self->names, label, label);
  ++self->var_count;
  return label->address;
}

/*
 * Lookup the value at address.
 */
static unsigned int lookup_value(struct cling_lcse_optimizer *self, unsigned int address) {
  struct cling_lcse_value *val, v;
  v.address=address;
  val=utillib_hashmap_at(&self->values, &v);
  if (val) {
    return val->value;
  }
  val=lcse_value_create(address, self->var_count);
  utillib_hashmap_insert(&self->values, val, val);
  ++self->var_count;
  return val->value;
}

static unsigned int lookup_variable(struct cling_lcse_optimizer *self, unsigned int temp) {
  int var=self->variables[temp];
  if (var != -1)
    return var;
  var=self->var_count;
  self->variables[temp]=var;
  ++self->var_count;
  return var;
}

static void update_value(struct cling_lcse_optimizer *self, unsigned int address, unsigned int value) {
  struct cling_lcse_value v, *val;
  v.address=address;
  val=utillib_hashmap_at(&self->values, &v);
  assert(val);
  val->value=value;
}

/*
 * Prepare operands for a lcse_ir by looking the operands of ast_ir
 * for various maps.
 */
static void translate(struct cling_lcse_optimizer *self, 
    struct cling_ast_ir *ast_ir, struct cling_lcse_ir *lcse_ir) {
  char const *name;
  unsigned int address;
  unsigned int value;

  lcse_ir->opcode=ast_ir->opcode;
  switch(ast_ir->opcode) {
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
      lcse_ir->kind=LCSE_BINARY;
      lcse_ir->binary.temp1=lookup_variable(self, ast_ir->binop.temp1);
      lcse_ir->binary.temp2=lookup_variable(self, ast_ir->binop.temp2);
      break;
    case OP_LOAD:
      name=ast_ir->load.name;
      if (ast_ir->load.is_rvalue) {
        /*
         * rvalue is about value and its stealness.
         */
        lcse_ir->load_rvalue.name=name;
        address=lookup_named_address(self, name);
        lcse_ir->load_rvalue.value=lookup_value(self, address);
        lcse_ir->kind=LCSE_LOAD_RVALUE;
      } else {
        /*
         * lvalue is all about address.
         */
        lcse_ir->load_lvalue.name=name;
        lcse_ir->load_lvalue.scope=ast_ir->load.is_global;
        lcse_ir->kind=LCSE_LOAD_LVALUE;
      }
      break;
    case OP_STORE:
      address=lookup_variable(self, ast_ir->store.addr);
      value=lookup_variable(self, ast_ir->store.value);
      lcse_ir->store.address=address;
      lcse_ir->store.value=value;
      break;
    default:
      assert(false);
  }
}

static bool insert_operation(struct cling_lcse_optimizer *self, struct cling_ast_ir *ast_ir,
    struct cling_lcse_ir *lcse_ir) {
  struct cling_lcse_ir *new_ir;
  unsigned int result, value, address;

  new_ir=utillib_hashmap_at(&self->operations, lcse_ir);
  if (new_ir) {
    /*
     * Sorry, this operation has been done!
     * Fix up variable map and point t to existing one.
     */
    switch(new_ir->kind) {
      case LCSE_BINARY:
        self->variables[ast_ir->binop.result]=new_ir->binary.result;
        break;
      case LCSE_LOAD_LVALUE:
        self->variables[ast_ir->load.temp]=new_ir->load_lvalue.address;
        break;
      case LCSE_LOAD_RVALUE:
        self->variables[ast_ir->load.temp]=new_ir->load_rvalue.value;
        break;
      case LCSE_STORE:
        /*
         * store produces no new value so nothing need to be done.
         */
        break;
        return false;
    }
  }

  /*
   * New operation appears, new var must be allocated.
   */
  new_ir=malloc(sizeof *new_ir);
  memcpy(new_ir, lcse_ir, sizeof *new_ir);
  switch(new_ir->kind) {
    case LCSE_BINARY:
      result= ast_ir->binop.result;
      ast_ir->binop.result=lookup_variable(self, result);
      new_ir->binary.result=ast_ir->binop.result;
      break;
    case LCSE_STORE:
      update_value(self, new_ir->store.address, new_ir->store.value);
      break;
    case LCSE_LOAD_LVALUE:
      address=ast_ir->load.temp;
      ast_ir->load.temp=lookup_variable(self, address);
      new_ir->load_lvalue.address=ast_ir->load.temp;
      break;
    case LCSE_LOAD_RVALUE:
      value=ast_ir->load.temp;
      ast_ir->load.temp=lookup_variable(self, value);
      break;
    default:
      assert(false);
  }
  utillib_hashmap_insert(&self->operations, new_ir, new_ir);
  return true;
}

static void optimize(struct cling_lcse_optimizer *self, struct cling_basic_block const *block, 
    struct utillib_vector *instrs) {
  bool add_instr;
  struct cling_ast_ir *ast_ir;
  struct cling_lcse_ir lcse_ir;
  int value, index, array, temp, result;

  for (int i=block->begin; i<block->end; ++i) {
    add_instr=true;
    ast_ir=utillib_vector_at(block->instrs, i);
    self->address_map[i]=utillib_vector_size(instrs);
    switch(ast_ir->opcode) {
    case OP_LT:
    case OP_LE:
    case OP_GT:
    case OP_GE:
    case OP_EQ:
    case OP_NE:
      /*
       * relop.
       */
    case OP_ADD:
    case OP_MUL:
    case OP_DIV:
    case OP_SUB:
      /*
       * arithop
       */
    case OP_LOAD:
    case OP_STORE:
      /*
       * These are the subexpr we will handle.
       */
      translate(self, ast_ir, &lcse_ir);
      add_instr=insert_operation(self, ast_ir, &lcse_ir);
      break;
    case OP_PUSH:
      value=lookup_variable(self, ast_ir->push.temp);
      ast_ir->push.temp=value;
      break;
    case OP_RET:
      if (ast_ir->ret.has_result) {
        value=lookup_variable(self, ast_ir->ret.result);
        ast_ir->ret.result=value;
      }
      break;
    case OP_BEZ:
      value=lookup_variable(self, ast_ir->bez.temp);
      ast_ir->bez.temp=value;
      break;
    case OP_CAL:
      if (ast_ir->call.has_result) {
        value=lookup_variable(self, ast_ir->call.result);
        ast_ir->call.result=value;
      }
      break;
    case OP_IDX:
      /*
       * We always do it no matter lvalue or rvalue.
       */
      array=lookup_variable(self, ast_ir->index.array_addr);
      index=lookup_variable(self, ast_ir->index.index_result);
      result=lookup_variable(self, ast_ir->index.result);
      ast_ir->index.array_addr=array;
      ast_ir->index.index_result=index;
      ast_ir->index.result=result;
      break;
    case OP_RDCHR:
    case OP_RDINT:
      /*
       * read always introduce new variables.
       */
      ast_ir->read.temp=lookup_variable(self, ast_ir->read.temp);
      break;
    case OP_LDIMM:
      /*
       * We also do not track ldimm since that's a matter of
       * constant propergation. We assume every ldimm introduces new value.
       */
        ast_ir->ldimm.temp=lookup_variable(self, ast_ir->ldimm.temp);
        break;
    case OP_LDSTR:
        /*
         * We always use 0 to ldstr since string can only be printed
         * and never produces new value.
         */
        temp=ast_ir->ldstr.temp;
        self->variables[temp]=LCSE_TEMP_ZERO;
        ast_ir->ldstr.temp=LCSE_TEMP_ZERO;
        break;
    case OP_WRCHR:
    case OP_WRSTR:
    case OP_WRINT:
        /*
         * write is just a read-only operation on value.
         */
        ast_ir->write.temp=lookup_variable(self, ast_ir->write.temp);
      break;
    }
    if (add_instr)
      utillib_vector_push_back(instrs, ast_ir);
  }
}

static void lcse_ir_destroy(struct cling_lcse_ir *self) { free(self); }

void cling_lcse_optimizer_init(struct cling_lcse_optimizer *self, struct cling_ast_function const *ast_func)
{
  size_t temp_size=ast_func->temps;
  self->var_count=LCSE_TEMP_ZERO+1;
  self->variables=malloc(sizeof self->variables[0] * temp_size);
  memset(self->variables, -1, sizeof self->variables[0] * temp_size);
  utillib_hashmap_init(&self->operations, &lcse_ir_callback);
  utillib_hashmap_init(&self->names, &mips_label_strcallback);
  utillib_hashmap_init(&self->values, &lcse_value_intcallback);
}

void cling_lcse_optimizer_destroy(struct cling_lcse_optimizer *self) {
  free(self->variables);
  utillib_hashmap_destroy_owning(&self->operations, NULL, lcse_ir_destroy);
  utillib_hashmap_destroy_owning(&self->names, NULL, mips_label_destroy);
  utillib_hashmap_destroy_owning(&self->values, NULL, lcse_value_destroy);
}

void cling_lcse_optimizer_emit(struct cling_lcse_optimizer *self,
    struct cling_basic_block const *block, struct utillib_vector *instrs) {
  optimize(self, block, instrs);
}


