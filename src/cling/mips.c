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

static struct cling_mips_data *mips_string_create(char const *label,
                                                  char const *string);

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
static struct cling_mips_ir *mips_branch(uint8_t opcode, uint8_t src_regid1,
                                         uint8_t src_regid2, int16_t offset) {
  struct cling_mips_ir *self = mips_ir_create(opcode);
  self->operands[0].regid = src_regid1;
  self->operands[1].regid = src_regid2;
  self->operands[2].offset = offset;
  return self;
}

static struct cling_mips_ir *mips_beq(uint8_t src_regid1, uint8_t src_regid2,
                                      int16_t offset) {
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
    free(self->operands[0].label);
    break;
  case MIPS_LA:
    free(self->operands[1].label);
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
    regstr[0] = mips_operand_regstr(self, 0);
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
  case MIPS_LI:
    regstr[0] = mips_operand_regstr(self, 0);
    mips_ir_snprintf(("%s %s, %" PRId32), opstr, regstr[0],
                     self->operands[1].imme32);
    return buffer;
  case MIPS_BNE:
  case MIPS_BEQ:
    regstr[0] = mips_operand_regstr(self, 0);
    regstr[1] = mips_operand_regstr(self, 1);
    mips_ir_snprintf(("%s %s %s %" PRId16), opstr, regstr[0], regstr[1], self->operands[2].offset);
    return buffer;
  case MIPS_BGEZ:
  case MIPS_BGTZ:
  case MIPS_BLEZ:
  case MIPS_BLTZ:
    regstr[0]=mips_operand_regstr(self, 0);
    mips_ir_snprintf(("%s %s %" PRId16), opstr, regstr[0], self->operands[1].offset);
    return buffer;
  default:
    assert(false);
  }
}

/*
 * End of MIPS routines.
 */

/*
 * We use PARA and ARG to differentiate the parameters passed to
 * **this** function and arguments this function passes to call others.
 */

static struct cling_mips_name *mips_name_create(uint8_t regid,
                                                uint32_t offset) {
  struct cling_mips_name *self = malloc(sizeof *self);
  self->regid = regid;
  self->offset = offset;
  return self;
}

static void mips_function_init(struct cling_mips_function *self,
                               struct utillib_vector *instrs,
                               struct cling_mips_global *global,
                               struct cling_ast_function const *ast_func) {
  self->global = global;
  self->instrs = instrs;
  self->temp_size = ast_func->temps;
  self->frame_size = 0;
  /*
   * Record the beginning of mips_instrs.
   */
  self->instr_begin = utillib_vector_size(instrs);
  self->reg_pool = calloc(sizeof self->reg_pool[0], CLING_MIPS_REG_MAX);
  self->address_map = malloc(sizeof self->address_map[0] *
                             utillib_vector_size(&ast_func->instrs));
  self->temps = calloc(sizeof self->temps[0], ast_func->temps);
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
    entry->offset = offset;
    return;
  }
  entry = mips_name_create(MIPS_REGR_NULL, offset);
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
  entry = mips_name_create(regid, MIPS_ADDR_NULL);
  utillib_hashmap_insert(&self->names, strdup(name), entry);
}

/*
 * Fix all the address of jump/branch in range of
 * [self->instr_begin, utillib_vector_size(self->instrs)).
 */
static void mips_function_fix_address(struct cling_mips_function *self) {
  struct cling_mips_ir *mips_ir;
  int instr_end = utillib_vector_size(self->instrs);
  int ast_ir_addr;
#define mips_branch_offset(self, ast_ir_addr, mips_pc)\
  ((self)->address_map[(ast_ir_addr)]-(mips_pc)-1)
  for (int mips_pc = self->instr_begin; mips_pc < instr_end; ++mips_pc) {
    mips_ir = utillib_vector_at(self->instrs, mips_pc);
    switch (mips_ir->opcode) {
    case MIPS_J:
      ast_ir_addr = mips_ir->operands[0].address;
      mips_ir->operands[0].address = self->address_map[ast_ir_addr];
      break;
    case MIPS_BEQ:
    case MIPS_BNE:
      ast_ir_addr = mips_ir->operands[2].offset;
      mips_ir->operands[2].offset =
        mips_branch_offset(self, ast_ir_addr, mips_pc);
      break;
    case MIPS_BGEZ:
    case MIPS_BGTZ:
    case MIPS_BLEZ:
    case MIPS_BLTZ:
      ast_ir_addr = mips_ir->operands[1].offset;
      mips_ir->operands[1].offset =
        mips_branch_offset(self, ast_ir_addr, mips_pc);
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
  utillib_vector_push_back(&self->saved_registers,
                           mips_name_create(regid, offset));
}

/*
 * A typical prologue is,
 * addi $sp, $sp, -128
 * sw $ra, 20($sp)
 * sw $s0, 24($sp)
 * # save other saved registers.
 */
static void mips_function_prologue(struct cling_mips_function *self) {
  struct cling_mips_ir *ir, *addi_sp;
  struct cling_mips_name const *saved;

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
 */
static void mips_function_epilogue(struct cling_mips_function *self) {
  struct cling_mips_ir *ir, *addi_sp;
  struct cling_mips_name const *saved;

  UTILLIB_VECTOR_FOREACH(saved, &self->saved_registers) {
    ir = mips_lw(saved->regid, saved->offset, MIPS_SP);
    mips_function_push_back(self, ir);
  }
  addi_sp = mips_addi(MIPS_SP, MIPS_SP, self->frame_size);
  mips_function_push_back(self, addi_sp);
}

/*
 * Similar to prologue
 * and epilogue but it is for a subroutine call.
 * They save/load the temp registers in use and a0s.
 * The para_size controls the total number of registers
 * we saved.
 */
static void mips_load_temps(struct cling_mips_function *self, bool is_load) {
  struct cling_mips_temp *temp_one;
  for (int i = 0; i < self->temp_size; ++i) {
    temp_one = &self->temps[i];
    if (temp_one->regid != MIPS_REGR_NULL) {
      mips_function_push_back(
          self, is_load ? mips_lw(temp_one->regid, temp_one->offset, MIPS_SP)
                        : mips_sw(temp_one->regid, temp_one->offset, MIPS_SP));
    }
  }
}

static void mips_function_load_paras(struct cling_mips_function *self, int index, bool is_load) {
  assert(index < MIPS_MEM_ARG_BLK);
  mips_function_push_back(self, is_load ?
      mips_lw(MIPS_A0 + index, self->frame_size + (index << 2), MIPS_SP):
      mips_sw(MIPS_A0 + index, self->frame_size + (index << 2), MIPS_SP));
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
  while (now & (align - 1))
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
        arg_size = ast_ir->push.size;
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
  for (int i = 0; i < self->temp_size; ++i) {
    if (mips_is_saved_register(self->temps[i].regid))
      continue;
    local_offset = mips_function_memalloc(self, MIPS_WORD_SIZE, MIPS_WORD_SIZE);
    mips_function_memmap_temp(self, i, local_offset);
  }

  UTILLIB_VECTOR_FOREACH(ast_ir, &ast_func->init_code) {
    switch (ast_ir->opcode) {
    case OP_DEFVAR:
      name = ast_ir->defvar.name;
      size=ast_ir->defvar.size;
      local_offset = mips_function_memalloc(self, size, size);
      mips_function_memmap_name(self, name, local_offset);
      break;
    case OP_DEFARR:
      name = ast_ir->defarr.name;
      base_size = ast_ir->defarr.base_size;
      size = ast_ir->defarr.extend;
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
 * Fills the temps with those temps having a saved_register.
 */
static void
mips_function_save_registers(struct cling_mips_function *self,
                             struct cling_ast_function const *ast_func) {
  struct cling_ast_ir const *ast_ir;
  struct cling_mips_name *named_var;
  uint32_t saved_offset;
  char const *name;
  int regid;
  int para_cnt = 0;

  UTILLIB_VECTOR_FOREACH(ast_ir, &ast_func->init_code) {
    switch (ast_ir->opcode) {
    case OP_LOAD:
      if (ast_ir->load.is_global)
        continue;
      name = ast_ir->load.name;
      named_var = utillib_hashmap_at(&self->names, name);
      if (!named_var)
        /*
         * If not in names, it failed to own saved_register.
         */
        continue;
      self->temps[ast_ir->load.temp].regid = named_var->regid;
      break;
    case OP_PARA:
      if (para_cnt++ < MIPS_REG_ARGS_N)
        continue;
    /* Fall through */
    case OP_DEFVAR:
      regid = mips_function_regalloc(self, mips_is_saved_register);
      if (regid == MIPS_REGR_NULL)
        continue;
      name = ast_ir->defvar.name;
      saved_offset =
          mips_function_memalloc(self, MIPS_WORD_SIZE, MIPS_WORD_SIZE);
      mips_function_saved_push_back(self, regid, saved_offset);
      mips_function_regmap_name(self, name, regid);
      break;
    }
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
    name = ast_ir->para.name;
    if (para_cnt < MIPS_REG_ARGS_N) {
      para_size = MIPS_WORD_SIZE;
    } else {
      para_size = ast_ir->para.size;
    }
    para_offset =
        mips_align_alloc(&para_blk, para_size, para_size) + self->frame_size;
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
    if (mips_is_temp_register(self->temps[i].regid) &&
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
static void mips_function_temp_unmap(struct cling_mips_function *self,
                                     uint8_t temp) {
  struct cling_mips_temp *temp_var;
  temp_var = &self->temps[temp];
  if (!mips_is_temp_register(temp_var->regid))
    return;
  mips_function_regfree(self, temp_var->regid);
  temp_var->regid = MIPS_REGR_NULL;
}

/*
 * Ages all the temp that is alive.
 */
static void mips_function_temp_ages(struct cling_mips_function *self) {
  for (int i = 0; i < self->temp_size; ++i) {
    if (mips_is_temp_register(self->temps[i].regid))
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
   * LRU Only applies to temp register.
   */
  assert(mips_is_temp_register(temp_one->regid));
  if (temp_one->age)
    --temp_one->age;
}

static uint8_t mips_function_temp_map(struct cling_mips_function *self,
                                      uint16_t temp, bool is_read) {
  struct cling_mips_temp *temp_var;
  uint8_t regid;

  temp_var = &self->temps[temp];
  if (mips_is_saved_register(temp_var->regid)) {
    return temp_var->regid;
  }

  if (temp_var->regid != MIPS_REGR_NULL) {
    mips_function_temp_refer(self, temp);
    return temp_var->regid;
  }

  regid = mips_function_temp_alloc(self);
  temp_var->regid = regid;
  temp_var->age = 0;
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
static inline uint8_t mips_function_read(struct cling_mips_function *self, int temp) {
  return mips_function_temp_map(self, temp, true);
}

/*
 * For writing, the content will be overwritten anyway, so we just return its
 * register or allocate a register for it.
 */
static inline uint8_t mips_function_write(struct cling_mips_function *self, int temp) {
  return mips_function_temp_map(self, temp, false);
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

  src_regid1 = mips_function_read(self, ast_ir->binop.temp1);
  src_regid2 = mips_function_read(self, ast_ir->binop.temp2);
  dest_regid = mips_function_write(self, ast_ir->binop.result);
  switch (ast_ir->opcode) {
  case OP_ADD:
    ir = mips_add(dest_regid, src_regid1, src_regid2);
    break;
  case OP_SUB:
    ir = mips_sub(dest_regid, src_regid1, src_regid2);
    break;
  default:
    assert(false);
  }
  mips_function_push_back(self, ir);
}

/*
 * Optionally move temp to $v0.
 * Jump to epilogue.
 */
static void mips_emit_ret(struct cling_mips_function *self,
                          struct cling_ast_ir const *ast_ir) {
  uint8_t regid;
  if (ast_ir->ret.has_result) {
    regid = mips_function_read(self, ast_ir->ret.result);
    mips_function_push_back(self, mips_move(MIPS_V0, regid));
  }
  mips_function_push_back(self, mips_j(ast_ir->ret.addr));
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

  src_regid1 = mips_function_read(self, ast_ir->binop.temp1);
  src_regid2 = mips_function_read(self, ast_ir->binop.temp2);
  switch (ast_ir->opcode) {
  case OP_DIV:
    ir = mips_div(src_regid1, src_regid2);
    break;
  case OP_MUL:
    ir = mips_mult(src_regid1, src_regid2);
    break;
  default:
    assert(false);
  }
  mips_function_push_back(self, ir);
  dest_regid = mips_function_write(self, ast_ir->binop.result);
  mips_function_push_back(self, mips_mflo(dest_regid));
}

/*
 * call name [temp]
 * The call prologue has finished in pushes.
 * This function handles epilogue.
 */
static void mips_emit_call(struct cling_mips_function *self,
                           struct cling_ast_ir const *ast_ir) {
  char const *callee;
  int regid;

  callee = ast_ir->call.name;
  mips_function_push_back(self, mips_jal(callee));
  if (ast_ir->call.has_result) {
    regid = mips_function_write(self, ast_ir->call.result);
    mips_function_push_back(self, mips_move(regid, MIPS_V0));
  }
}

/*
 * index temp(result) temp(base_wide) index(is_rvalue).
 * where temp is the address of the array,
 * base_wide is the elemsz,
 * index is the result of the expr,
 * is_rvalue tells us final result is the address or content.
 */
static void mips_emit_index(struct cling_mips_function *self,
                            struct cling_ast_ir const *ast_ir) {
  uint8_t index_regid;
  uint8_t array_regid;
  uint8_t dest_regid;
  size_t base_size;
  bool is_rvalue;

  is_rvalue = ast_ir->index.is_rvalue;
  base_size = ast_ir->index.base_size;
  array_regid = mips_function_read(self, ast_ir->index.array_addr);
  index_regid = mips_function_read(self, ast_ir->index.index_result);
  dest_regid = mips_function_write(self, ast_ir->index.result);
  /*
   * We borrow dest_regid to hold the elemsz.
   */
  mips_function_push_back(self, mips_li(dest_regid, base_size));
  mips_function_push_back(self, mips_mult(index_regid, dest_regid));
  mips_function_push_back(self, mips_mflo(dest_regid));
  mips_function_push_back(self, mips_add(dest_regid, dest_regid, array_regid));
  /*
   * Now we have A+index in dest_regid.
   */
  /*
   * If this is a rhs index, we need to load the content.
   */
  if (is_rvalue) {
    mips_function_push_back(self, base_size == MIPS_WORD_SIZE
                                      ? mips_lw(dest_regid, 0, dest_regid)
                                      : mips_lb(dest_regid, 0, dest_regid));
  }
}

/*
 * Allocate an .asciiz for this string in the .data
 * segment.
 * Notes every string will have its own label. They
 * are not pooled.
 */
static char const *mips_global_asciiz(struct cling_mips_global *self,
                                      char const *string) {
#define MIPS_LABEL_BUFSIZ 64
  static char buffer[MIPS_LABEL_BUFSIZ];
  snprintf(buffer, MIPS_LABEL_BUFSIZ, "__asciiz_%d",
           self->label_count);
  ++self->label_count;
  utillib_vector_push_back(self->data, mips_string_create(buffer, string));
  return buffer;
}

static void mips_emit_ldstr(struct cling_mips_function *self,
                            struct cling_ast_ir const *ast_ir) {
  uint8_t regid;
  char const *label;

  label = mips_global_asciiz(self->global, ast_ir->ldstr.string);
  regid = mips_function_write(self, ast_ir->ldstr.temp);
  mips_function_push_back(self, mips_la(regid, label));
}

static void mips_emit_ldimm(struct cling_mips_function *self,
                            struct cling_ast_ir const *ast_ir) {
  uint8_t dest_regid;
  size_t size;

  dest_regid = mips_function_write(self, ast_ir->ldimm.temp);
  size = ast_ir->ldimm.size;
  mips_function_push_back(self, mips_li(dest_regid, ast_ir->ldimm.value)); 
}

static void mips_emit_store(struct cling_mips_function *self,
                            struct cling_ast_ir const *ast_ir) {
  uint8_t addr_regid;
  uint8_t src_regid;
  size_t size;

  addr_regid = mips_function_read(self, ast_ir->store.addr);
  src_regid = mips_function_read(self, ast_ir->store.value);
  size = ast_ir->store.size;
  mips_function_push_back(self, size == MIPS_WORD_SIZE
                                    ? mips_sw(src_regid, 0, addr_regid)
                                    : mips_sb(src_regid, 0, addr_regid));
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
 * Note the address is borrowed from ast_ir.
 */
static struct cling_mips_ir *mips_branch_relop(int relop, uint8_t regid,
                                               uint16_t offset) {
  struct cling_mips_ir *ir;

  switch (relop) {
  case OP_GT:
    ir = mips_ir_create(MIPS_BLEZ);
    break;
  case OP_GE:
    ir = mips_ir_create(MIPS_BLTZ);
    break;
  case OP_LE:
    ir = mips_ir_create(MIPS_BGTZ);
    break;
  case OP_LT:
    ir = mips_ir_create(MIPS_BGEZ);
    break;
  default:
    assert(false);
  }
  ir->operands[0].regid = regid;
  ir->operands[1].offset = offset;
  return ir;
}

/*
 * Handle each specific relop.
 */
static void mips_emit_branch_relop(struct cling_mips_function *self,
                                   struct cling_ast_ir const *relop,
                                   struct cling_ast_ir const *ast_ir) {
  assert(relop);
  assert(ast_ir->opcode == OP_BEZ);
  assert(ast_ir->bez.temp == relop->binop.result);
  uint8_t src_regid1, src_regid2, dest_regid;
  struct cling_mips_ir *ir;

  src_regid1 = mips_function_read(self, relop->binop.temp1);
  src_regid2 = mips_function_read(self, relop->binop.temp2);

  switch (relop->opcode) {
  /*
   * Black is white.
   * White is black.
   */
  case OP_NE:
    ir = mips_beq(src_regid1, src_regid2, ast_ir->bez.addr);
    break;
  case OP_EQ:
    ir = mips_bne(src_regid1, src_regid2, ast_ir->bez.addr);
    break;
  case OP_GT:
  case OP_GE:
  case OP_LE:
  case OP_LT:
    dest_regid = mips_function_write(self, relop->binop.result);
    mips_function_push_back(self, mips_sub(dest_regid, src_regid1, src_regid2));
    ir = mips_branch_relop(relop->opcode, dest_regid, ast_ir->bez.addr);
    break;
  default:
    assert(false);
  }
  mips_function_push_back(self, ir);
}

/*
 * Handle simple branch that does not involve relop.
 * Note the address for the mips_irs this function
 * creates is function-absolute **borrowed** from ast_ir.
 */
static void mips_emit_branch(struct cling_mips_function *self,
                             struct cling_ast_ir const *ast_ir) {
  uint8_t src_regid1, src_regid2;

  switch (ast_ir->opcode) {
  case OP_BEZ:
    /*
     * branch if equals to zero.
     */
    src_regid1 = mips_function_read(self, ast_ir->bez.temp);
    mips_function_push_back(self, mips_beq(src_regid1, MIPS_ZERO, ast_ir->bez.addr));
    break;
  case OP_BNE:
    /*
     * branch if not equals.
     */
    src_regid1 = mips_function_read(self, ast_ir->bne.temp1);
    src_regid2 = mips_function_read(self, ast_ir->bne.temp2);
    mips_function_push_back(self, mips_bne(src_regid1, src_regid2, ast_ir->bne.addr));
    break;
  default:
    assert(false);
  }
}

/*
 * load the address or content of name into temp.
 */
static void mips_emit_load(struct cling_mips_function *self,
                           struct cling_ast_ir const *ast_ir) {
  assert(ast_ir->opcode == OP_LOAD);
  register struct cling_mips_name *var;
  uint8_t dest_regid = mips_function_write(self, ast_ir->load.temp);

  if (ast_ir->load.is_global) {
    /*
     * TODO: Currently the name is the label of the global
     * variable.
     */
    mips_function_push_back(self, mips_la(dest_regid, ast_ir->load.name));
  } else {
    var = utillib_hashmap_at(&self->names, ast_ir->load.name);
    mips_function_push_back(self, mips_addi(dest_regid, MIPS_SP, var->offset));
  }
  /*
   * now dest_regid holds the address of this variable.
   */
  if (ast_ir->load.is_rvalue) {
    mips_function_push_back(self, ast_ir->load.size == MIPS_WORD_SIZE
                                      ? mips_lw(dest_regid, 0, dest_regid)
                                      : mips_lb(dest_regid, 0, dest_regid));
  }
}

/*
 * Notes the function-absolute address.
 */
static void mips_emit_jmp(struct cling_mips_function *self,
                          struct cling_ast_ir const *ast_ir) {
  mips_function_push_back(self, mips_j(ast_ir->jmp.addr));
}

/*
 * temp is the address to store the value in.
 * syscall.
 */
static void mips_emit_read(struct cling_mips_function *self,
                           struct cling_ast_ir const *ast_ir) {
  uint8_t regid;
  int syscall;

  syscall=cling_ast_opcode_to_syscall(ast_ir->opcode);
  mips_function_push_back(self, mips_li(MIPS_V0, syscall));
  mips_function_push_back(self, &cling_mips_syscall);
  regid = mips_function_write(self, ast_ir->read.temp);
  mips_function_push_back(self, ast_ir->opcode == OP_RDINT
                                    ? mips_sw(MIPS_V0, 0, regid)
                                    : mips_sb(MIPS_V0, 0, regid));
}

/*
 * temp is the address of a string or word.
 * Maybe write-string or wrint-int.
 */
static void mips_emit_write(struct cling_mips_function *self,
                            struct cling_ast_ir const *ast_ir) {
  uint8_t regid;
  int syscall;
  if (self->para_size) {
    /*
     * Save $a0 to sp+frame_size.
     */
    mips_function_push_back(self, mips_sw(MIPS_A0, self->frame_size, MIPS_SP));
  }
  /*
   * syscall argument.
   */
  regid = mips_function_read(self, ast_ir->write.temp);
  mips_function_push_back(self, mips_move(MIPS_A0, regid));
  /*
   * syscall number.
   */
  syscall=cling_ast_opcode_to_syscall(ast_ir->opcode);
  mips_function_push_back(self, mips_li(MIPS_V0, syscall));
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
 * Enter with a push or call, end with the first instruction 
 * after call.
 */
static int mips_emit_push_call(struct cling_mips_function *self,
    size_t begin, size_t ast_instrs_size, 
    struct cling_ast_function const* ast_func) {
  int ast_pc;
  uint8_t regid;
  struct cling_ast_ir const *ast_ir;
  int push_state=0;
  size_t size;
  uint32_t offset;
  struct cling_mips_ir *ir;
  uint32_t args_blk=0;

  for (ast_pc = begin; ast_pc < ast_instrs_size; ++ast_pc) {
    ast_ir=utillib_vector_at(&ast_func->instrs, ast_pc);
    switch(ast_ir->opcode) {
    case OP_PUSH:
      if (push_state == 0) {
        mips_load_temps(self, false);
      }
      regid = mips_function_read(self, ast_ir->push.temp);
      if (push_state < MIPS_REG_ARGS_N) {
        mips_function_load_paras(self, push_state, false);
        ir = mips_move(MIPS_A0 + push_state, regid);
      } else {
        size=ast_ir->push.size;
        offset = mips_function_argalloc(&args_blk, size);
        if (size == MIPS_WORD_SIZE)
          ir = mips_sw(regid, offset, MIPS_SP);
        else
          ir = mips_sb(regid, offset, MIPS_SP);
      }
      mips_function_push_back(self, ir);
      ++push_state;
      break;
    case OP_CAL:
      if (push_state == 0) {
        mips_load_temps(self, false);
      }
      mips_emit_call(self, ast_ir);
      mips_load_temps(self, true);
      while (push_state) {
        --push_state;
        mips_function_load_paras(self, push_state, true);
      }
      break;
    default:
      return ast_pc;
    }
  }
}

/*
 * Loop over all the ast_ir and emit mips_ir.
 */
static void mips_function_instrs(struct cling_mips_function *self,
                                 struct cling_ast_function const *ast_func) {
  size_t ast_instrs_size;
  struct cling_ast_ir const *ast_ir;
  struct cling_ast_ir const *relop = NULL;

  ast_instrs_size = utillib_vector_size(&ast_func->instrs);
  for (int ast_pc = 0; ast_pc < ast_instrs_size; ++ast_pc) {
    ast_ir = utillib_vector_at(&ast_func->instrs, ast_pc);
    self->address_map[ast_pc] = utillib_vector_size(self->instrs);
    switch (ast_ir->opcode) {
    case OP_NOP:
      break;
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
    case OP_CAL:
      ast_pc=mips_emit_push_call(self, ast_pc, ast_instrs_size, ast_func);
      break;
    case OP_NE:
    case OP_EQ:
    case OP_GT:
    case OP_GE:
    case OP_LE:
    case OP_LT:
      relop = ast_ir;
      break;
    case OP_BEZ:
      /*
       * bez is used to impl if.
       */
      if (relop) {
        mips_emit_branch_relop(self, relop, ast_ir);
        relop = NULL;
      } else {
        mips_emit_branch(self, ast_ir);
      }
      break;
    case OP_BNE:
      /*
       * bne is used in `switch'.
       */
      mips_emit_branch(self, ast_ir);
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
    case OP_RDCHR:
    case OP_RDINT:
      mips_emit_read(self, ast_ir);
      break;
    case OP_WRSTR:
    case OP_WRINT:
    case OP_WRCHR:
      mips_emit_write(self, ast_ir);
      break;
    case OP_LDSTR:
      mips_emit_ldstr(self, ast_ir);
      break;
    default:
      assert(false);
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
  if (self->frame_size)
    mips_function_prologue(self);
  mips_function_instrs(self, ast_func);
  if (self->frame_size)
    mips_function_epilogue(self);
  mips_function_fix_address(self);
  mips_function_push_back(self, mips_jr(MIPS_RA));
}

/*
 * Data Directive
 */

static struct cling_mips_data *mips_data_create(uint8_t type,
                                                char const *label) {
  struct cling_mips_data *self = malloc(sizeof *self);
  self->type = type;
  self->label = strdup(label);
  return self;
}

static struct cling_mips_data *mips_array_create(char const *label,
                                                 size_t extend) {
  struct cling_mips_data *self = mips_data_create(MIPS_SPACE, label);
  self->extend = extend;
  return self;
}

static struct cling_mips_data *mips_string_create(char const *label,
                                                  char const *string) {
  struct cling_mips_data *self = mips_data_create(MIPS_ASCIIZ, label);
  self->string = strdup(string);
  return self;
}

static void mips_data_destroy(struct cling_mips_data *self) {
  free(self->label);
  if (self->type == MIPS_ASCIIZ)
    free(self->string);
  free(self);
}

static void mips_data_print(struct cling_mips_data const *self, FILE *file) {
  fprintf(file, "%s: %s ", self->label,
          cling_mips_data_kind_tostring(self->type));
  switch (self->type) {
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

static void mips_program_emit_data(struct cling_mips_program *self,
                                   struct cling_ast_program const *program) {
  struct cling_ast_ir const *ast_ir;
  int info;
  struct cling_mips_data *data;
  char const *name;
  size_t base_size;

  UTILLIB_VECTOR_FOREACH(ast_ir, &program->init_code) {
    switch (ast_ir->opcode) {
    case OP_DEFVAR:
      if (ast_ir->defvar.size == MIPS_WORD_SIZE)
        data = mips_data_create(MIPS_WORD, ast_ir->defvar.name);
      else
        data = mips_data_create(MIPS_BYTE, ast_ir->defvar.name);
      utillib_vector_push_back(&self->data, data);
      break;
    case OP_DEFARR:
      data =
          mips_array_create(ast_ir->defarr.name, 
              ast_ir->defarr.base_size * ast_ir->defarr.extend);
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
  struct utillib_vector *text = &self->text;
  utillib_vector_push_back(text, mips_jal("main"));
  utillib_vector_push_back(text, mips_li(MIPS_V0, MIPS_EXIT));
  utillib_vector_push_back(text, &cling_mips_syscall);
}

static void mips_global_init(struct cling_mips_global *self,
                             struct utillib_vector *data) {
  self->label_count = 0;
  self->data = data;
}

void cling_mips_program_init(struct cling_mips_program *self,
                             struct cling_ast_program const *program) {
  utillib_vector_init(&self->text);
  utillib_vector_init(&self->data);
  self->func_size = utillib_vector_size(&program->funcs);
  self->func_range = malloc(sizeof self->func_range[0] * self->func_size);
}

void cling_mips_program_destroy(struct cling_mips_program *self) {
  utillib_vector_destroy_owning(&self->text, mips_ir_destroy);
  utillib_vector_destroy_owning(&self->data, mips_data_destroy);
  free(self->func_range);
}

void cling_mips_program_emit(struct cling_mips_program *self,
                             struct cling_ast_program const *program) {
  struct cling_mips_global global;
  struct cling_mips_function function;
  struct cling_ast_function const *ast_func;
  int index = 0;

  mips_global_init(&global, &self->data);
  mips_program_emit_data(self, program);

  UTILLIB_VECTOR_FOREACH(ast_func, &program->funcs) {
    self->func_range[index].begin = utillib_vector_size(&self->text);
    /*
     * Notes: the name is borrowed from ast_func->name.
     */
    self->func_range[index].label = ast_func->name;
    mips_function_init(&function, &self->text, &global, ast_func);
    mips_function_emit(&function, ast_func);
    mips_function_destroy(&function);
    self->func_range[index].end = utillib_vector_size(&self->text);
    ++index;
  }
}

void cling_mips_program_print(struct cling_mips_program const *self,
                              FILE *file) {
  struct cling_mips_data const *data;
  struct cling_mips_ir const *ir;

  if (!utillib_vector_empty(&self->data)) {
    fputs(".data\n", file);
    UTILLIB_VECTOR_FOREACH(data, &self->data) { mips_data_print(data, file); }
  }
  fputs(".text\n", file);
  fputs(".globl main\n", file);
  for (int i = 0; i < self->func_size; ++i) {
    fprintf(file, "%s:\n", self->func_range[i].label);
    for (int begin = self->func_range[i].begin, end = self->func_range[i].end;
         begin < end; ++begin) {
      ir = utillib_vector_at(&self->text, begin);
      fprintf(file, "%4" PRIu32 "\t%s\n", begin, mips_ir_tostring(ir));
    }
    fputs("\n", file);
  }
}
