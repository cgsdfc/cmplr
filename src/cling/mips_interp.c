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
#include "mips_interp.h"

/*
 * string as key.
 */
static int mips_label_strcmp(struct cling_mips_label const *lhs,
                             struct cling_mips_label const *rhs) {
  return strcmp(lhs->label, rhs->label);
}

static size_t mips_label_strhash(struct cling_mips_label const *self) {
  return php_strhash(self->label);
}

static const struct utillib_hashmap_callback mips_label_strcallback = {
    .compare_handler = mips_label_strcmp, .hash_handler = mips_label_strhash,
};

void cling_mips_interp_init(struct cling_mips_interp *self,
                            struct cling_mips_program const *program) {
  self->pc = 0;
  self->lo = 0;
  self->program = program;
  utillib_hashmap_init(&self->labels, &mips_label_strcallback);
  memset(self->page_array, 0, sizeof self->page_array);
  memset(self->regs, 0, sizeof self->regs);
  mips_interp_load(self, program);
}

static void mips_interp_load_data(struct cling_mips_interp *self) {
  struct cling_mips_label *label;
  struct cling_address_range *range;
  struct cling_mips_data *data;
  uint32_t address;

  UTILLIB_VECTOR_FOREACH(data, &program->data) {
    switch (data->type) {
    case MIPS_SPACE:
      address = mips_interp_alloc(self, data->extend, true);
      break;
    case MIPS_WORD:
      address = mips_interp_alloc(self, MIPS_WORD_SIZE, true);
      break;
    case MIPS_BYTE:
      address = mips_interp_alloc(self, MIPS_BYTE_SIZE, true);
      break;
    case MIPS_ASCIIZ:
      address = mips_interp_alloc(self, strlen(data->string) + 1, false);
      strcpy((char *)(self->memory + address), data->string);
      break;
    }
    label = mips_label_create(data->label, address);
    utillib_hashmap_insert(&self->labels, label, label);
  }
}

static int mips_do_syscall(struct cling_mips_interp *self) {
  int int_val;
  char char_val;
  char const *str_val;

  switch (self->regs[MIPS_V0]) {
  case MIPS_PRINT_INT:
    printf("%d", (int)self->regs[MIPS_A0]);
    break;
  case MIPS_PRINT_STRING:
    str_val = (char const *)self->memory + self->regs[MIPS_A0];
    printf("%s", str_val);
    break;
  case MIPS_PRINT_CHAR:
    printf("%c", (char)self->regs[MIPS_A0]);
    break;
  case MIPS_READ_INT:
    scanf("%d", &int_val);
    self->regs[MIPS_V0] = int_val;
    break;
  case MIPS_READ_CHAR:
    scanf("%c", &char_val);
    self->regs[MIPS_V0] = char_val;
    break;
  case MIPS_EXIT:
    return MIPS_EC_EXIT;
  default:
  case MIPS_READ_STRING:
    assert(false);
  }
  return MIPS_EC_EXIT;
}

static void mips_do_move(struct cling_mips_interp *self,
                         struct cling_mips_ir const *ir) {
  uint8_t dest_regid = ir->operands[0].regid;
  uint8_t src_regid = ir->operands[1].regid;
  self->regs[dest_regid] = self->regs[src_regid];
}

static int mips_do_addi(struct cling_mips_interp *self,
                        struct cling_mips_ir const *ir) {
  uint8_t dest_regid = ir->operands[0].regid;
  uint8_t src_regid = ir->operands[1].regid;
  int16_t imme = ir->operands[2].imme16;
  self->regs[dest_regid] = self->regs[src_regid] + imme;
  /*
   * TODO: Overflow Detect.
   */
  return MIPS_EC_OK;
}

static void mips_do_mflo(struct cling_mips_interp *self,
                         struct cling_mips_ir const *ir) {
  uint8_t dest_regid = ir->operands[0].regid;
  self->regs[dest_regid] = self->lo;
}

static void mips_do_jal(struct cling_mips_interp *self,
                        struct cling_mips_ir const *ir) {
  struct cling_mips_label const *label;
  assert(label);
  self->regs[MIPS_RA] = self->pc + 1;
  self->pc = label->address;
}

static void mips_do_div(struct cling_mips_interp *self,
                        struct cling_mips_ir const *ir) {
  uint8_t src_regid1 = ir->operands[0].regid;
  uint8_t src_regid2 = ir->operands[1].regid;
  self->lo = self->regs[src_regid1] / self->regs[src_regid2];
}

static int mips_do_beq(struct cling_mips_interp *self,
                       struct cling_mips_ir const *ir) {
  uint8_t src_regid1 = ir->operands[0].regid;
  uint8_t src_regid2 = ir->operands[1].regid;
  if (self->regs[src_regid1] == self->regs[src_regid2]) {
    self->pc = self->pc + ir->operands[2].offset + 1;
    return MIPS_EC_BTAKEN;
  }
  return MIPS_EC_OK;
}

static void mips_do_li(struct cling_mips_interp *self,
                       struct cling_mips_ir const *ir) {
  uint8_t dest_regid = ir->operands[0].regid;
  self->regs[dest_regid] = ir->operands[1].imme32;
}

static void mips_do_jr(struct cling_mips_interp *self,
                       struct cling_mips_ir const *ir) {
  uint8_t src_regid = ir->operands[0].regid;
  self->pc = self->regs[src_regid];
}

static uint32_t mips_do_address(struct cling_mips_interp *self,
                                struct cling_mips_ir const *ir) {
  uint8_t base_regid = ir->operands[2].regid;
  int16_t offset = ir->operands[1].offset;
  return self->regs[base_regid] + offset;
}

static int mips_do_lw(struct cling_mips_interp *self,
                      struct cling_mips_ir const *ir) {
  uint32_t address = mips_do_address(self, ir);
  uint8_t dest_regid = ir->operands[0].regid;
  if (address & (MIPS_WORD_SIZE - 1))
    return MIPS_EC_ALIGN;
  self->regs[dest_regid] = *(uint32_t *)(self->memory + address);
  return 0;
}

static int mips_do_sw(struct cling_mips_interp *self,
                      struct cling_mips_ir const *ir) {
  uint32_t address = mips_do_address(self, ir);
  uint8_t dest_regid = ir->operands[0].regid;
  if (address & (MIPS_WORD_SIZE - 1))
    return MIPS_EC_ALIGN;
}

static void mips_do_la(struct cling_mips_interp *self,
                       struct cling_mips_ir const *ir) {
  struct cling_mips_label const *label;
  uint8_t dest_regid;
  dest_regid = ir->operands[0].regid;
  assert(label);
  self->regs[dest_regid] = label->address;
}

void cling_mips_interp_destroy(struct cling_mips_interp *self) {
  free(self->memory);
}

int mips_do_branch(struct cling_mips_interp *self,
                   struct cling_mips_ir const *ir) {}

void mips_do_j(struct cling_mips_interp *self, struct cling_mips_ir const *ir) {

}

int cling_mips_interp_exec(struct cling_mips_interp *self) {
  struct cling_mips_ir const *ir;
  const size_t instr_size = mips_program_size(self->program);

  while (true) {
  fetch:
    ir = mips_program_instr(self->program, self->pc);
    switch (ir->opcode) {
    case MIPS_NOP:
      break;
    case MIPS_SYSCALL:
      if (MIPS_EC_EXIT == mips_do_syscall(self))
        return 0;
    case MIPS_MOVE:
      mips_do_move(self, ir);
      break;
    case MIPS_ADDI:
      mips_do_addi(self, ir);
      break;
    case MIPS_MFLO:
      mips_do_mflo(self, ir);
      break;
    case MIPS_ADDU:
    case MIPS_SUB:
    case MIPS_SUBU:
      assert(false);
    case MIPS_JAL:
      mips_do_jal(self, ir);
      goto fetch;
    case MIPS_DIV:
      mips_do_div(self, ir);
      break;
    case MIPS_MULT:
    case MIPS_ADD:
    case MIPS_J:
      mips_do_j(self, ir);
      goto fetch;
    case MIPS_JR:
      mips_do_jr(self, ir);
      goto fetch;
    case MIPS_BNE:
    case MIPS_BEQ:
    case MIPS_BGEZ:
    case MIPS_BGTZ:
    case MIPS_BLEZ:
    case MIPS_BLTZ:
      if (MIPS_EC_BTAKEN == mips_do_branch(self, ir))
        goto fetch;
      break;
    case MIPS_LW:
      if (MIPS_EC_ALIGN == mips_do_lw(self, ir))
        return MIPS_EC_ALIGN;
      break;
    case MIPS_SW:
    case MIPS_LB:
    case MIPS_SB:
    case MIPS_LA:
    case MIPS_LI:
      mips_do_li(self, ir);
      break;
    default:
      assert(false);
    }
    ++self->pc;
    if (self->pc >= instr_size)
      return 0;
  }
}
