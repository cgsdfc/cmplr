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
 * Used as a place holder when the address or offset
 * of jump or branch instructions are unknown.
 */
#define MIPS_ADDR_NULL 0
#define MIPS_IMME_NULL 0
#define MIPS_WORD_SIZE sizeof (int)
#define MIPS_BYTE_SIZE sizeof (char)

/*
 * Double words align.
 */
#define MIPS_ALIGN (MIPS_WORD_SIZE << 1)

/*
 * Four MIPS_WORD s
 */
#define MIPS_MEM_ARG_BLK (MIPS_WORD_SIZE << 2)
#define MIPS_REGR_NULL 0

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
  UTILLIB_ETAB_ELEM_INIT(MIPS_ADD, "add")
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
static struct cling_mips_ir*
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
  self->operands[0].regid=src_regid1;
  self->operands[1].regid=src_regid2;
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

static struct cling_mips_ir *
mips_ir_copy(struct cling_mips_ir const *self) {
  struct cling_mips_ir *other=malloc(sizeof *other);
  memcpy(other, self, sizeof *other);
  return other;
}

/*
 * Print a mips ir to a static buffer.
 * The instructions are grouped by similar format.
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
  snprintf(buffer, MIPS_BUFSIZ, FORMAT, __VA_ARGS__);\
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
      mips_ir_snprintf(("%s %s, %" PRId16 "(%s)") , opstr, 
          regstr[0], self->operands[1].offset, regstr[2]);
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
      mips_ir_snprintf(("%s %" PRIu32) , opstr, self->operands[0].address);
      return buffer;
    case MIPS_ADDI:
      regstr[0]=mips_operand_regstr(self, 0);
      regstr[1]=mips_operand_regstr(self, 1);
      mips_ir_snprintf(("%s %s, %s, %" PRId16) , opstr, regstr[0], regstr[1], self->operands[2].imme32);
      return buffer;
    case MIPS_BNE:
      regstr[0]=mips_operand_regstr(self, 0);
      mips_ir_snprintf(("%s %s %" PRId16) , opstr, regstr[0], self->operands[1].offset);
      return buffer;
    default:
      puts(opstr);
      assert(false);
  }
}

/*
 * End of MIPS routines.
 */

static struct cling_mips_entity *
mips_entity_create(int kind, int regid, int offset) {
  struct cling_mips_entity * self=malloc(sizeof *self);
  self->kind=kind;
  self->state=regid == MIPS_REGR_NULL? MIPS_IN_MEM:MIPS_IN_REG;
  self->regid=regid;
  self->offset=offset;
  return self;
}

/*
 * Maps a name to an stack offset and a possible
 * register. A name may be allocated a register on start up.
 */
static struct cling_mips_entity *
mips_entity_make_name(char const *name, int regid) {
  struct cling_mips_entity *self=mips_entity_create(MIPS_NAME, regid, MIPS_ADDR_NULL);
  self->name=strdup(name);
  return self;
}

/*
 * Maps a temp to an stack offset.
 * Initially, temp is not given any register.
 */
static struct cling_mips_entity*
mips_entity_make_temp(int temp, int offset) {
  struct cling_mips_entity *self=mips_entity_create(MIPS_TEMP, MIPS_REGR_NULL, offset);
  self->temp=temp;
  return self;
}

/*
 * Maps an array.
 */
static struct cling_mips_entity *
mips_entity_make_array(char const *name) {
  struct cling_mips_entity *self=mips_entity_create(MIPS_ARRAY, MIPS_REGR_NULL, MIPS_ADDR_NULL);
  self->name=strdup(name);
  return self;
}

/*
 * We use PARA and ARG to differentiate the parameters passed to
 * **this** function and arguments this function passes to call others.
 */

/*
 * make_parais similar to make_name except that
 * the first-four arguments must own registers a0-a3
 * while the rest depends on allocation. What's more,
 * offset of arguments must be fixed up later because
 * they go into parent's frame.
 */
static struct cling_mips_entity *
mips_entity_make_para(char const *name, int regid, int offset) {
  struct cling_mips_entity *self=mips_entity_create(MIPS_PARA, regid, offset);
  self->name=strdup(name);
  return self;
}

/*
 * We might need a hashmap to speed up lookup.
 */
static int mips_entity_compare(struct cling_mips_entity const* lhs,
    struct cling_mips_entity const* rhs) {
  if (lhs->kind != rhs->kind)
    return 1;
  if (lhs->kind == MIPS_NAME || lhs->kind == MIPS_PARA)
    return strcmp(lhs->name, rhs->name);
  if (lhs->kind == MIPS_TEMP)
    return lhs->temp - rhs->temp;
  assert(false);
}

static size_t mips_entity_hash(struct cling_mips_entity const *self) {
  switch(self->kind) {
    case MIPS_PARA:
    case MIPS_NAME:
      return mysql_strhash(self->name);
    case MIPS_TEMP:
      return self->temp;
  }
  assert(false);
}

static void mips_entity_destroy(struct cling_mips_entity  *self) {
  if (self->kind==MIPS_NAME || self->kind == MIPS_PARA)
    free(self->name);
  free(self);
}

static const struct utillib_hashmap_callback mips_entity_callback={
  .compare_handler=mips_entity_compare,
  .hash_handler=mips_entity_hash,
};

static struct cling_mips_entity*
mips_function_find_name(struct cling_mips_function const* self, char const *name)
{
  struct cling_mips_entity entity;
  entity.kind=MIPS_NAME;
  entity.name=name;
  return utillib_hashmap_at(&self->memmap, &entity);
}

static struct cling_mips_entity*
mips_function_find_temp(struct cling_mips_function const *self, unsigned int temp)
{
  struct cling_mips_entity entity;
  entity.kind=MIPS_TEMP;
  entity.temp=temp;
  return utillib_hashmap_at(&self->memmap, &entity);
}

/*
 * Mark all the registers as free.
 */
static void mips_function_init(struct cling_mips_function *self, struct cling_ast_function const *ast_func)
{
  self->stack_offset=0;
  utillib_hashmap_init(&self->memmap, &mips_entity_callback);
  self->reg_pool=calloc(sizeof self->reg_pool[0], CLING_MIPS_REG_MAX);
  self->address_map=malloc(sizeof self->address_map[0] * utillib_vector_size(&ast_func->instrs));
}

static void mips_function_destroy(struct cling_mips_function *self)
{
  free(self->reg_pool);
  free(self->address_map);
  utillib_hashmap_destroy_owning(&self->memmap,
      mips_entity_destroy, free);
}

/*
 * Try to allocate a register from one of those free
 * which `regkind' says yes.
 * Fail with -1. 
 * Maybe some MEMOUT can be attempted.
 */
static int mips_function_regalloc(struct cling_mips_function *self,
    bool (*regkind) (uint8_t regid)) {
  for (int i=0; i<CLING_MIPS_REG_MAX; ++i)
    if (!self->reg_pool[i] && regkind(i)) {
      self->reg_pool[i]=true;
      return i;
    }
  return MIPS_REGR_NULL;
}

/*
 * Decides whether the function is a leaf.
 */
static bool mips_function_is_leaf(struct cling_ast_function const *ast_func)
{
  struct cling_ast_ir const *ast_ir;
  UTILLIB_VECTOR_FOREACH(ast_ir, &ast_func->instrs)
    if (ast_ir->opcode == OP_CAL)
      return false;
  return true;
}

static void mips_function_memmap(struct cling_mips_function *self,
    struct cling_mips_entity const *entity) {
  utillib_vector_push_back(&self->memmap, entity);
  utillib_hashmap_insert(&self->memindex, entity, entity);
}

static void mips_function_memmap_name(struct cling_mips_function *self,
    char const* name, uint32_t offset)
{


}

static void mips_function_memmap_temp(struct cling_mips_function *self,
    int temp, uint32_t offset)
{


}

static void mips_function_regmap_name(struct cling_mips_function *self,
    char const  *name, int regid)
{



}


static void mips_function_fix_address(struct cling_mips_function *self,
    struct utillib_vector const *ast_instrs,
    struct utillib_vector *mips_instrs) 
{
  struct cling_ast_ir const *ast_ir;
  struct cling_mips_ir *mips_ir;
  size_t ast_instrs_size=utillib_vector_size(ast_instrs);
  size_t mips_addr;
  size_t ast_addr;

  for (int i=0; i<ast_instrs_size; ++i) {
    ast_ir=utillib_vector_at(ast_instrs, i);
    switch (ast_ir->opcode) {
      case OP_JMP:
        /*
         * mips_j requires a absolute address so we make
         * use of global->instr_offset to get the current
         * offset of instructions in terms of function.
         * Since the ast_ir use absolute address inside
         * a function, adding a global offset of function
         * serves our purpose.
         */
        mips_addr=self->address_map[i];
        mips_ir=utillib_vector_at(mips_instrs, mips_addr);
        assert(mips_ir->opcode == MIPS_J);
        ast_addr=ast_ir->operands[0].scalar;
        mips_addr=self->address_map[ast_addr];
        mips_ir->operands[0].address=self->global->instr_offset+mips_addr;
        break;
      case OP_BEZ:
        /*
         * Branches requires a relative address of the **next** pc
         * of the current instruction (branch itself). For example,
         * beq $0, $0, -1 causes a dead loop since it always branches
         * to itself.
         * Here we simply make a substraction + 1.
         */
        mips_addr=self->address_map[i];
        mips_ir=utillib_vector_at(mips_instrs, mips_addr);
        assert(mips_ir->opcode == MIPS_BEZ);
        ast_addr=ast_ir->operands[1].scalar;
        mips_ir->operands[1].offset=mips_addr-self->address_map[ast_addr]+1;
        break;
      case OP_BNE:
        mips_addr=self->address_map[i];
        mips_ir=utillib_vector_at(mips_instrs, mips_addr);
        assert(mips_ir->opcode == MIPS_BNE);
        ast_addr=ast_ir->operands[2].scalar;
        mips_ir->operands[2].offset=mips_addr-self->address_map[ast_addr]+1;
        break;
    }
  }
}


/*
 * A typical prologue is,
 * addi $sp, $sp, -128
 * sw $ra, 20($sp)
 * sw $s0, 24($sp)
 * # save other saved registers.
 */
static void mips_function_prologue(
    struct cling_mips_function *self,
    struct utillib_vector * mips_instrs)
{
  struct cling_mips_ir *ir, *addi_sp;
  struct saved_register  *saved;

  addi_sp=mips_addi(MIPS_SP, MIPS_SP, -self->frame_size);
  utillib_vector_push_back(instrs, addi_sp);

  UTILLIB_VECTOR_FOREACH(saved, &self->saved_registers) {
    ir=mips_sw(saved->regid, saved->offset, MIPS_SP);
    utillib_vector_push_back(mips_instrs, ir);
  }
}

static void mips_function_epilogue(
    struct cling_mips_function *self,
    struct utillib_vector * mips_instrs)
{
  struct cling_mips_ir *ir, *addi_sp;
  struct saved_register *saved;

  UTILLIB_VECTOR_FOREACH(saved, &self->saved_registers) {
    ir=mips_lw(saved->regid, saved->offset, MIPS_SP);
    utillib_vector_push_back(mips_instrs, ir);
  }
  addi_sp=mips_addi(MIPS_SP, MIPS_SP, self->frame_size);
  utillib_vector_push_back(mips_instrs, addi_sp);
}

/*
 * Allocate a block of size on top of *blk
 * and increament *blk.
 */
static uint32_t mips_align_alloc(uint32_t *blk, size_t size) {
  uint32_t now=*blk;
  while(now & (MIPS_ALIGN-1)))
    ++now;
  *blk=now + size;
  return now;
}

static uint32_t mips_function_memalloc(struct cling_mips_function *self, size_t size)
{
  /*
   * Align to size.
   * if self->frame_size % size == 0, it is aligned.
   * We use fast-modulo so it becomes frame_size & (size-1) == 0.
   */
  return mips_align_alloc(&self->frame_size, size);
}

/*
 * Figures out the maximum stack space required for
 * any function called by self.
 */
static uint32_t mips_function_max_args(struct cling_ast_function const *ast_func)
{
  uint32_t args_blk=0;
  uint32_t max_args=0;
  struct cling_ast_ir const *ast_ir;
  int state=0;
  int arg_cnt=0;
  size_t arg_size;

  UTILLIB_VECTOR_FOREACH(ast_ir, &ast_func->instrs) {
    if (ast_ir->opcode == OP_PUSH) {
      if (state == 0)
        state = 1;
      if (arg_cnt++ < 4)
        arg_size=MIPS_WORD_SIZE;
        /*
         * First Four are always words.
         */
      else
        arg_size=getsize(ast_ir->operands[0].info);
      mips_align_alloc(&args_blk, arg_size);
    } else if (state == 1) {
      /*
       * Non push in state 1 mean the end of a push seq.
       * Now we reduce it to max_args.
       */
      state=0;
      if (args_blk > max_args)
        max_args=args_blk;
      args_blk=0;
    } 
    /*
     * Skip off the non-push instructions.
     */
  }
  return max_args;
}

static void mips_function_saved_push_back(
    struct cling_mips_function *self, uint8_t regid, uint32_t offset) {
  struct saved_register * saved=malloc(sizeof *saved);
  saved->regid=regid;
  saved->offset=offset;
  utillib_vector_push_back(&self->saved_registers, saved);
}

/*
 * local data region includes temps, array and single variable.
 * Allocates stack space for local data.
 */
static void mips_function_local_data(
    struct cling_mips_function *self,
    struct cling_ast_function *ast_func)
{
  uint32_t local_offset;
  char const *name;
  size_t size, base_size;
  struct cling_mips_entity *entity;
  /*
   * Maps all temps as words.
   */
  for (int i=0; i<ast_func->temps; ++i) {
    local_offset=mips_function_memalloc(self, MIPS_WORD_SIZE);
    mips_function_memmap_temp(self, i, local_offset);
  }

  UTILLIB_VECTOR_FOREACH(ast_ir, &ast_func->init_code) {
    switch(ast_ir->opcode) {
      case OP_DEFVAR:
        name=ast_ir->operands[0].text;
        size=getsize(ast_ir->operands[0].info);
        local_offset=mips_function_memalloc(self, size);
        mips_function_memmap_name(self, name, local_offset);
        break;
      case OP_DEFARR:
        name=ast_ir->operands[0].text;
        base_size=getsize(ast_ir->operands[0].info);
        sscanf(ast_ir->operands[1].text, "%lu", &size);
        local_offset=mips_function_memalloc(self, size * base_size);
        mips_function_memmap_name(self, name, local_offset);
        break;
    }
  }
}

/*
 * Allocate saved_register to 2 kinds of entities.
 * 1. PARA over four.
 * 2. SingleVariable or NAME.
 * Fills the saved_registers vector for prologue and epilogue.
 */
static void mips_function_save_registers(
    struct cling_mips_function *self,
    struct cling_ast_function const *ast_func) 
{
  struct cling_ast_ir const* ast_ir;
  uint32_t saved_offset;
  int regid;
  char const *name;
  int para_cnt=0;

  /*
   * Allocate saved registers
   * TODO: memmap the allocated regid to the name.
   */
  UTILLIB_VECTOR_FOREACH(ast_ir, &ast_func->init_code) {
loop:
    switch(ast_ir->opcode) {
      case OP_PARA:
        if (para_cnt++ < 4)
          continue;
        goto alloc_reg;
      case OP_DEFVAR:
        goto alloc_reg;
    } 
  }
alloc_reg:
  regid=mips_function_regalloc(self, mips_is_saved_register);
  if (regid == MIPS_REGR_NULL)
    return;
  name=ast_ir->operands[0].text;
  saved_offset=mips_function_memalloc(self, MIPS_WORD_SIZE);
  mips_function_saved_push_back(self, regid, saved_offset);
  mips_function_regmap_name(self, name, regid);
  goto loop;

}

/*
 * If this function is non leaf, layouts
 * its Argument Region, which must be the
 * first region from top of stack.
 * The space is reserved for future use of call.
 */
static void mips_function_args_layout(
    struct cling_mips_function *self,
    struct cling_ast_function const *ast_func)
{
  uint32_t max_args;
  uint32_t args_offset;
  
  max_args=mips_function_max_args(ast_func);
  args_offset=mips_function_memalloc(self, max_args);
  assert(args_offset == 0);
}

/*
 * Layout parameters which have offsets start
 * at sp+frame_size.
 */
static void mips_function_para_layout(
    struct cling_mips_function *self,
    struct cling_ast_function const *ast_func)
{
  int para_cnt=0;
  char const *name;
  size_t para_size;
  struct cling_ast_ir const *ast_ir;
  uint32_t para_offset, para_blk=0;

  UTILLIB_VECTOR_FOREACH(ast_ir, &ast_func->init_code) {
    if (ast_ir->opcode != OP_PARA)
      continue;
    name=ast_ir->operands[0].text;
    if (para_cnt++ < 4) {
      para_size=MIPS_WORD_SIZE;
    } else {
      para_size=getsize(ast_ir->operands[0].info);
    }
    para_offset=mips_align_alloc(&para_blk, para_size)+self->frame_size;
    mips_function_memmap_name(self, name, para_offset);
  }
}

/*
 * Driver that layouts all kinds of stuffs on the stack.
 */
static void mips_function_stack_layout(
    struct cling_mips_function *self,
    struct cling_ast_function const *ast_func)
{
  struct cling_ast_ir const *ast_ir;
  bool is_non_leaf=mips_function_is_non_leaf(ast_func);
  if (is_non_leaf) {
    mips_function_args_layout(self, ast_func);
  }
  mips_function_save_registers(self, ast_func);
  if (is_non_leaf) {
    /*
     * Save $ra.
     * Allocate a word for it and add it to the saved_registers.
     */
    offset=mips_function_memalloc(self, MIPS_WORD_SIZE);
    mips_function_saved_push_back(self, MIPS_RA, offset);
  }
  mips_function_local_data(self, ast_func);
  mips_function_para_layout(self, ast_func);
}

static void mips_function_instrs(
    struct cling_mips_function *self,
    struct cling_ast_function const* ast_func,
    struct utillib_vector *instrs)
{
  size_t ast_instrs_size;
  struct cling_ast_ir const *ast_ir;

  ast_instrs_size=utillib_vector_size(&ast_func->instrs);
  for (int ast_pc=0; ast_pc < ast_instrs_size; ++ast_pc) {
    ast_ir=utillib_vector_at(&ast_func->instrs);

  }



}

