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
#include "misc.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void mips_mock(void);

/*
 * Used as a place holder when the address or offset
 * of jump or branch instructions are unknown.
 */
#define MIPS_ADDR_NULL 0
#define MIPS_IMME_NULL 0
#define MIPS_REGR_NULL 0

#define MIPS_WORD_SIZE sizeof(int)
#define MIPS_BYTE_SIZE sizeof(char)

/*
 * Double words align.
 */
#define MIPS_ALIGN (MIPS_WORD_SIZE << 1)

/*
 * Four MIPS_WORD s
 */
#define MIPS_MEM_ARG_BLK (MIPS_WORD_SIZE << 2)
#define MIPS_REG_ARGS_N 4

/*
 * We define them as static instance because
 * they take no arguments.
 */
static const struct cling_mips_ir cling_mips_nop = {.opcode = MIPS_NOP};
static const struct cling_mips_ir cling_mips_syscall = {.opcode = MIPS_SYSCALL};

/*
 * opcode and register strings.
 */
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
UTILLIB_ETAB_ELEM_INIT(MIPS_BEQ, "beq")
UTILLIB_ETAB_ELEM_INIT(MIPS_BGEZ, "bgez")
UTILLIB_ETAB_ELEM_INIT(MIPS_BGTZ, "bgtz")
UTILLIB_ETAB_ELEM_INIT(MIPS_BLEZ, "blez")
UTILLIB_ETAB_ELEM_INIT(MIPS_BLTZ, "bltz")
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

UTILLIB_ETAB_BEGIN(cling_mips_data_kind)
UTILLIB_ETAB_ELEM_INIT(MIPS_ASCIIZ, ".asciiz")
UTILLIB_ETAB_ELEM_INIT(MIPS_SPACE, ".space")
UTILLIB_ETAB_ELEM_INIT(MIPS_WORD, ".word")
UTILLIB_ETAB_ELEM_INIT(MIPS_BYTE, ".byte")
UTILLIB_ETAB_END(cling_mips_data_kind);

/*
 * Get the size of object.
 */
static size_t getsize(int info) {
  if (info & CL_WORD)
    return MIPS_WORD_SIZE;
  if (info & CL_BYTE)
    return MIPS_BYTE_SIZE;
  assert(false);
}

/*
 * Temp register, caller saved.
 */
static bool mips_is_temp_register(uint8_t regid) {
  return (MIPS_T0 <= regid && regid <= MIPS_T7) || MIPS_T8 == regid ||
         MIPS_T9 == regid;
}

/*
 * Argument register, caller saved.
 */
static bool mips_is_arg_register(uint8_t regid) {
  return MIPS_A0 <= regid && regid <= MIPS_A3;
}

/*
 * Saved register, global register, callee saved.
 */
static bool mips_is_saved_register(uint8_t regid) {
  return MIPS_S0 <= regid && regid <= MIPS_S7;
}

/*
 * One to one mapping from mips instructions
 * to C routines.
 */
static struct cling_mips_ir *mips_ir_create(uint8_t opcode) {
  struct cling_mips_ir *self = calloc(sizeof *self, 1);
  self->opcode = opcode;
  return self;
}

/*
 * Jal/J/Jr family.
 */
/*
 * jal <label>
 * Jump and link. Jump to the label
 * and store next pc to $ra.
 * Notes it overwrites the original content of $ra.
 */
static struct cling_mips_ir *mips_jal(char const *label) {
  struct cling_mips_ir *self = mips_ir_create(MIPS_JAL);
  self->operands[0].label = strdup(label);
}

/*
 * jr <register>
 * Jump to address held at <register>. 
 * Usually used to return from subroutine call.
 */
static struct cling_mips_ir *mips_jr(uint8_t target_regid) {
  struct cling_mips_ir *self = mips_ir_create(MIPS_JR);
  self->operands[0].regid = target_regid;
  return self;
}

/*
 * j <address>
 * Jump to an absolute address unconditionally.
 */
static struct cling_mips_ir *mips_j(uint32_t address) {
  struct cling_mips_ir *self = mips_ir_create(MIPS_J);
  self->operands[0].address = address;
  return self;
}

/*
 * Load/Store family.
 * Address uses a base address in a register plus offset mode.
 */
static struct cling_mips_ir *mips_load_store(int opcode, uint8_t dest_regid,
                                             int16_t offset,
                                             uint8_t base_regid) {
  struct cling_mips_ir *self = mips_ir_create(opcode);
  self->operands[0].regid = dest_regid;
  self->operands[1].offset = offset;
  self->operands[2].regid = base_regid;
  return self;
}

/*
 * lw $s1, -2($sp)
 */
static struct cling_mips_ir *mips_lw(uint8_t dest_regid, int16_t offset,
                                     uint8_t base_regid) {
  return mips_load_store(MIPS_LW, dest_regid, offset, base_regid);
}

static struct cling_mips_ir *mips_lb(uint8_t dest_regid, int16_t offset,
                                     uint8_t base_regid) {
  return mips_load_store(MIPS_LB, dest_regid, offset, base_regid);
}

/*
 * sw $s2, 0($sp)
 */
static struct cling_mips_ir *mips_sw(uint8_t src_regid, int16_t offset,
                                     uint8_t base_regid) {
  return mips_load_store(MIPS_SW, src_regid, offset, base_regid);
}

/*
 * sb $s2, -4($s1)
 */
static struct cling_mips_ir *mips_sb(uint8_t src_regid, int16_t offset,
                                     uint8_t base_regid) {
  return mips_load_store(MIPS_SB, src_regid, offset, base_regid);
}

/*
 * Arithmetic/Calculative with immediate.
 */
static struct cling_mips_ir *mips_immecalc(uint8_t opcode, uint8_t dest_regid,
                                           uint8_t src_regid, int16_t imme) {
  struct cling_mips_ir *self = mips_ir_create(opcode);
  self->operands[0].regid = dest_regid;
  self->operands[1].regid = src_regid;
  self->operands[2].imme16 = imme;
  return self;
}

/*
 * addi $s0, $s1, imme
 * $s0 = imme + $s1 **with** overflow.
 */
static struct cling_mips_ir *mips_addi(uint8_t dest_regid, uint8_t src_regid,
                                       int16_t imme) {
  return mips_immecalc(MIPS_ADDI, dest_regid, src_regid, imme);
}

/*
 * Calculative using three registers.
 */
static struct cling_mips_ir *mips_regcalc(uint8_t opcode, uint8_t dest_regid,
                                          uint8_t src_regid1,
                                          uint8_t src_regid2) {
  struct cling_mips_ir *self = mips_ir_create(opcode);
  self->operands[0].regid = dest_regid;
  self->operands[1].regid = src_regid1;
  self->operands[2].regid = src_regid2;
  return self;
}

/*
 * subu $s0, $s1, $s2
 */
static struct cling_mips_ir *mips_subu(uint8_t dest_regid, uint8_t src_regid1,
                                       uint8_t src_regid2) {
  return mips_regcalc(MIPS_SUBU, dest_regid, src_regid1, src_regid2);
}

/*
 * sub $s0, $s1, $s2
 */
static struct cling_mips_ir *mips_sub(uint8_t dest_regid, uint8_t src_regid1,
                                      uint8_t src_regid2) {
  return mips_regcalc(MIPS_SUB, dest_regid, src_regid1, src_regid2);
}

static struct cling_mips_ir *mips_addu(uint8_t dest_regid, uint8_t src_regid1,
                                       uint8_t src_regid2) {
  return mips_regcalc(MIPS_ADDU, dest_regid, src_regid1, src_regid2);
}

static struct cling_mips_ir *mips_add(uint8_t dest_regid, uint8_t src_regid1,
                                      uint8_t src_regid2) {
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
static struct cling_mips_ir *mips_la(uint8_t dest_regid, char const *label) {
  struct cling_mips_ir *self = mips_ir_create(MIPS_LA);
  self->operands[0].regid = dest_regid;
  self->operands[1].label = strdup(label);
}

/*
 * li $s0, imme32
 * load a 32 bit imme into $s0.
 */
static struct cling_mips_ir *mips_li(uint8_t dest_regid, int32_t imme32) {
  struct cling_mips_ir *self = mips_ir_create(MIPS_LI);
  self->operands[0].regid = dest_regid;
  self->operands[1].imme32 = imme32;
}

/*
 * move $s1, $s2
 * Copy content of $s2 to $s1.
 */
static struct cling_mips_ir *mips_move(uint8_t dest_regid, uint8_t src_regid) {
  struct cling_mips_ir *self = mips_ir_create(MIPS_MOVE);
  self->operands[0].regid = dest_regid;
  self->operands[1].regid = src_regid;
  return self;
}

/*
 * mflo $s0
 * Moves $lo to $s0.
 */
static struct cling_mips_ir *mips_mflo(uint8_t dest_regid) {
  struct cling_mips_ir *self = mips_ir_create(MIPS_MFLO);
  self->operands[0].regid = dest_regid;
  return self;
}

/*
 * Conditional branch family
 */
static struct cling_mips_ir *mips_branch(uint8_t opcode,
    uint8_t src_regid1, uint8_t src_regid2, int16_t offset) {
  struct cling_mips_ir *self = mips_ir_create(opcode);
  self->operands[0].regid = src_regid1;
  self->operands[1].regid = src_regid2;
  self->operands[2].offset = offset;
  return self;
}

static struct cling_mips_ir *mips_beq(uint8_t src_regid1, uint8_t src_regid2, int16_t offset) {
  return mips_branch(MIPS_BEQ, src_regid1, src_regid2, offset);
}

/*
 * bez $s0, <offset>
 * branch to pc+offset if $s0 equals to zero.
 */
static struct cling_mips_ir *mips_bez(uint8_t regid, int16_t offset) {
  return mips_beq(MIPS_ZERO, regid, offset);
}

/*
 * bne $s0, $s1, <offset>
 * branch to pc+offset if $s0 does not equal to $s1.
 */
static struct cling_mips_ir *mips_bne(uint8_t src_regid1, uint8_t src_regid2,
                                      int16_t offset) {
  return mips_branch(MIPS_BNE, src_regid1, src_regid2, offset);
}

/*
 * Arithmetic that uses $lo and $hi.
 */
static struct cling_mips_ir *mips_tworegs(int opcode, uint8_t src_regid1,
                                          uint8_t src_regid2) {
  struct cling_mips_ir *self = mips_ir_create(opcode);
  self->operands[0].regid = src_regid1;
  self->operands[1].regid = src_regid2;
  return self;
}

/*
 * div $s0, $s1
 * divide $s0 by $s1 and store quotient in $lo
 * and remainder in $hi.
 */
static struct cling_mips_ir *mips_div(uint8_t src_regid1, uint8_t src_regid2) {
  return mips_tworegs(MIPS_DIV, src_regid1, src_regid2);
}

/*
 * mult $s0, $s1
 * multiply $s0 by $s1 and store the product in $lo.
 */
static struct cling_mips_ir *mips_mult(uint8_t src_regid1, uint8_t src_regid2) {
  return mips_tworegs(MIPS_MULT, src_regid1, src_regid2);
}

static void mips_ir_destroy(struct cling_mips_ir *self) {
  switch (self->opcode) {
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
 * The instructions are grouped by similar format.
 */
static char const *mips_ir_tostring(struct cling_mips_ir const *self) {
/*
 * Size of the static buffer.
 */
#define MIPS_BUFSIZ 128
/*
 * Short cut for snprintf.
 * Notes it uses buffer implicitly.
 */
#define mips_ir_snprintf(FORMAT, ...)                                          \
  do {                                                                         \
    snprintf(buffer, MIPS_BUFSIZ, FORMAT, __VA_ARGS__);                        \
  } while (0)

/*
 * Turns a regid field of self into a regstr.
 * Notes it does not check for the regid.
 */
#define mips_operand_regstr(self, index)                                       \
  cling_mips_regster_tostring(self->operands[index].regid)

  static char buffer[MIPS_BUFSIZ];
  char const *opstr = cling_mips_opcode_kind_tostring(self->opcode);
  char const *regstr[CLING_MIPS_OPERAND_MAX];
  switch (self->opcode) {
  case MIPS_NOP:
  case MIPS_SYSCALL:
    return opstr;
  case MIPS_SW:
  case MIPS_LW:
  case MIPS_SB:
  case MIPS_LB:
    regstr[0] = mips_operand_regstr(self, 0);
    regstr[2] = mips_operand_regstr(self, 2);
    mips_ir_snprintf(("%s %s, %" PRId16 "(%s)"), opstr, regstr[0],
                     self->operands[1].offset, regstr[2]);
    return buffer;
  case MIPS_ADD:
  case MIPS_ADDU:
  case MIPS_SUB:
  case MIPS_SUBU:
    regstr[0] = mips_operand_regstr(self, 0);
    regstr[1] = mips_operand_regstr(self, 1);
    regstr[2] = mips_operand_regstr(self, 2);
    mips_ir_snprintf("%s %s, %s, %s", opstr, regstr[0], regstr[1], regstr[2]);
    return buffer;
  case MIPS_MOVE:
  case MIPS_DIV:
  case MIPS_MULT:
    regstr[0] = mips_operand_regstr(self, 0);
    regstr[1] = mips_operand_regstr(self, 1);
    mips_ir_snprintf("%s %s, %s", opstr, regstr[0], regstr[1]);
    return buffer;
  case MIPS_LA:
    regstr[0]=mips_operand_regstr(self, 0);
    mips_ir_snprintf("%s %s, %s", opstr, regstr[0], self->operands[1].label);
    return buffer;
  case MIPS_JAL:
    mips_ir_snprintf("%s %s", opstr, self->operands[0].label);
    return buffer;
  case MIPS_MFLO:
  case MIPS_JR:
    regstr[0] = mips_operand_regstr(self, 0);
    mips_ir_snprintf("%s %s", opstr, regstr[0]);
    return buffer;
  case MIPS_J:
    mips_ir_snprintf(("%s %" PRIu32), opstr, self->operands[0].address);
    return buffer;
  case MIPS_ADDI:
    regstr[0] = mips_operand_regstr(self, 0);
    regstr[1] = mips_operand_regstr(self, 1);
    mips_ir_snprintf(("%s %s, %s, %" PRId16), opstr, regstr[0], regstr[1],
                     self->operands[2].imme16);
    return buffer;
  case MIPS_BNE:
    regstr[0] = mips_operand_regstr(self, 0);
    mips_ir_snprintf(("%s %s %" PRId16), opstr, regstr[0],
                     self->operands[1].offset);
    return buffer;
  case MIPS_LI:
    regstr[0]=mips_operand_regstr(self, 0);
    mips_ir_snprintf(("%s %s, %" PRId32), opstr, regstr[0], self->operands[1].imme32);
    return buffer;
  default:
    cling_default_assert(self->opcode, cling_mips_opcode_kind_tostring);
  }
}

/*
 * End of MIPS routines.
 */

/*
 * We use PARA and ARG to differentiate the parameters passed to
 * **this** function and arguments this function passes to call others.
 */

static struct cling_mips_name *mips_name_create(int state, uint8_t regid, uint32_t offset) {
  struct cling_mips_name *self=malloc(sizeof *self);
  self->state=state;
  self->regid=regid;
  self->offset=offset;
  return self;
}

static void mips_function_init(struct cling_mips_function *self,
                               struct utillib_vector *instrs,
                               struct cling_mips_global *global,
                               struct cling_ast_function const *ast_func) {
  self->global=global;
  self->instrs = instrs;
  self->temp_size = ast_func->temps;
  self->frame_size = 0;
  self->reg_pool = calloc(sizeof self->reg_pool[0], CLING_MIPS_REG_MAX);
  self->address_map = malloc(sizeof self->address_map[0] *
                             utillib_vector_size(&ast_func->instrs));
  self->temps = malloc(sizeof self->temps[0] * ast_func->temps);
  utillib_hashmap_init(&self->names, &cling_string_hash);
  utillib_vector_init(&self->saved_registers);
}

static void mips_function_destroy(struct cling_mips_function *self) {
  free(self->reg_pool);
  free(self->address_map);
  free(self->temps);
  utillib_vector_destroy_owning(&self->saved_registers, free);
  utillib_hashmap_destroy_owning(&self->names, free, free);
}

/*
 * Try to allocate a register from one of those free
 * which `regkind' says yes.
 */
static int mips_function_regalloc(struct cling_mips_function *self,
                                  bool (*regkind)(uint8_t regid)) {
  for (int i = 0; i < CLING_MIPS_REG_MAX; ++i)
    if (!self->reg_pool[i] && regkind(i)) {
      self->reg_pool[i] = true;
      return i;
    }
  return MIPS_REGR_NULL;
}

/*
 * Decides whether the function is a leaf.
 */
static bool mips_function_is_leaf(struct cling_ast_function const *ast_func) {
  struct cling_ast_ir const *ast_ir;
  UTILLIB_VECTOR_FOREACH(ast_ir, &ast_func->instrs)
  if (ast_ir->opcode == OP_CAL)
    /*
     * Call others.
     */
    return false;
  return true;
}

static void mips_function_memmap_name(struct cling_mips_function *self,
                                      char const *name, uint32_t offset) {
  struct cling_mips_name *entry;
  entry = utillib_hashmap_at(&self->names, name);
  if (entry) {
    /*
     * Since we do regmap earlier than doing memmap
     * there maybe some entries been regmapped.
     */
    assert(entry->state == MIPS_IN_REG);
    entry->offset = offset;
    return;
  }
  entry = mips_name_create(MIPS_IN_MEM, MIPS_REGR_NULL, offset);
  utillib_hashmap_insert(&self->names, strdup(name), entry);
}

static void mips_function_memmap_temp(struct cling_mips_function *self,
                                      int temp, uint32_t offset) {
  assert(temp < self->temp_size);
  struct cling_mips_temp *entry;
  entry = &self->temps[temp];
  entry->offset = offset;
  entry->regid = MIPS_REGR_NULL;
}

/*
 * Maps this name to a saved register.
 */
static void mips_function_regmap_name(struct cling_mips_function *self,
                                      char const *name, int regid) {
  struct cling_mips_name *entry;
  entry = utillib_hashmap_at(&self->names, name);
  if (entry) {
    printf("name %s was regmapped to %d, twice!\n", name, regid);
    assert(false);
  }
  entry = mips_name_create(MIPS_IN_REG, regid, MIPS_ADDR_NULL);
  utillib_hashmap_insert(&self->names, strdup(name), entry);
}

static void mips_function_fix_address(struct cling_mips_function *self,
                                      struct utillib_vector const *ast_instrs,
                                      struct utillib_vector *mips_instrs) {
  struct cling_ast_ir const *ast_ir;
  struct cling_mips_ir *mips_ir;
  size_t ast_instrs_size = utillib_vector_size(ast_instrs);
  size_t mips_addr;
  size_t ast_addr;

  for (int i = 0; i < ast_instrs_size; ++i) {
    ast_ir = utillib_vector_at(ast_instrs, i);
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
      mips_addr = self->address_map[i];
      mips_ir = utillib_vector_at(mips_instrs, mips_addr);
      assert(mips_ir->opcode == MIPS_J);
      ast_addr = ast_ir->operands[0].scalar;
      mips_addr = self->address_map[ast_addr];
      mips_ir->operands[0].address = self->global->instr_offset + mips_addr;
      break;
    case OP_BEZ:
      /*
       * Branches requires a relative address of the **next** pc
       * of the current instruction (branch itself). For example,
       * beq $0, $0, -1 causes a dead loop since it always branches
       * to itself.
       * Here we simply make a substraction + 1.
       */
      mips_addr = self->address_map[i];
      mips_ir = utillib_vector_at(mips_instrs, mips_addr);
      ast_addr = ast_ir->operands[1].scalar;
      mips_ir->operands[1].offset = mips_addr - self->address_map[ast_addr] + 1;
      break;
    case OP_BNE:
      mips_addr = self->address_map[i];
      mips_ir = utillib_vector_at(mips_instrs, mips_addr);
      assert(mips_ir->opcode == MIPS_BNE);
      ast_addr = ast_ir->operands[2].scalar;
      mips_ir->operands[2].offset = mips_addr - self->address_map[ast_addr] + 1;
      break;
    }
  }
}

static void mips_function_push_back(struct cling_mips_function *self,
                                    struct cling_mips_ir const *ir) {
  utillib_vector_push_back(self->instrs, ir);
}

static void mips_function_saved_push_back(struct cling_mips_function *self,
                                          uint8_t regid, uint32_t offset) {
  struct saved_register *saved = malloc(sizeof *saved);
  saved->regid = regid;
  saved->offset = offset;
  utillib_vector_push_back(&self->saved_registers, saved);
}

/*
 * A typical prologue is,
 * addi $sp, $sp, -128
 * sw $ra, 20($sp)
 * sw $s0, 24($sp)
 * # save other saved registers.
 * `saved_registers' holds all the global registers.
 */
static void mips_function_prologue(struct cling_mips_function *self) {
  struct cling_mips_ir *ir, *addi_sp;
  struct saved_register const *saved;

  addi_sp = mips_addi(MIPS_SP, MIPS_SP, -self->frame_size);
  mips_function_push_back(self, addi_sp);

  UTILLIB_VECTOR_FOREACH(saved, &self->saved_registers) {
    ir = mips_sw(saved->regid, saved->offset, MIPS_SP);
    mips_function_push_back(self, ir);
  }
}

/*
 * A typical epilogue is,
 * lw $s0, 20($sp)
 * lw $ra, 24($sp)
 * addi $sp, $sp, 128
 * Notes that the final jr is emitted by this
 * function, not the mips_emit_ret which may be
 * omitted because the original ast_func deos not
 * have a ret.
 */
static void mips_function_epilogue(struct cling_mips_function *self) {
  struct cling_mips_ir *ir, *addi_sp;
  struct saved_register const *saved;

  UTILLIB_VECTOR_FOREACH(saved, &self->saved_registers) {
    ir = mips_lw(saved->regid, saved->offset, MIPS_SP);
    mips_function_push_back(self, ir);
  }
  addi_sp = mips_addi(MIPS_SP, MIPS_SP, self->frame_size);
  mips_function_push_back(self, addi_sp);
  mips_function_push_back(self, mips_jr(MIPS_RA));
}

/*
 * Similar to prologue
 * and epilogue but it is for a subroutine call.
 * They save/load the temp registers in use and a0s.
 * The para_size controls the total number of registers 
 * we saved.
 */
static void mips_load_temps_paras(struct cling_mips_function *self, bool is_load)
{
  struct cling_mips_temp *temp_one;
  for (int i=0; i<self->temp_size; ++i) {
    temp_one=&self->temps[i];
    if (temp_one->regid != MIPS_REGR_NULL) {
      mips_function_push_back(self, is_load? 
          mips_lw(temp_one->regid, temp_one->offset, MIPS_SP):
          mips_sw(temp_one->regid, temp_one->offset, MIPS_SP));
    }
  }
  for (int i=0; i<MIPS_REG_ARGS_N && i<self->para_size; ++i) {
    mips_function_push_back(self, is_load?
        mips_lw(MIPS_A0+i, self->frame_size+(i<<2), MIPS_SP):
        mips_sw(MIPS_A0+i, self->frame_size+(i<<2), MIPS_SP));
  }
}

/*
 * Allocate a block of size on top of *blk
 * and increament *blk. Memory address is aligned
 * to `align'.
 */
static uint32_t mips_align_alloc(uint32_t *blk, size_t size, size_t align) {
  /*
   * Align to size.
   * if self->frame_size % size == 0, it is aligned.
   * We use fast-modulo so it becomes frame_size & (size-1) == 0.
   */
  uint32_t now = *blk;
  while (now & (align- 1))
    ++now;
  *blk = now + size;
  return now;
}

static uint32_t mips_function_memalloc(struct cling_mips_function *self,
                                       size_t size, size_t align) {
  return mips_align_alloc(&self->frame_size, size, align);
}

/*
 * Stack Layout code.
 */

/*
 * Figures out the maximum stack space required for
 * any function called by self.
 */
static uint32_t
mips_function_max_args(struct cling_ast_function const *ast_func) {
  struct cling_ast_ir const *ast_ir;
  size_t arg_size;
  uint32_t max_args = 0;

  /*
   * Stateful 
   */
  uint32_t args_blk = 0;
  int state = 0;
  int arg_cnt = 0;

  UTILLIB_VECTOR_FOREACH(ast_ir, &ast_func->instrs) {
    if (ast_ir->opcode == OP_PUSH) {
      if (state == 0)
        state = 1;
      if (arg_cnt++ < MIPS_REG_ARGS_N)
        arg_size = MIPS_WORD_SIZE;
      /*
       * First Four are always words.
       */
      else
        arg_size = getsize(ast_ir->operands[0].info);
      mips_align_alloc(&args_blk, arg_size, arg_size);
    } else if (state == 1) {
      /*
       * The end of a push seq.
       * Now we reduce it to max_args
       * and clear stateful stuffs.
       */
      if (args_blk > max_args)
        max_args = args_blk;
      args_blk = 0;
      state = 0;
      arg_cnt = 0;
    }
    /*
     * Skip off the non-push instructions.
     */
  }
  return max_args;
}

/*
 * local data region includes temps, array and single variable.
 * Allocates stack space for local data.
 */
static void
mips_function_local_layout(struct cling_mips_function *self,
                         struct cling_ast_function const *ast_func) {
  uint32_t local_offset;
  char const *name;
  size_t size, base_size;
  struct cling_ast_ir const *ast_ir;
  /*
   * Maps all temps as words.
   */
  for (int i = 0; i < ast_func->temps; ++i) {
    local_offset = mips_function_memalloc(self, MIPS_WORD_SIZE, MIPS_WORD_SIZE);
    mips_function_memmap_temp(self, i, local_offset);
  }

  UTILLIB_VECTOR_FOREACH(ast_ir, &ast_func->init_code) {
    switch (ast_ir->opcode) {
    case OP_DEFVAR:
      name = ast_ir->operands[0].text;
      size = getsize(ast_ir->operands[0].info);
      local_offset = mips_function_memalloc(self, size, size);
      mips_function_memmap_name(self, name, local_offset);
      break;
    case OP_DEFARR:
      name = ast_ir->operands[0].text;
      base_size = getsize(ast_ir->operands[0].info);
      size = ast_ir->operands[0].imme_int;
      /*
       * Align to the base_size.
       */
      local_offset = mips_function_memalloc(self, size * base_size, base_size);
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
static void
mips_function_save_registers(struct cling_mips_function *self,
                             struct cling_ast_function const *ast_func) {
  struct cling_ast_ir const *ast_ir;
  uint32_t saved_offset;
  int regid;
  char const *name;
  int para_cnt = 0;

  /*
   * Allocate saved registers
   * TODO: memmap the allocated regid to the name.
   */
  UTILLIB_VECTOR_FOREACH(ast_ir, &ast_func->init_code) {
    if (ast_ir->opcode != OP_PARA && ast_ir->opcode != OP_DEFVAR)
      continue;
    if (ast_ir->opcode == OP_PARA && para_cnt++ < MIPS_REG_ARGS_N)
      continue;
    regid = mips_function_regalloc(self, mips_is_saved_register);
    if (regid == MIPS_REGR_NULL)
      return;
    name = ast_ir->operands[0].text;
    saved_offset = mips_function_memalloc(self, MIPS_WORD_SIZE, MIPS_WORD_SIZE);
    mips_function_saved_push_back(self, regid, saved_offset);
    mips_function_regmap_name(self, name, regid);
  }
}

/*
 * If this function is non leaf, layouts
 * its Argument Region, which must be the
 * first region on the top of stack.
 * The space is reserved for future use of call.
 */
static void
mips_function_args_layout(struct cling_mips_function *self,
                          struct cling_ast_function const *ast_func) {
  uint32_t max_args;
  uint32_t args_offset;

  max_args = mips_function_max_args(ast_func);
  args_offset = mips_function_memalloc(self, max_args, MIPS_WORD_SIZE);
  assert(args_offset == 0);
}

/*
 * Layout parameters which have offsets start
 * at sp+frame_size.
 */
static void
mips_function_para_layout(struct cling_mips_function *self,
                          struct cling_ast_function const *ast_func) {
  char const *name;
  struct cling_ast_ir const *ast_ir;
  size_t para_size;
  uint32_t para_offset;
  int para_cnt = 0;
  uint32_t para_blk = 0;

  UTILLIB_VECTOR_FOREACH(ast_ir, &ast_func->init_code) {
    if (ast_ir->opcode != OP_PARA)
      continue;
    name = ast_ir->operands[0].text;
    if (para_cnt < MIPS_REG_ARGS_N) {
      para_size = MIPS_WORD_SIZE;
    } else {
      para_size = getsize(ast_ir->operands[0].info);
    }
    para_offset = mips_align_alloc(&para_blk, para_size, para_size) + self->frame_size;
    mips_function_memmap_name(self, name, para_offset);
    ++para_cnt;
  }
  /*
   * Control how much parameters we will save
   * before subroutine call.
   */
  self->para_size = para_cnt;
}

/*
 * Driver that layouts all kinds of stuffs on the stack.
 */
static void
mips_function_stack_layout(struct cling_mips_function *self,
                           struct cling_ast_function const *ast_func) {
  struct cling_ast_ir const *ast_ir;
  uint32_t offset;
  bool is_leaf;

  is_leaf = mips_function_is_leaf(ast_func);
  if (!is_leaf) {
    /*
     * Reserve maximum space for call in the future.
     */
    mips_function_args_layout(self, ast_func);
  }
  /*
   * Allocate global registers and put them in saved_registers.
   */
  mips_function_save_registers(self, ast_func);
  if (!is_leaf) {
    /*
     * Save $ra.
     * Allocate a word for it and add it to the saved_registers.
     */
    offset = mips_function_memalloc(self, MIPS_WORD_SIZE, MIPS_WORD_SIZE);
    mips_function_saved_push_back(self, MIPS_RA, offset);
  }
  mips_function_local_layout(self, ast_func);
  mips_function_para_layout(self, ast_func);
}

/*
 * Temp Register Management
 *
 * We use LRU algorithm to allocate temps.
 * all the temps has one cling_mips_temp entry which records
 * its name, its age and the register it owns. Each time a register
 * is in need to hold a temp, the reg_pool will be searched for an vacant
 * one. If it succeeds, the temp entry will be set up to have age zero
 * (youngest)
 * and own that register. At the end of each emission of an ast_ir, the temps
 * that
 * own a register will have their age increased (they own that by show their
 * regid
 * not equal to MIPS_REGR_NULL, note) and when the reg_pool goes drained, the
 * oldest
 * temp will be spilled out to memory (its regid now equals to MIPS_REGR_NULL).
 * Everytime
 * a temp is referred to, it gets younger (not younger than zero).
 *
 * Virtual Register
 *
 * We use a mapping from virtual register (also called temps) to actual register
 * to bridge the differences between Saved-Register and Temp-Register. In one word,
 * Saved-Register is just Temp-Register that cannot be stripped from a virtual register.
 * The virtual register simplifies all the operands of the ast_ir to temps while all the
 * burden of mapping them to real register or memory laid on the load-store ast_ir, the 
 * stack-layout function and the register-allocator. 
 * In the first place the allocator tries to allocate saved-registers statically to local 
 * variable and their virtual registers. Those who failed to own a sr will have to compete
 * for tr with temporaries. The stack-layout knows how to distinguish the real temporaries
 * (that hold result for expressions) from those represent local variable having sr or not.
 * Then, when a virtual register is looked up, those for local variables having sr always
 * load into their sr while those haven't are in similar situation with temporaries, the story
 * for which is all about LRU.
 *
 */

static void mips_function_regfree(struct cling_mips_function *self,
                                  uint8_t regid) {
  self->reg_pool[regid] = false;
}

/*
 * Allocate a temp register by possibly spilling out temp variable.
 */
static uint8_t mips_function_temp_alloc(struct cling_mips_function *self) {
  uint8_t regid;
  int oldest_age = 0;
  struct cling_mips_temp *oldest_var;

  regid = mips_function_regalloc(self, mips_is_temp_register);
  if (regid != MIPS_REGR_NULL)
    return regid;
  for (int i = 0; i < self->temp_size; ++i) {
    if (self->temps[i].regid != MIPS_REGR_NULL &&
        self->temps[i].age > oldest_age) {
      oldest_age = self->temps[i].age;
      oldest_var = &self->temps[i];
    }
  }

  regid = oldest_var->regid;
  /*
   * Spill it out.
   */
  mips_function_push_back(self, mips_sw(regid, oldest_var->offset, MIPS_SP));
  oldest_var->regid = MIPS_REGR_NULL;
  return regid;
}

/*
 * Unmap a temp. Strip its register, which is free now.
 * Some short-lived temp can free up register once it was finished.
 * However, their value will be discarded.
 */
static void mips_function_temp_unmap(struct cling_mips_function *self, uint8_t temp) {
  struct cling_mips_temp *temp_var;
  temp_var=&self->temps[temp];
  if (temp_var->regid == MIPS_REGR_NULL)
    return;
  mips_function_regfree(self, temp_var->regid);
  temp_var->regid=MIPS_REGR_NULL;
}

/*
 * Ages all the temp that is alive.
 */
static void mips_function_temp_ages(struct cling_mips_function *self) {
  for (int i = 0; i < self->temp_size; ++i) {
    if (self->temps[i].regid != MIPS_REGR_NULL)
      ++self->temps[i].age;
  }
}

/*
 * Refers to a living temp to make it younger.
 */
static void mips_function_temp_refer(struct cling_mips_function *self,
                                     uint16_t temp) {
  assert(temp < self->temp_size);
  struct cling_mips_temp *temp_one;
  temp_one = &self->temps[temp];
  /*
   * Only can we refer to live temp.
   */
  assert(temp_one->regid != MIPS_REGR_NULL);
  if (temp_one->age)
    --temp_one->age;
}

static uint8_t mips_function_temp_map(struct cling_mips_function *self, 
    uint16_t temp, bool is_read)
{
  struct cling_mips_temp *temp_var;
  uint8_t regid;

  temp_var=&self->temps[temp];
  if (temp_var->regid != MIPS_REGR_NULL) {
    mips_function_temp_refer(self, temp);
    return temp_var->regid;
  }

  regid=mips_function_temp_alloc(self);
  temp_var->regid=regid;
  temp_var->age=0;
  if (!is_read) {
    return regid;
  }
  mips_function_push_back(self, mips_lw(regid, temp_var->offset, MIPS_SP));
  return regid;
}

/*
 * The following 2 functions make sure that the read and write
 * to temp variables are translated to their temp registers correctly.
 * They take a cling_ast_operand and return regid.
 */

/*
 * For reading, if the tempv is in_reg, then its value must be the newest.
 * However, if it is in_mem, its last content is in the stack so we allocate
 * a register and load back its content.
 */
#define mips_check_temp(ast_ir, index) do {\
  int info=ast_ir->operands[index].info; \
  assert(info & CL_TEMP); \
} while (0)

static uint8_t mips_function_read(struct cling_mips_function *self,
    struct cling_ast_ir const *ast_ir, int index)
{
  mips_check_temp(ast_ir, index);
  return mips_function_temp_map(self, ast_ir->operands[index].scalar, true);
}

/*
 * For writing, the content will be overwritten anyway, so we just return its
 * register or allocate a register for it. 
 */
static uint8_t mips_function_write(struct cling_mips_function *self,
    struct cling_ast_ir const *ast_ir, int index)
{
  mips_check_temp(ast_ir, index);
  return mips_function_temp_map(self, ast_ir->operands[index].scalar, false);
}

/*
 * End of temp management.
 */

/*
 * Translation of different ast_ir.
 */

/*
 * Three operands calculation.
 */
static void mips_emit_ternary_calc(struct cling_mips_function *self,
                          struct cling_ast_ir const *ast_ir) {
  /*
   * We use register to hold operands in any cases.
   * So that we do not bother whether to use addi.
   */
  uint8_t dest_regid;
  uint8_t src_regid1, src_regid2;
  struct cling_mips_ir *ir;

  src_regid1 = mips_function_read(self, ast_ir, 1);
  src_regid2 = mips_function_read(self, ast_ir, 2);
  dest_regid = mips_function_write(self, ast_ir, 0);
  switch(ast_ir->opcode) {
    case OP_ADD:
      ir = mips_add(dest_regid, src_regid1, src_regid2);
      break;
    case OP_SUB:
      ir = mips_sub(dest_regid, src_regid1, src_regid2);
      break;
    default:
      cling_default_assert(ast_ir->opcode, cling_ast_opcode_kind_tostring);
  }
  mips_function_push_back(self, ir);
}

/*
 * Optionally move temp to $v0.
 */
static void mips_emit_ret(struct cling_mips_function *self,
    struct cling_ast_ir const *ast_ir) {
  uint8_t regid;
  if (ast_ir->operands[0].info != CL_NULL) {
    regid = mips_function_read(self, ast_ir, 0);
    mips_function_push_back(self, mips_move(MIPS_V0, regid));
  }
}

/*
 * Calculation that takes 2 operands in mips but 3 operands
 * in ast_ir.
 * like, div $t1, $t2, mflo $t3
 */
static void mips_emit_binary_calc(struct cling_mips_function *self,
                                 struct cling_ast_ir const *ast_ir) {
  uint8_t src_regid1, src_regid2, dest_regid;
  struct cling_mips_ir *ir;

  src_regid1 = mips_function_read(self, ast_ir,1);
  src_regid2 = mips_function_read(self, ast_ir,2);
  switch (ast_ir->opcode) {
  case OP_DIV:
    ir = mips_div(src_regid1, src_regid2);
    break;
  case OP_MUL:
    ir = mips_mult(src_regid1, src_regid2);
    break;
  default:
    cling_default_assert(ast_ir->opcode, cling_ast_opcode_kind_tostring);
  }
  mips_function_push_back(self, ir);
  dest_regid = mips_function_write(self, ast_ir, 0);
  mips_function_push_back(self, mips_mflo(dest_regid));
}

static void mips_emit_call(struct cling_mips_function *self, 
    struct cling_ast_ir const *ast_ir) {
  char const *callee;
  int regid;

  callee=ast_ir->operands[0].text;
  mips_function_push_back(self, mips_jal(callee));
  if (ast_ir->operands[1].info != CL_NULL) {
    regid=mips_function_write(self, ast_ir, 1);
    mips_function_push_back(self, mips_move(regid, MIPS_V0));
  }
  mips_load_temps_paras(self, true);
}

/*
 * index temp(base_wide) index(is_load).
 * where temp is the address of the array,
 * base_wide is the elemsz,
 * index is the result of the expr,
 * is_load tells us final result is the address or content.
 */
static void mips_emit_index(struct cling_mips_function *self,
    struct cling_ast_ir const *ast_ir) {
  uint8_t index_regid;
  uint8_t array_regid;
  uint8_t dest_regid;
  size_t base_size;

  array_regid=mips_function_read(self, ast_ir, 1);
  index_regid=mips_function_read(self, ast_ir, 2);
  dest_regid=mips_function_write(self, ast_ir, 0);
  base_size=getsize(ast_ir->operands[0].info);
  /*
   * We borrow dest_regid to hold the elemsz.
   */
  mips_function_push_back(self, mips_li(dest_regid, base_size));
  mips_function_push_back(self, mips_mult(index_regid, dest_regid));
  mips_function_push_back(self, mips_mflo(dest_regid));
  mips_function_push_back(self, mips_add(dest_regid, dest_regid, array_regid));
  mips_function_temp_unmap(self, array_regid);

  /*
   * Now we have A+index in dest_regid.
   */
  /*
   * If this is a rhs index, we need to load the content.
   */
  if (ast_ir->operands[1].info) {
    mips_function_push_back(self, base_size == MIPS_WORD_SIZE?
        mips_lw(dest_regid, 0, dest_regid):
        mips_lb(dest_regid, 0, dest_regid));
  }
}

/*
 * ldstr string temp.
 * load the label of the string into temp.
 */
static void mips_emit_ldstr(struct cling_mips_function *self,
    struct cling_ast_ir const *ast_ir) {
  uint8_t regid;
  char const *label;

  regid=mips_function_read(self, ast_ir, 1);
  label=mips_function_label(self, ast_ir, 0);
  mips_function_push_back(self, mips_la(regid, label));
}

/*
 * ldimm value(wide) temp
 * No matter what is the wide, we load it using li.
 */
static void mips_emit_ldimm(struct cling_mips_function *self,
    struct cling_ast_ir const *ast_ir) {
  uint8_t dest_regid;
  size_t size;

  dest_regid=mips_function_write(self, ast_ir, 1);
  size=getsize(ast_ir->operands[0].info);
  mips_function_push_back(self, 
      mips_li(dest_regid, size == MIPS_WORD_SIZE?
      ast_ir->operands[0].imme_int:
      ast_ir->operands[0].imme_char));
}

/*
 * store temp1(wide) temp2
 * Where temp1 holds address of variable.
 */
static void mips_emit_store(struct cling_mips_function *self,
    struct cling_ast_ir const *ast_ir) {
  uint8_t addr_regid;
  uint8_t src_regid;
  size_t size;

  addr_regid=mips_function_read(self, ast_ir, 0);
  src_regid=mips_function_read(self, ast_ir, 1);
  size=getsize(ast_ir->operands[0].info);
  mips_function_push_back(self, size == MIPS_WORD_SIZE?
      mips_sw(src_regid, 0, addr_regid):
      mips_sb(src_regid, 0, addr_regid));
  mips_function_temp_unmap(self, addr_regid);
}

/*
 * We utilize the fact that the grammar restrict the use of condition
 * expression right above a branch instruction to translate condition
 * ir directly to conditional branch.
 * Also notes that, expression (without relop) is allowed in condition,
 * so we use a state to deduct that.
 */

/*
 * Translate relop like OP_NE to mips dialect, blt, ble...
 * Note the implicit ast_ir following relop is OP_BEZ.
 */
static struct cling_mips_ir *
mips_branch_zero(int relop, uint8_t regid, uint16_t offset) {
  struct cling_mips_ir *ir;

  switch(relop) {
      case OP_GT:
        ir=mips_ir_create(MIPS_BLEZ);
        break;
      case OP_GE:
        ir=mips_ir_create(MIPS_BLTZ);
        break;
      case OP_LE:
        ir=mips_ir_create(MIPS_BGTZ);
        break;
      case OP_LT:
        ir=mips_ir_create(MIPS_BGEZ);
        break;
  }
  ir->operands[0].regid=regid;
  ir->operands[1].offset=offset;
  return ir;
}

static void mips_emit_branch_relop(struct cling_mips_function *self, 
    struct cling_ast_ir const *relop) {
  assert(relop);
  uint8_t src_regid1, src_regid2, dest_regid;
  struct cling_mips_ir *ir;

  src_regid1=mips_function_read(self, relop, 1);
  src_regid2=mips_function_read(self, relop, 2);

  switch(relop->opcode) {
    /*
     * Black is white.
     * White is black.
     */
      case OP_NE:
        ir=mips_beq(src_regid1, src_regid2, MIPS_ADDR_NULL);
        break;
      case OP_EQ:
        ir=mips_bne(src_regid1, src_regid2, MIPS_ADDR_NULL);
        break;
      case OP_GT:
      case OP_GE:
      case OP_LE:
      case OP_LT:
        dest_regid=mips_function_write(self, relop, 0);
        mips_function_push_back(self, mips_sub(dest_regid, src_regid1, src_regid2));
        ir=mips_branch_zero(relop->opcode, dest_regid, MIPS_ADDR_NULL);
        break;
  }
  mips_function_push_back(self, ir);
}

/*
 * Handle simple branch that does not involve relop.
 */
static void mips_emit_branch(struct cling_mips_function *self,
    struct cling_ast_ir const *ast_ir) {
  uint8_t src_regid1, src_regid2;

  switch(ast_ir->opcode) {
    case OP_BEZ:
      /*
       * branch if equals to zero.
       */
      src_regid1=mips_function_read(self, ast_ir, 0);
      mips_function_push_back(self,
          mips_beq(src_regid1, MIPS_ZERO, MIPS_ADDR_NULL));
      break;
    case OP_BNE:
      /*
       * branch if not equals.
       */
      src_regid1=mips_function_read(self, ast_ir, 0);
      src_regid2=mips_function_read(self, ast_ir, 1);
      mips_function_push_back(self,
          mips_bne(src_regid1, src_regid2, MIPS_ADDR_NULL));
      break;
    case OP_BNZ:
      /*
       * branch if not equals to zero.
       */
      src_regid1=mips_function_read(self, ast_ir, 0);
      mips_function_push_back(self,
          mips_bne(src_regid1, MIPS_ZERO, MIPS_ADDR_NULL));
      break;
  }
}

/*
 * load name(scope|wide) temp(is_load)
 * load the address or content of name into temp.
 */
static void mips_emit_load(struct cling_mips_function *self,
    struct cling_ast_ir const *ast_ir) {
  uint8_t dest_regid;
  struct cling_mips_name *var;

  bool is_global=ast_ir->operands[0].info & CL_GLBL;
  char const *name=ast_ir->operands[0].text;
  bool is_load=ast_ir->operands[1].info;
  size_t size=getsize(ast_ir->operands[0].info);

  dest_regid=mips_function_write(self, ast_ir, 1);
  if (is_global) {
    /*
     * TODO: Currently the name is the label of the global
     * variable.
     */
    mips_function_push_back(self, mips_la(dest_regid, name));
  } else {
    var=utillib_hashmap_at(&self->names, name);
    mips_function_push_back(self, mips_addi(dest_regid, MIPS_SP, var->offset));
  }
  /*
   * now dest_regid holds the address of this variable.
   */
  if (is_load) {
    mips_function_push_back(self, size == MIPS_WORD_SIZE ?
        mips_lw(dest_regid, 0, dest_regid):
        mips_lb(dest_regid, 0, dest_regid));
  }
}

static void mips_emit_jmp(struct cling_mips_function *self,
    struct cling_ast_ir const *ast_ir) {
  mips_function_push_back(self, mips_j(MIPS_ADDR_NULL));
}

/*
 * read temp(wide)
 * temp is the address to store the value in.
 */
static void mips_emit_read(struct cling_mips_function *self,
    struct cling_ast_ir const *ast_ir) {
  size_t size;
  uint8_t regid;

  mips_function_push_back(self, mips_li(MIPS_V0, MIPS_READ_INT));
  mips_function_push_back(self, &cling_mips_syscall);
  size=getsize(ast_ir->operands[0].info);
  regid=mips_function_read(self, ast_ir, 0);
  mips_function_push_back(self, size == MIPS_WORD_SIZE?
      mips_sw(MIPS_V0, 0, regid):
      mips_sb(MIPS_V0, 0, regid));
}

/*
 * write temp
 * temp is the address of a string or word.
 */
static void mips_emit_write(struct cling_mips_function *self,
    struct cling_ast_ir const *ast_ir) {
  uint8_t regid;
  if (self->para_size) {
    /*
     * Save $a0 to sp+frame_size.
     */
    mips_function_push_back(self, mips_sw(MIPS_A0, self->frame_size, MIPS_SP));
  }
  regid=mips_function_read(self, ast_ir, 0);
  /*
   * syscall number.
   */
  mips_function_push_back(self, 
      mips_li(MIPS_V0, ast_ir->opcode == OP_WRSTR ?
        MIPS_PRINT_STRING:MIPS_PRINT_INT));
  /*
   * syscall argument.
   */
  mips_function_push_back(self, mips_move(MIPS_A0, regid));
  mips_function_push_back(self, &cling_mips_syscall);
  if (self->para_size) {
    /*
     * Restore $a0.
     */
    mips_function_push_back(self, mips_lw(MIPS_A0, self->frame_size, MIPS_SP));
  }
}

/*
 * End of translation of different ast_ir.
 */

/*
 * Allocate space for arguments greater than four from MIPS_MEM_ARG_BLK.
 */
static uint32_t mips_function_argalloc(uint32_t *args_blk, size_t size) { 
  return MIPS_MEM_ARG_BLK + mips_align_alloc(args_blk, size, size);
}

/*
 * Loop over all the ast_ir and emit mips_ir.
 */
static void mips_function_instrs(struct cling_mips_function *self,
    struct cling_ast_function const *ast_func) {
  size_t ast_instrs_size;
  /*
   * We use the same way to handle push seq.
   */
  int push_state = 0;
  struct cling_ast_ir const *ast_ir;
  uint8_t regid;
  size_t size;
  struct cling_mips_ir *ir;
  uint32_t args_blk;
  uint32_t offset;
  struct cling_ast_ir const* relop=NULL;

  ast_instrs_size = utillib_vector_size(&ast_func->instrs);
  for (int ast_pc = 0; ast_pc < ast_instrs_size; ++ast_pc) {
    ast_ir = utillib_vector_at(&ast_func->instrs, ast_pc);
    switch (ast_ir->opcode) {
      case OP_SUB:
      case OP_ADD:
        mips_emit_ternary_calc(self, ast_ir);
        break;
      case OP_RET:
        mips_emit_ret(self, ast_ir);
        break;
      case OP_DIV:
      case OP_MUL:
        mips_emit_binary_calc(self, ast_ir);
        break;
      case OP_PUSH:
        if (push_state == 0) {
          mips_load_temps_paras(self, false);
          args_blk=0;
        } 
        regid=mips_function_read(self, ast_ir, 0);
        if (push_state < MIPS_REG_ARGS_N) {
          ir=mips_move(MIPS_A0+push_state, regid);
        } else {
          size=getsize(ast_ir->operands[0].info);
          offset=mips_function_argalloc(&args_blk, size);
          if (size == MIPS_WORD_SIZE)
            ir=mips_sw(regid, offset, MIPS_SP);
          else
            ir=mips_sb(regid, offset, MIPS_SP);
        }
        mips_function_push_back(self, ir);
        ++push_state;
        break;
      case OP_CAL:
        mips_emit_call(self, ast_ir);
        push_state=0;
        break;
      case OP_NE:
      case OP_EQ:
      case OP_GT:
      case OP_GE:
      case OP_LE:
      case OP_LT:
        relop=ast_ir;
        break;
      case OP_BNZ:
      case OP_BEZ:
      case OP_BNE:
        if (relop) {
          /*
           * bez is used to impl if.
           */
          assert(ast_ir->opcode == OP_BEZ);
          mips_emit_branch_relop(self, relop);
          relop = NULL;
        } else {
          mips_emit_branch(self, ast_ir);
        }
        break;
      case OP_LDIMM:
        mips_emit_ldimm(self, ast_ir);
        break;
      case OP_LOAD:
        mips_emit_load(self, ast_ir);
        break;
      case OP_STORE:
        mips_emit_store(self, ast_ir);
        break;
      case OP_IDX:
        mips_emit_index(self, ast_ir);
        break;
      case OP_JMP:
        mips_emit_jmp(self, ast_ir);
        break;
      case OP_READ:
        mips_emit_read(self, ast_ir);
        break;
      case OP_WRITE:
        mips_emit_write(self, ast_ir);
        break;
      default:
        cling_default_assert(ast_ir->opcode,
            cling_ast_opcode_kind_tostring);
    }
    mips_function_temp_ages(self);
  }
}

/*
 * Driver to translate all the ast_ir to mips_ir.
 */
static void mips_function_emit(struct cling_mips_function *self,
    struct cling_ast_function const *ast_func) {
  mips_function_stack_layout(self, ast_func);
  mips_function_prologue(self);
  mips_function_instrs(self, ast_func);
  mips_function_epilogue(self);
}

/*
 * Data Directive
 */

static struct cling_mips_data*
mips_data_create(uint8_t type, char const *label) {
  struct cling_mips_data *self=malloc(sizeof *self);
  self->type=type;
  self->label=strdup(label);
  return self;
}

static struct cling_mips_data *
mips_array_create(char const *label, size_t extend) {
  struct cling_mips_data *self=mips_data_create(MIPS_SPACE, label);
  self->extend=extend;
  return self;
}

static struct cling_mips_data *
mips_string_create(char const *label, char const *string) {
  struct cling_mips_data *self=mips_data_create(MIPS_SPACE, label);
  self->string=strdup(string);
  return self;
}

static void mips_data_destroy(struct cling_mips_data *self) {
  free(self->label);
  if (self->type == MIPS_ASCIIZ)
    free(self->string);
  free(self);
}

static void mips_data_print(struct cling_mips_data const *self, FILE *file) {
  fprintf(file, "%s: %s ", self->label, cling_mips_data_kind_tostring(self->type));
  switch(self->type) {
    case MIPS_ASCIIZ:
      fprintf(file, "\"%s\"\n", self->string);
      break;
    case MIPS_BYTE:
    case MIPS_WORD:
      fputs("0\n", file);
      break;
    case MIPS_SPACE:
      fprintf(file, "%lu\n", self->extend);
      break;
  }
}

/*
 * cling_mips_program
 */

static void mips_program_emit_data(struct cling_mips_program *self, struct cling_ast_program const *program) {
  struct cling_ast_ir const *ast_ir;
  int info;
  struct cling_mips_data *data;
  char const *name;
  size_t base_size;

  UTILLIB_VECTOR_FOREACH(ast_ir, &program->init_code) {
    switch(ast_ir->opcode) {
      case OP_DEFVAR:
        info=ast_ir->operands[0].info;
        name=ast_ir->operands[0].text;
        if (info & CL_WORD)
          data=mips_data_create(MIPS_WORD, name);
        else
          data=mips_data_create(MIPS_BYTE, name);
        utillib_vector_push_back(&self->data, data);
        break;
      case OP_DEFARR:
        name=ast_ir->operands[0].text;
        base_size=getsize(ast_ir->operands[0].info);
        data=mips_array_create(name, base_size * ast_ir->operands[1].scalar);
        utillib_vector_push_back(&self->data, data);
        break;
    }
  }
}

/*
 * Write a few lines at the most front to jump to main
 * and then exit.
 */
static void mips_program_setup(struct cling_mips_program *self) {
  struct utillib_vector *text=&self->text;
  utillib_vector_push_back(text, mips_jal("main"));
  utillib_vector_push_back(text, mips_li(MIPS_V0, MIPS_EXIT));
  utillib_vector_push_back(text, &cling_mips_syscall);
}

static void mips_global_init(struct cling_mips_global *self, struct utillib_hashmap *string) {
  self->instr_offset=0;
  self->string=string;
}

void cling_mips_program_init(struct cling_mips_program *self) {
  utillib_vector_init(&self->text);
  utillib_vector_init(&self->data);
  utillib_hashmap_init(&self->string, &cling_string_hash);
}

void cling_mips_program_destroy(struct cling_mips_program *self) {
  utillib_vector_destroy_owning(&self->text, mips_ir_destroy);
  utillib_vector_destroy_owning(&self->data, mips_data_destroy);
}

void cling_mips_program_emit(struct cling_mips_program *self, struct cling_ast_program *program) {
  struct cling_mips_global global;
  struct cling_mips_function function;
  struct cling_ast_function const *ast_func;

  mips_global_init(&global, &self->string);
  mips_program_setup(self);
  mips_program_emit_data(self, program);

  /* const int sizes[]={ 1, 1, 4, 4, 1, -1 }; */
  /* int blk=0; */
  /* uint32_t offset; */
  /* for (const int *p=sizes; *p!=-1; ++p) { */
  /*   offset=mips_align_alloc(&blk, *p); */
  /*   printf("Address %lu, blk %lu", offset, blk); */
  /* } */


  /* for (int i=0; i<cling_mips_opcode_kind_N; ++i) { */
  /*   struct cling_mips_ir *ir=mips_ir_create(i); */
  /*   mips_ir_tostring(ir); */
  /* } */

  UTILLIB_VECTOR_FOREACH(ast_func, &program->funcs) {
    mips_function_init(&function, &self->text, &global, ast_func);
    mips_function_emit(&function, ast_func);
    mips_function_destroy(&function);
  }

}

void cling_mips_program_print(struct cling_mips_program const *self, FILE *file) {
  struct cling_mips_data const *data;
  struct cling_mips_ir const *ir;

  fputs(".data\n", file);
  UTILLIB_VECTOR_FOREACH(data, &self->data) {
    mips_data_print(data, file);
  }
  fputs(".text\n", file);
  fputs(".globl main\n", file);
  UTILLIB_VECTOR_FOREACH(ir, &self->text) {
    fprintf(file, "%s\n", mips_ir_tostring(ir));
  }
}

/*
 * Test
 */
static void mips_mock(void) {
  struct cling_mips_ir * ir[20];
  ir[0]=&cling_mips_syscall;
  ir[1]=mips_move(MIPS_V0, MIPS_T0);
  ir[2]=mips_addi(MIPS_SP, MIPS_SP, 100);
  ir[3]=mips_addu(MIPS_S1, MIPS_ZERO, MIPS_S3);
  ir[4]=mips_jal("function");
  ir[5]=mips_j(0x14);
  ir[6]=mips_jr(MIPS_S0);
  ir[7]=mips_bne(MIPS_S0, MIPS_T2, -1);
  ir[9]=mips_mflo(MIPS_T1);
  ir[10]=mips_lw(MIPS_S1, 10, MIPS_SP);
  ir[11]=mips_div(MIPS_S2, MIPS_T1);
  ir[12]=mips_la(MIPS_S0, "Address");
  ir[13]=mips_li(MIPS_T0, 0x11111);
  ir[14]=mips_sw(MIPS_S0, 2, MIPS_A0);
  for (int i=0; i< 15; ++i) {
    puts(mips_ir_tostring(ir[i]));
  }
}
