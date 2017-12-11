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
#include "mips.h"
#include "ast_ir.h"
#include "ir.h"

#include <utillib/strhash.h>

#include <stdlib.h>
#include <assert.h>
#include <string.h>

/*
 * We define them as static instance because
 * they take no arguments.
 */
static const struct cling_mips_ir cling_mips_nop={ .opcode=MIPS_NOP };
static const struct cling_mips_ir cling_mips_syscall={ .opcode=MIPS_SYSCALL};

UTILLIB_ETAB_BEGIN(cling_mips_opcode_kind)
  UTILLIB_ETAB_ELEM_INIT(MIPS_SYSCALL, "syscall")
  UTILLIB_ETAB_ELEM_INIT(MIPS_MOVE, "move")
  UTILLIB_ETAB_ELEM_INIT(MIPS_ADDI, "addi")
  UTILLIB_ETAB_ELEM_INIT(MIPS_ADDU, "addu")
  UTILLIB_ETAB_ELEM_INIT(MIPS_SUB, "sub")
  UTILLIB_ETAB_ELEM_INIT(MIPS_SUBU, "subu")
  UTILLIB_ETAB_ELEM_INIT(MIPS_JAL, "jal")
  UTILLIB_ETAB_ELEM_INIT(MIPS_J, "j")
  UTILLIB_ETAB_ELEM_INIT(MIPS_JR, "jr")
  UTILLIB_ETAB_ELEM_INIT(MIPS_BNE, "bne")
  UTILLIB_ETAB_ELEM_INIT(MIPS_BEZ, "bez")
  UTILLIB_ETAB_ELEM_INIT(MIPS_MFLO, "mflo")
  UTILLIB_ETAB_ELEM_INIT(MIPS_LW, "lw")
  UTILLIB_ETAB_ELEM_INIT(MIPS_DIV, "div")
  UTILLIB_ETAB_ELEM_INIT(MIPS_MULT, "mult")
  UTILLIB_ETAB_ELEM_INIT(MIPS_NOP, "nop")
  UTILLIB_ETAB_ELEM_INIT(MIPS_SW, "sw")
  UTILLIB_ETAB_ELEM_INIT(MIPS_LB, "lb")
  UTILLIB_ETAB_ELEM_INIT(MIPS_SB, "sb")
  UTILLIB_ETAB_ELEM_INIT(MIPS_LA, "la")
  UTILLIB_ETAB_ELEM_INIT(MIPS_LI, "li")
UTILLIB_ETAB_END(cling_mips_opcode_kind);

UTILLIB_ETAB_BEGIN(cling_mips_regster)
  UTILLIB_ETAB_ELEM_INIT(MIPS_ZERO, "$zero")
  UTILLIB_ETAB_ELEM_INIT(MIPS_AT, "$at")
  UTILLIB_ETAB_ELEM_INIT(MIPS_V0, "$v0")
  UTILLIB_ETAB_ELEM_INIT(MIPS_V1, "$v1")
  UTILLIB_ETAB_ELEM_INIT(MIPS_A0, "$a0")
  UTILLIB_ETAB_ELEM_INIT(MIPS_A1, "$a1")
  UTILLIB_ETAB_ELEM_INIT(MIPS_A2, "$a2")
  UTILLIB_ETAB_ELEM_INIT(MIPS_A3, "$a3")
  UTILLIB_ETAB_ELEM_INIT(MIPS_T0, "$t0")
  UTILLIB_ETAB_ELEM_INIT(MIPS_T1, "$t1")
  UTILLIB_ETAB_ELEM_INIT(MIPS_T2, "$t2")
  UTILLIB_ETAB_ELEM_INIT(MIPS_T3, "$t3")
  UTILLIB_ETAB_ELEM_INIT(MIPS_T4, "$t4")
  UTILLIB_ETAB_ELEM_INIT(MIPS_T5, "$t5")
  UTILLIB_ETAB_ELEM_INIT(MIPS_T6, "$t6")
  UTILLIB_ETAB_ELEM_INIT(MIPS_T7, "$t7")
  UTILLIB_ETAB_ELEM_INIT(MIPS_S0, "$s0")
  UTILLIB_ETAB_ELEM_INIT(MIPS_S1, "$s1")
  UTILLIB_ETAB_ELEM_INIT(MIPS_S2, "$s2")
  UTILLIB_ETAB_ELEM_INIT(MIPS_S3, "$s3")
  UTILLIB_ETAB_ELEM_INIT(MIPS_S4, "$s4")
  UTILLIB_ETAB_ELEM_INIT(MIPS_S5, "$s5")
  UTILLIB_ETAB_ELEM_INIT(MIPS_S6, "$s6")
  UTILLIB_ETAB_ELEM_INIT(MIPS_S7, "$s7")
  UTILLIB_ETAB_ELEM_INIT(MIPS_T8, "$t8")
  UTILLIB_ETAB_ELEM_INIT(MIPS_T9, "$t9")
  UTILLIB_ETAB_ELEM_INIT(MIPS_K0, "$k0")
  UTILLIB_ETAB_ELEM_INIT(MIPS_K1, "$k1")
  UTILLIB_ETAB_ELEM_INIT(MIPS_GP, "$gp")
  UTILLIB_ETAB_ELEM_INIT(MIPS_SP, "$sp")
  UTILLIB_ETAB_ELEM_INIT(MIPS_RA, "$ra")
UTILLIB_ETAB_END(cling_mips_regster);

/*
 * Get the size of object.
 */
static size_t getsize(int info) {
  if (info & CL_WORD)
    return sizeof (int);
  if (info & CL_BYTE)
    return sizeof (char);
  assert(false);
}

/*
 * Temp register, caller saved.
 */
static bool mips_is_temp_register(uint8_t regid)
{
  return (MIPS_T0 <= regid && regid <= MIPS_T7) ||
    MIPS_T8 == regid || MIPS_T9 == regid;
}

/*
 * Argument register, caller saved.
 */
static bool mips_is_arg_register(uint8_t regid)
{
  return MIPS_A0 <= regid && regid <= MIPS_A3;
}

/*
 * Saved register, global register, callee saved.
 */
static bool mips_is_saved_register(uint8_t regid)
{
  return MIPS_S0 <= regid && regid <= MIPS_S7;
}


/*
 * One to one mapping from mips instructions
 * to C routines.
 */
static struct cling_mips_ir * mips_ir_create(uint8_t opcode) {
  struct cling_mips_ir *self=calloc(sizeof self[0], 1);
  self->opcode=opcode;
  return self;
};

/*
 * Jal/J/Jr family.
 */
/*
 * jal <label>
 * Jump and link. Jump to the label
 * and store next pc to $ra.
 * Notes it overwrites the original content of $ra.
 */
static struct cling_mips_ir *
mips_jal(char const* label) {
  struct cling_mips_ir *self=mips_ir_create(MIPS_JAL);
  self->operands[0].label=strdup(label);
}

/*
 * jr $ra.
 * Jump to address held at $ra. Usually used to return
 * from subroutine call.
 */
static struct cling_mips_ir *
mips_jr(uint8_t target_regid) {
  struct cling_mips_ir *self=mips_ir_create(MIPS_JR);
  self->operands[0].regid=target_regid;
  return self;
}

/*
 * j <address>
 * Jump to an absolute address unconditionally.
 */
static struct cling_mips_ir *
mips_j(uint32_t address) {
  struct cling_mips_ir *self=mips_ir_create(MIPS_J);
  self->operands[0].address=address;
  return self;
}

/*
 * Load/Store family.
 * Address uses a base address in a register plus offset mode.
 */
static struct cling_mips_ir *
mips_load_store(int opcode, uint8_t dest_regid, int16_t offset, uint8_t base_regid) {
  struct cling_mips_ir *self=mips_ir_create(opcode);
  self->operands[0].regid=dest_regid;
  self->operands[1].offset=offset;
  self->operands[2].regid=base_regid;
  return self;
}

/*
 * lw $s1, -2($sp)
 */
static struct cling_mips_ir *
mips_lw(uint8_t dest_regid, int16_t offset, uint8_t base_regid) {
  return mips_load_store(MIPS_LW, dest_regid, offset, base_regid);
}

static struct cling_mips_ir *
mips_lb(uint8_t dest_regid, int16_t offset, uint8_t base_regid) {
  return mips_load_store(MIPS_LB, dest_regid, offset, base_regid);
}

/*
 * sw $s2, 0($sp)
 */
static struct cling_mips_ir *
mips_sw(uint8_t src_regid, int16_t offset, uint8_t base_regid) {
  return mips_load_store(MIPS_SW, src_regid, offset, base_regid);
}

/*
 * sb $s2, -4($s1)
 */
static struct cling_mips_ir *
mips_sb(uint8_t src_regid, int16_t offset, uint8_t base_regid) {
  return mips_load_store(MIPS_SB, src_regid, offset, base_regid);
}

/*
 * Arithmetic/Calculative with immediate.
 */
static struct cling_mips_ir *
mips_immecalc(uint8_t opcode,
    uint8_t dest_regid,
    uint8_t src_regid,
    int16_t imme)
{
  struct cling_mips_ir *self=mips_ir_create(opcode);
  self->operands[0].regid=dest_regid;
  self->operands[1].regid=src_regid;
  self->operands[2].imme16=imme;
  return self;
}

/*
 * addi $s0, $s1, imme
 * $s0 = imme + $s1 **with** overflow.
 */
static struct cling_mips_ir *
mips_addi(uint8_t dest_regid, uint8_t src_regid, int16_t imme)
{
  return mips_immecalc(MIPS_ADDI, dest_regid, src_regid, imme);
}

/*
 * Calculative using three registers.
 */
static struct cling_mips_ir *
mips_regcalc(uint8_t opcode,
    uint8_t dest_regid,
    uint8_t src_regid1,
    uint8_t src_regid2)
{
  struct cling_mips_ir *self=mips_ir_create(opcode);
  self->operands[0].regid=dest_regid;
  self->operands[1].regid=src_regid1;
  self->operands[2].regid=src_regid2;
  return self;
}

/*
 * subu $s0, $s1, $s2
 */
static struct cling_mips_ir *
mips_subu(uint8_t dest_regid, uint8_t src_regid1, uint8_t src_regid2)
{
  return mips_regcalc(MIPS_SUBU, dest_regid, src_regid1, src_regid2);
}

/*
 * sub $s0, $s1, $s2
 */
static struct cling_mips_ir *
mips_sub(uint8_t dest_regid, uint8_t src_regid1, uint8_t src_regid2)
{
  return mips_regcalc(MIPS_SUB, dest_regid, src_regid1, src_regid2);
}

static struct cling_mips_ir *
mips_addu(uint8_t dest_regid, uint8_t src_regid1, uint8_t src_regid2)
{
  return mips_regcalc(MIPS_ADDU, dest_regid, src_regid1, src_regid2);
}

static struct cling_mips_ir *
mips_add(uint8_t dest_regid, uint8_t src_regid1, uint8_t src_regid2)
{
  return mips_regcalc(MIPS_ADD, dest_regid, src_regid1, src_regid2);
}

/*
 * Misc and Pseudo instructions.
 */
/*
 * la $s0, <label>
 * load address onto a register.
 * Used to access global variables and asciiz.
 */
static struct cling_mips_ir *
mips_la(uint8_t dest_regid, char const *label) {
  struct cling_mips_ir *self=mips_ir_create(MIPS_LA);
  self->operands[0].regid=dest_regid;
  self->operands[1].label=strdup(label);
}

/*
 * li $s0, imme32
 * load a 32 bit imme into $s0.
 */
static struct cling_mips_ir *
mips_li(uint8_t dest_regid, int32_t imme32) {
  struct cling_mips_ir *self=mips_ir_create(MIPS_LI);
  self->operands[0].regid=dest_regid;
  self->operands[1].imme32=imme32;
}

/*
 * move $s1, $s2
 * Copy content of $s2 to $s1.
 */
static struct cling_mips_ir *
mips_move(uint8_t dest_regid, uint8_t src_regid) {
  struct cling_mips_ir *self=mips_ir_create(MIPS_MOVE);
  self->operands[0].regid=dest_regid;
  self->operands[1].regid=src_regid;
  return self;
}

static struct cling_mips_ir *
mips_onereg(uint8_t opcode, uint8_t regid) {
  struct cling_mips_ir *self=mips_ir_create(opcode);
  self->operands[0].regid=regid;
  return self;
}

/*
 * mflo $s0
 * Moves $lo to $s0.
 */
static struct cling_ast_ir *
mips_mflo(uint8_t dest_regid) {
  return mips_onereg(MIPS_MFLO, dest_regid);
}

/*
 * Conditional branch family
 */
/*
 * bez $s0, <offset>
 * branch to pc+offset if $s0 equals to zero.
 */
static struct cling_mips_ir *
mips_bez(uint8_t regid, int16_t offset) {
  struct cling_mips_ir *self=mips_ir_create(MIPS_BNE);
  self->operands[0].regid=regid;
  self->operands[1].offset=offset;
  return self;
}

/*
 * bne $s0, $s1, <offset>
 * branch to pc+offset if $s0 does not equal to $s1.
 */
static struct cling_mips_ir *
mips_bne(uint8_t src_regid1, uint8_t src_regid2,
    int16_t offset) {
  struct cling_mips_ir *self=mips_ir_create(MIPS_BNE);
  self->operands[0].regid=src_regid1;
  self->operands[1].regid=src_regid2;
  self->operands[2].offset=offset;
  return self;
}

/*
 * Arithmetic that uses $lo and $hi.
 */
static struct cling_mips_ir *
mips_tworegs(int opcode,
    uint8_t src_regid1, uint8_t src_regid2) {
  struct cling_mips_ir *self=mips_ir_create(opcode);
  self->operands[0]=src_regid1;
  self->operands[1]=src_regid2;
  return self;
}

/*
 * div $s0, $s1
 * divide $s0 by $s1 and store quotient in $lo
 * and remainder in $hi.
 */
static struct cling_mips_ir *
mips_div(uint8_t src_regid1, uint8_t src_regid2) {
  return mips_tworegs(MIPS_DIV, src_regid1, src_regid2);
}

/*
 * mult $s0, $s1
 * multiply $s0 by $s1 and store the product in $lo.
 */
static struct cling_mips_ir *
mips_mult(uint8_t src_regid1, uint8_t src_regid2) {
  return mips_tworegs(MIPS_MULT, src_regid1, src_regid2);
}

static void 
mips_ir_destroy(struct cling_mips_ir *self) {
  switch(self->opcode) {
    case MIPS_NOP:
    case MIPS_SYSCALL:
      /* 
       * static instance.
       */
      return;
    case MIPS_JAL:
    case MIPS_LA:
      free(self->operands[0].label);
      break;
  }
  free(self);
}

/*
 * Print a mips ir to a static buffer.
 */
static char const *
mips_ir_tostring(struct cling_mips_ir const* self) {
/*
 * Size of the static buffer.
 */
#define MIPS_BUFSIZ 128      
/*
 * Short cut for snprintf.
 * Notes it uses buffer implicitly.
 */
#define mips_ir_snprintf(FORMAT, ...) do {\
  snprintf(buffer, CLING_MIPS_BUFSIZ, FORMAT, __VA_ARGS__);\
}while(0)

/*
 * Turns a regid field of self into a regstr.
 * Notes it does not check for the regid.
 */
#define mips_operand_regstr(self, index) cling_mips_regster_tostring(self->operands[0].regid)

  static char buffer[MIPS_BUFSIZ];
  char const *opstr=cling_mips_opcode_kind_tostring(self->opcode);
  char const *regstr[CLING_MIPS_OPERAND_MAX];
  switch(self->opcode) {
    case MIPS_NOP:
    case MIPS_SYSCALL:
      return opstr;
    case MIPS_SW:
    case MIPS_LW:
    case MIPS_SB:
    case MIPS_LB:
      regstr[0]=mips_operand_regstr(self, 0);
      regstr[2]=mips_operand_regstr(self, 2);
      mips_ir_snprintf("%s %s, " PRId16 "(%s)", opstr, self->operands[1].offset,
          regstr[0], regstr[2]);
      return buffer;
    case MIPS_ADD:
    case MIPS_ADDU:
    case MIPS_SUB:
    case MIPS_SUBU:
      regstr[0]=mips_operand_regstr(self, 0);
      regstr[1]=mips_operand_regstr(self, 1);
      regstr[2]=mips_operand_regstr(self, 2);
      mips_ir_snprintf("%s %s, %s, %s", opstr, regstr[0], regstr[1], regstr[2]);
      return buffer;
    case MIPS_MOVE:
    case MIPS_DIV:
    case MIPS_MULT:
      regstr[0]=mips_operand_regstr(self, 0);
      regstr[1]=mips_operand_regstr(self, 1);
      mips_ir_snprintf("%s %s, %s", opstr, regstr[0], regstr[1]);
      return buffer;
    case MIPS_LA:
    case MIPS_JAL:
      mips_ir_snprintf("%s %s", opstr, self->operands[0].label);
      return buffer;
    case MIPS_MFLO:
    case MIPS_JR:
    case MIPS_BEZ:
      regstr[0]=mips_operand_regstr(self, 0);
      mips_ir_snprintf("%s %s", opstr, regstr[0]);
      return buffer;
    case MIPS_J:
      mips_ir_snprintf("%s " PRIu32, opstr, self->operands[0].address);
      return buffer;
    case MIPS_ADDI:
      regstr[0]=mips_operand_regstr(self, 0);
      regstr[1]=mips_operand_regstr(self, 1);
      mips_ir_snprintf("%s %s, %s, " PRId16, opstr, regstr[0], regstr[1]);
      return buffer;
    case MIPS_BNE:
      regstr[0]=mips_operand_regstr(self, 0);
      mips_ir_snprintf("%s %s " PRId16, opstr, regstr[0]);
  }
}

/*
 * End of MIPS routines.
 */

/*
 * cling_mips_memmap
 * Map each entity to an address on the stack or one of the registers.
 */
static struct cling_mips_entity * mips_entity_create(int kind) {
  struct cling_mips_entity * self=malloc(sizeof *self);
  self->kind=MIPS_NAME;
  return self;
}

static struct cling_mips_entity * mips_entity_make_name(char const *name) {
  struct cling_mips_entity *self=mips_entity_create(MIPS_NAME);
  self->name=strdup(name);
  return self;
}

static struct cling_mips_entity* mips_entity_make_temp(uint8_t temp) {
  struct cling_mips_entity *self=mips_entity_create(MIPS_TEMP);
  self->temp=temp;
  return self;
}

static int mips_entity_compare(struct cling_mips_entity const* lhs,
    struct cling_mips_entity const* rhs) {
  if (lhs->kind != rhs->kind)
    return 1;
  if (lhs->kind == MIPS_NAME)
    return strcmp(lhs->name, rhs->name);
  if (lhs->kind == MIPS_TEMP)
    return lhs->temp - rhs->temp;
  assert(false);
}

static size_t mips_entity_hash(struct cling_mips_entity const *self) {
  switch(self->kind) {
    case MIPS_NAME:
      return mysql_strhash(self->name);
    case MIPS_TEMP:
      return self->temp;
  }
  assert(false);
}

static void mips_entity_destroy(struct cling_mips_entity  *self) {
  if (self->kind==MIPS_NAME)
    free(self->name);
  free(self);
}

static const struct utillib_hashmap_callback mips_entity_callback={
  .compare_handler=mips_entity_compare,
  .hash_handler=mips_entity_hash,
};

static struct cling_mips_state *
mips_memmap_find_name(struct cling_mips_memmap const* self, char const *name)
{
  struct cling_mips_entity entity;
  entity.kind=MIPS_NAME;
  entity.name=name;
  return utillib_hashmap_at(&self->memmap, &entity);
}

static struct cling_mips_state *
mips_memmap_find_temp(struct cling_mips_memmap const *self, unsigned int temp)
{
  struct cling_mips_entity entity;
  entity.kind=MIPS_TEMP;
  entity.temp=temp;
  return utillib_hashmap_at(&self->memmap, &entity);
}

/*
 * Mark all the registers as free.
 */
static void mips_memmap_init(struct cling_mips_memmap *self)
{
  self->stack_offset=0;
  utillib_hashmap_init(&self->memmap, &mips_entity_callback);
  self->reg_pool=calloc(sizeof self->reg_pool[0], CLING_MIPS_REG_MAX);
}

static void mips_memmap_destroy(struct cling_mips_memmap *self)
{
  free(self->reg_pool);
  utillib_hashmap_destroy_owning(&self->memmap,
      mips_entity_destroy, free);
}

/*
 * Try to allocate a register from one of those free
 * which `regkind' says yes.
 * Fail with -1. 
 * Maybe some MEMOUT can be attempted.
 */
static int mips_memmap_regalloc(struct cling_mips_memmap *self,
    bool (*regkind) (uint8_t regid)) {
  for (int i=0; i<CLING_MIPS_REG_MAX; ++i)
    if (!self->reg_pool[i] && regkind(i)) {
      self->reg_pool[i]=true;
      return i;
    }
  return -1;
}

/*
 * A cling_mips_state can have both regid
 * and offset valid or invalid determined
 * by the state field.
 */
static struct cling_mips_state * 
cling_mips_state_create(int state, uint8_t regid, int16_t offset) {
  struct cling_mips_state *self=malloc(sizeof *self);
  self->state=state;
  self->regid=regid;
  self->offset=offset;
  return self;
}

/*
 * Insert a non-array variable into memmap.
 * Allocate a stack slot for it and attempt a saved-register.
 */
static void mips_memmap_insert_single_var(struct cling_mips_memmap *self,
    struct cling_ast_operand const *operand)
{
  size_t size;
  struct cling_mips_entity *entity;
  struct cling_mips_state *state;
  int maybe_regid;

  size=getsize(operand->info);
  entity=mips_entity_make_name(operand->text);
  maybe_regid=mips_memmap_regalloc(self, mips_is_saved_register);
  state=cling_mips_state_create(maybe_regid==-1?MIPS_IN_MEM:MIPS_IN_REG, 
      maybe_regid, self->stack_offset);
  utillib_hashmap_insert(&self->memmap, entity, state);
  self->stack_offset+=size;
}

/*
 * Insert a formal argument into memmap.
 * Following the first-four via a0-a3 and
 * others on stack rule.
 * This function handles the first-four args.
 */
static void mips_memmap_insert_arg(struct cling_mips_memmap *self,
    int arg_regid,
    struct cling_ast_operand const* operand)
{
  assert(mips_is_arg_register(arg_regid));
  assert(self->reg_pool[arg_regid]==false);

  struct cling_mips_entity *entity;
  struct cling_mips_state *state;

  entity=mips_entity_make_name(operand->text);
  state=cling_mips_state_create(MIPS_IN_REG, arg_regid, -1);
  utillib_hashmap_insert(&self->memmap, entity, state);
  self->reg_pool[arg_regid]=true;
}

/*
 * Insert an array into memmap.
 * Array resides entirely on the stack.
 */
static void mips_memmap_insert_array(struct cling_mips_memmap *self,
  struct cling_ast_operand const *name,
  struct cling_ast_operand const *extend)
{
  assert(name->info & CL_NAME);
  assert(extend->info & CL_IMME);

  unsigned int length;
  scanf(extend->text, "%u", &length);
  utillib_hashmap_insert(&self->memmap,
      mips_entity_make_name(name->text),
      cling_mips_state_create(MIPS_IN_MEM, -1, self->stack_offset));
  self->stack_offset+=length * getsize(name->info);
}

static void mips_memmap_emit_init(struct cling_mips_memmap *self,
    struct utillib_vector const *init_code)
{
  /*
   * Make sure that all the named variables have places 
   * on the stack, which is the minimum requirement for
   * memory.
   */
  struct cling_ast_ir const *ir;
  int para_cnt=0;

  UTILLIB_VECTOR_FOREACH(ir, init_code) {
    switch(ir->opcode) {
      case OP_DEFVAR:
        mips_memmap_insert_single_var(self, &ir->operands[0]);
        break;
      case OP_PARA:
        if (++para_cnt >= CLING_MIPS_PARA_IN_REG)
          mips_memmap_insert_single_var(self, &ir->operands[0]);
        else
          mips_memmap_insert_arg(self, MIPS_A0+para_cnt, &ir->operands[0]);
        break;
      case OP_DEFARR:
          mips_memmap_insert_array(self, &ir->operands[0], &ir->operands[1]); 
        break;
    }
  }
  self->max_stack=self->stack_offset;
}

static void mips_emit_call(
    struct cling_ast_ir const* ir,
    struct cling_mips_global *global,
    struct cling_mips_memmap *memmap,
    struct utillib_vector *instrs)
{




}

static void mips_emit_ret(
    struct cling_ast_ir const* ir,
    struct cling_mips_global *global, 
    struct cling_mips_memmap *memmap,
    struct utillib_vector *instrs)
{
  struct cling_mips_ir *jr, *maybe_lw; 
  int info;
  struct cling_mips_state *state;

  info=ir->operands[0].info;
  jr=mips_jr(MIPS_RA);
  if (info == CL_NULL) {
    /*
     * No return value.
     */
    utillib_vector_push_back(instrs, jr);
    return;
  }
  state=mips_memamp_find_entity(memmap, &ir->operands[0]);
  switch(state->state) {
    case MIPS_IN_MEM:
      /*
       * This entity is in memory.
       * We load it into v0.
       */
      if (info & CL_WORD) {
        maybe_lw=mips_lw(MIPS_V0, state->offset, MIPS_SP);
      } else {
        maybe_lw=mips_lb(MIPS_V0, state->offset, MIPS_SP);
      }
      break;
    case MIPS_IN_REG:
      /*
       * In register, move it to v0.
       */
      maybe_lw=mips_move(MIPS_V0, state->regid);
      break;
  }
  utillib_vector_push_back(instrs, maybe_lw);
  utillib_vector_push_back(instrs, jr);
}

/*
 * Linearly scan the list and find as much
 * matches of ast_ir_addr as possible to
 * fix their jump target in mips and then
 * remove them from the list.
 */
static void mips_global_fix_jump_target(
    struct cling_mips_global *global,
    int mips_ir_addr,
    int ast_ir_addr)
{
  struct cling_mips_target *current, *previous;
  struct cling_mips_ir *mips_ir;
  int index;

  current=global->target_head;
  previous=current;
  if (!current)
    return;
  
  while (current) {
    if (current->ast_ir_addr == ast_ir_addr) {
      mips_ir=current->mips_ir;
      switch(mips_ir->opcode){
        case MIPS_J:
          index=0;
          break;
        case MIPS_BEZ:
          index=1;
          break;
        case MIPS_BNE:
          index=2;
          break;
      }
      mips_ir->operands[index].target=mips_ir_addr;
      previous->next=current->next;
      current=previous->next;
      free(current);
    }

    previous=current;
    current=current->next;
  }



}

static void mips_emit_jmp(
    struct cling_ast_ir const* ir,
    struct cling_mips_global *global, 
    struct cling_mips_memmap *memmap,
    struct utillib_vector *instrs)
{
  struct cling_mips_ir *mips_jump=mips_ir_create(MIPS_J);
  utillib_vector_push_back(instrs, mips_jump); 
}

static void mips_emit_function(
    struct cling_mips_global *global,
    struct cling_mips_memmap *memmap,
    struct cling_ast_function const* function,
    struct utillib_vector *instrs)
{
  struct cling_ast_ir const *ir;
  int ast_ir_addr=0;
  int mips_ir_addr=0;

  mips_memmap_emit_init(memmap, &function->init_code); 
  UTILLIB_VECTOR_FOREACH(ir, &function->instrs) {
    switch(ir->opcode) {
      case OP_RET:
        mips_emit_ret(ir, global, memmap, instrs);
        break;
      case OP_JMP:
        mips_emit_jmp(ir, global, memmap, instrs);
        break;
      case OP_ADD:
      case OP_SUB:
      case OP_MUL:
      case OP_DIV:
      case OP_LT:
      case OP_LE:
      case OP_GT:
      case OP_GE:
      case OP_EQ:
      case OP_NE:
      case OP_BNE:
      case OP_BEZ:
      case OP_PUSH:


      
    }
    ++ast_ir_addr;
    mips_global_fix_jump_target(global, utillib_vector_size(instrs), ast_ir_addr);
    
  }



}

static void mips_write(struct cling_ast_ir const* ir,
  struct cling_mips_global *global,  struct utillib_vector *instrs)
{



}


static void mips_localize_temps(struct cling_ast_function const* function)
{




}

