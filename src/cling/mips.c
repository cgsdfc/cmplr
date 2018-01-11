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
#include "option.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#define MIPS_REGR_NULL 0

/*
 * The name of the procedurce that prints a single newline
 */
#define MIPS_AUTO_NEWLINE "__auto_newline"
/*
 * The name of the newline string
 */
#define MIPS_NEWLINE "__newline"

/*
 * Four MIPS_WORD s
 */
#define MIPS_MEM_ARG_BLK (MIPS_WORD_SIZE << 2)
#define MIPS_REG_ARGS_N 4

/*
 * Size of the static buffer.
 */
#define MIPS_BUFSIZ 128
static char buffer[MIPS_BUFSIZ];

/*
 * Short cut for snprintf.
 * Notes it uses buffer implicitly.
 */
#define mips_ir_snprintf(FORMAT, ...)                                          \
        do {                                                                         \
                snprintf(buffer, MIPS_BUFSIZ, FORMAT, __VA_ARGS__);                        \
        } while (0)

/*
 * We define them as static instance because
 * they take no arguments.
 */
static const struct cling_mips_ir cling_mips_nop = {.opcode = MIPS_NOP};
static const struct cling_mips_ir cling_mips_syscall = {.opcode = MIPS_SYSCALL};

static struct cling_mips_data *mips_string_create(char const *label,
                char const *string);
static char *mips_global_label(struct cling_mips_global *self,
                uint32_t address);

static void mips_function_saved_push_back(struct cling_mips_function *self,
                uint8_t regid, uint32_t offset);
static int mips_regalloc(struct cling_mips_function *self,
                bool (*regkind)(uint8_t regid));

static void mips_para_context(struct cling_mips_function *self, int flag);

static void mips_function_load_paras(struct cling_mips_function *self, int index, bool is_load);
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
        struct cling_mips_ir *self = calloc(1, sizeof *self);
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
 * j <address> <label>
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
        return self;
}

/*
 * li $s0, imme32
 * load a 32 bit imme into $s0.
 */
static struct cling_mips_ir *mips_li(uint8_t dest_regid, int32_t imme32) {
        struct cling_mips_ir *self = mips_ir_create(MIPS_LI);
        self->operands[0].regid = dest_regid;
        self->operands[1].imme32 = imme32;
        return self;
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
 * The instructions are grouped by similar format.
 */
/*
 * Turns a regid field of self into a regstr.
 * Notes it does not check for the regid.
 */
#define mips_operand_regstr(self, index)                                       \
        cling_mips_regster_tostring(self->operands[index].regid)
void mips_ir_fprint(struct cling_mips_ir const *self, FILE *file) 
{
        char const *opstr = cling_mips_opcode_kind_tostring(self->opcode);
        char const *regstr[CLING_MIPS_OPERAND_MAX];
        switch (self->opcode) {
                case MIPS_NOP:
                case MIPS_SYSCALL:
                        fprintf(file, "%s\n", opstr);
                        break;
                case MIPS_SW:
                case MIPS_LW:
                case MIPS_SB:
                case MIPS_LB:
#ifndef NDEBUG
                        if (self->operands[1].offset & 3) {
                                printf(">>> Align Error: %" PRId16 "\n", self->operands[1].offset);
                        }
#endif
                        regstr[0] = mips_operand_regstr(self, 0);
                        regstr[2] = mips_operand_regstr(self, 2);
                        fprintf(file, ("%s %s, %" PRId16 "(%s)\n"), opstr, regstr[0],
                                        self->operands[1].offset, regstr[2]);
                        break;
                case MIPS_ADD:
                case MIPS_ADDU:
                case MIPS_SUB:
                case MIPS_SUBU:
                        regstr[0] = mips_operand_regstr(self, 0);
                        regstr[1] = mips_operand_regstr(self, 1);
                        regstr[2] = mips_operand_regstr(self, 2);
                        fprintf(file, "%s %s, %s, %s\n", opstr, regstr[0], regstr[1], regstr[2]);
                        break;
                case MIPS_MOVE:
                case MIPS_DIV:
                case MIPS_MULT:
                        regstr[0] = mips_operand_regstr(self, 0);
                        regstr[1] = mips_operand_regstr(self, 1);
                        fprintf(file, "%s %s, %s\n", opstr, regstr[0], regstr[1]);
                        break;
                case MIPS_LA:
                        regstr[0] = mips_operand_regstr(self, 0);
                        fprintf(file, "%s %s, %s\n", opstr, regstr[0], self->operands[1].label);
                        break;
                case MIPS_JAL:
                        fprintf(file, "%s %s\n", opstr, self->operands[0].label);
                        break;
                case MIPS_MFLO:
                case MIPS_JR:
                        regstr[0] = mips_operand_regstr(self, 0);
                        fprintf(file, "%s %s\n", opstr, regstr[0]);
                        break;
                case MIPS_J:
                        fprintf(file, "%s %s\n", opstr, self->operands[0].label);
                        break;
                case MIPS_ADDI:
                        regstr[0] = mips_operand_regstr(self, 0);
                        regstr[1] = mips_operand_regstr(self, 1);
                        fprintf(file, ("%s %s, %s, %" PRId16 "\n"), opstr, regstr[0], regstr[1],
                                        self->operands[2].imme16);
                        break;
                case MIPS_LI:
                        regstr[0] = mips_operand_regstr(self, 0);
                        fprintf(file, ("%s %s, %" PRId32 "\n"), opstr, regstr[0],
                                        self->operands[1].imme32);
                        break;
                case MIPS_BNE:
                case MIPS_BEQ:
                        regstr[0] = mips_operand_regstr(self, 0);
                        regstr[1] = mips_operand_regstr(self, 1);
                        fprintf(file, "%s %s, %s, %s\n", opstr, regstr[0], regstr[1],
                                        self->operands[2].label);
                        break;
                case MIPS_BGEZ:
                case MIPS_BGTZ:
                case MIPS_BLEZ:
                case MIPS_BLTZ:
                        regstr[0] = mips_operand_regstr(self, 0);
                        fprintf(file, "%s %s, %s\n", opstr, regstr[0], self->operands[1].label);
                        break;
                default:
                        assert(false);
        }
}

/*
 * We use PARA and ARG to differentiate the parameters passed to
 * **this** function and arguments this function passes to call others.
 */

static void mips_function_init(struct cling_mips_function *self,
                struct utillib_vector *instrs,
                struct cling_mips_global *global,
                struct cling_ast_function const *ast_func) {
        self->last_temp = 1;
        self->global = global;
        self->instrs = instrs;
        self->temp_size = ast_func->temps;
        self->ast_instrs_size=utillib_vector_size(&ast_func->instrs);
        self->frame_size = 0;
        self->para_size=0;
        self->instr_begin = utillib_vector_size(instrs);
        self->reg_used =calloc(CLING_MIPS_REG_MAX, sizeof self->reg_used[0]);
        self->address_map = malloc(sizeof self->address_map[0] * self->ast_instrs_size);
        self->temps = calloc(self->temp_size, sizeof self->temps[0]);
        utillib_hashmap_init(&self->names, &cling_string_hash);
        utillib_vector_init(&self->saved);
}

static void mips_function_destroy(struct cling_mips_function *self) {
        free(self->reg_used);
        free(self->address_map);
        free(self->temps);
        utillib_vector_destroy_owning(&self->saved, free);
        utillib_hashmap_destroy_owning(&self->names, NULL, free);
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

/*
 * Fix all the address of jump/branch in range of
 * [self->instr_begin, utillib_vector_size(self->instrs)).
 */
static void mips_function_fix_address(struct cling_mips_function *self)
{
        struct cling_mips_ir *mips_ir;
        int instr_end = utillib_vector_size(self->instrs);
        int ast_ir_addr;
        uint32_t mips_ir_addr;

#define mips_getaddr_safe(self, ast_ir_addr) \
        ({\
         if ((ast_ir_addr) >= (self->ast_instrs_size)) {\
         printf("ast_ir_addr %d ast_instrs_size %d\n", ast_ir_addr, self->ast_instrs_size);\
         }\
         assert((ast_ir_addr) < (self->ast_instrs_size));\
         self->address_map[ast_ir_addr];\
         })

        for (int mips_pc = self->instr_begin; mips_pc < instr_end; ++mips_pc) {
                mips_ir = utillib_vector_at(self->instrs, mips_pc);
                switch (mips_ir->opcode) {
                case MIPS_J:
                        /*
                         * Lonely J needs a label to jump to!
                         */
                        ast_ir_addr = mips_ir->operands[0].address;
                        mips_ir_addr = mips_getaddr_safe(self, ast_ir_addr);
                        mips_ir->operands[0].label =
                                mips_global_label(self->global, mips_ir_addr);
                        break;
                case MIPS_BEQ:
                case MIPS_BNE:
                        /*
                         * Mars requires everything that can jump to is a label,
                         * so we hack the offset of branch family to be _all_absolute_address_
                         * just like J. And it no longer use offset but now use label
                         * since only 3 fields are available.
                         */
                        ast_ir_addr = mips_ir->operands[2].offset;
                        mips_ir_addr = mips_getaddr_safe(self, ast_ir_addr);
                        mips_ir->operands[2].label =
                                mips_global_label(self->global, mips_ir_addr);
                        break;
                case MIPS_BGEZ:
                case MIPS_BGTZ:
                case MIPS_BLEZ:
                case MIPS_BLTZ:
                        ast_ir_addr = mips_ir->operands[1].offset;
                        mips_ir_addr = mips_getaddr_safe(self, ast_ir_addr);
                        mips_ir->operands[1].label =
                                mips_global_label(self->global, mips_ir_addr);
                        break;
        }
}
}

static void mips_function_push_back(struct cling_mips_function *self,
                struct cling_mips_ir const *ir) {
        utillib_vector_push_back(self->instrs, ir);
}

static void mips_context_switch(struct cling_mips_function *self, int flag) {

        struct cling_mips_name const *saved;
        UTILLIB_VECTOR_FOREACH(saved, &self->saved) {
                mips_function_push_back(self, flag == MIPS_SAVE ?
                                mips_sw(saved->regid, saved->offset, MIPS_SP):
                                mips_lw(saved->regid, saved->offset, MIPS_SP));
        }

}

/*
 * A typical prologue is,
 * addi $sp, $sp, -128
 * sw $ra, 20($sp)
 * sw $s0, 24($sp)
 * # save other saved registers.
 */
static void mips_function_prologue(struct cling_mips_function *self) {
        mips_function_push_back(self, 
                        mips_addi(MIPS_SP, MIPS_SP, -self->frame_size));
        mips_context_switch(self, MIPS_SAVE);
        mips_para_context(self, MIPS_SAVE);
}

/*
 * A typical epilogue is,
 * lw $s0, 20($sp)
 * lw $ra, 24($sp)
 * addi $sp, $sp, 128
 */
static void mips_function_epilogue(struct cling_mips_function *self) {
        mips_context_switch(self, MIPS_LOAD);
        mips_function_push_back(self, 
                        mips_addi(MIPS_SP, MIPS_SP, self->frame_size));
}

/*
 * Save temps, if any in use.
 */
static void mips_temp_context(struct cling_mips_function *self, int flag)
{
        struct cling_mips_temp *temp_one;
        for (int i = 0; i < self->temp_size; ++i) {
                temp_one = &self->temps[i];
                if (temp_one->kind == MIPS_TEMP &&
                                temp_one->state == MIPS_HAS_REG) {
                        mips_function_push_back(
                                        self, flag == MIPS_LOAD ? 
                                        mips_lw(temp_one->regid, temp_one->offset, MIPS_SP)
                                        : mips_sw(temp_one->regid, temp_one->offset, MIPS_SP));
                }
        }
}

/*
 * Save parameters, if any.
 */
static void mips_para_context(struct cling_mips_function *self, int flag)
{
        for (int i=0; i<self->para_size && i<MIPS_REG_ARGS_N; ++i) {
                mips_function_push_back(
                                self, flag == MIPS_LOAD
                                ? mips_lw(MIPS_A0 + i, self->frame_size + (i << 2), MIPS_SP)
                                : mips_sw(MIPS_A0 + i, self->frame_size + (i << 2), MIPS_SP));
        }
}

/*
 * Stack Layout code.
 */

/*
 * size is the number of words.
 */
static uint32_t mips_alloc(struct cling_mips_function *self, unsigned int size)
{
        uint32_t alloc=self->frame_size;
        self->frame_size+=size<<2;
        return alloc;
}

/*
 * Figures out the maximum stack space required for
 * any function called by self. We use STATE MACHINE.
 */
static uint32_t
mips_function_max_args(struct cling_ast_function const *ast_func) {
        struct cling_ast_ir const *ast_ir;
        uint32_t arg_size;
        uint32_t max_args = 0;

        UTILLIB_VECTOR_FOREACH(ast_ir, &ast_func->instrs) {
                if (ast_ir->opcode != OP_CAL)
                        continue;
                arg_size= ast_ir->call.argc;
                if (arg_size > max_args)
                        max_args = arg_size;
        }
        return max_args << 2;
}

static void mips_function_memmap(struct cling_mips_function *self,
                char const *name, uint32_t offset) {
        struct cling_mips_name *entry;
        entry = utillib_hashmap_find(&self->names, name);
        if (entry) {
                entry->offset = offset;
                return;
        }
        entry=malloc(sizeof *entry);
        entry->offset=offset;
        utillib_hashmap_insert(&self->names, name, entry);
}

static void mips_function_temp_layout(struct cling_mips_function *self,
                struct cling_ast_function const *ast_func) 
{
        struct cling_mips_name *var;
        struct cling_ast_ir const *ast_ir;

        for (int i=0;i<self->temp_size; ++i) {
                self->temps[i].kind=MIPS_TEMP;
        }
        UTILLIB_VECTOR_FOREACH(ast_ir, &ast_func->instrs) {
                if (ast_ir->opcode == OP_LDNAM && ast_ir->ldnam.scope) {
                        var=utillib_hashmap_find(&self->names, ast_ir->ldnam.name);
                        /*
                         * These temps are alias of var.
                         */
                        self->temps[ast_ir->ldnam.temp].kind=var->kind;
                        self->temps[ast_ir->ldnam.temp].regid=var->regid;
                }
        }
        for (int i=0; i<self->temp_size; ++i) {
                if (self->temps[i].kind == MIPS_TEMP) {
                        self->temps[i].offset=mips_alloc(self, 1);
                        self->temps[i].state=MIPS_NO_REG;
                }
        }
}

static void
mips_function_local_layout(struct cling_mips_function *self,
                struct cling_ast_function const *ast_func) {
        struct cling_ast_ir const *ast_ir;

        UTILLIB_VECTOR_FOREACH(ast_ir, &ast_func->init_code) {
                switch (ast_ir->opcode) {
                        case OP_DEFVAR:
                                mips_function_memmap(self, ast_ir->defvar.name,
                                                mips_alloc(self, 1));
                                break;
                        case OP_DEFARR:
                                mips_function_memmap(self, ast_ir->defarr.name, 
                                                mips_alloc(self, ast_ir->defarr.extend));
                                break;
                }
        }
}

static void mips_function_regmap(struct cling_mips_function *self,
                char const *name, int kind, int regid) {
        struct cling_mips_name *entry;
        entry = utillib_hashmap_find(&self->names, name);
        if (entry) {
                entry->kind=kind;
                entry->regid=regid;
                return;
        }
        entry = malloc(sizeof *entry);
        entry->kind=kind;
        entry->regid=regid;
        utillib_hashmap_insert(&self->names, name, entry);
}

static void
mips_function_save_registers(struct cling_mips_function *self,
                struct cling_ast_function const *ast_func) {
        struct cling_ast_ir const *ast_ir;
        uint32_t saved_offset;
        int regid;
        int para_cnt = 0;

        UTILLIB_VECTOR_FOREACH(ast_ir, &ast_func->init_code) {
                switch (ast_ir->opcode) {
                        case OP_PARA:
                                if (para_cnt < MIPS_REG_ARGS_N) {
                                        mips_function_regmap(self, ast_ir->para.name, MIPS_ARGREG, 
                                                        MIPS_A0+para_cnt);
                                } else {
                                        mips_function_regmap(self, ast_ir->para.name, MIPS_ARGMEM, 0);
                                }
                                ++para_cnt;
                                break;
                        case OP_DEFVAR:
                                regid = mips_regalloc(self, mips_is_saved_register);
                                if (regid == MIPS_REGR_NULL) {
                                        mips_function_regmap(self, ast_ir->defvar.name, MIPS_UNSAVED, 0);
                                } else {
                                        saved_offset = mips_alloc(self, 1);
                                        mips_function_saved_push_back(self, regid, saved_offset);
                                        mips_function_regmap(self, ast_ir->defvar.name, MIPS_SAVED,  regid); 
                                }
                                break;
                }
        }
}

static void mips_function_saved_push_back(struct cling_mips_function *self,
                uint8_t regid, uint32_t offset) {
        struct cling_mips_name *name=malloc(sizeof *name);
        name->regid=regid;
        name->offset=offset;
        utillib_vector_push_back(&self->saved, name);
}

static void mips_function_save_ra(struct cling_mips_function *self) {
        /*
         * Save $ra.
         * Allocate a word for it and add it to the saved.
         */
        uint32_t offset;
        offset = mips_alloc(self, 1);
        mips_function_saved_push_back(self, MIPS_RA, offset);
}

/*
 * If this function is non leaf, layouts
 * its Argument Region, which must be the
 * first region on the top of stack.
 * The space is reserved for future use of call.
 */
inline static void
mips_function_args_layout(struct cling_mips_function *self,
                struct cling_ast_function const *ast_func) {
        self->frame_size=mips_function_max_args(ast_func);
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
        uint32_t para_offset;

        UTILLIB_VECTOR_FOREACH(ast_ir, &ast_func->init_code) {
                if (ast_ir->opcode != OP_PARA)
                        continue;
                name = ast_ir->para.name;
                para_offset = (self->para_size<<2) + self->frame_size;
                mips_function_memmap(self, name, para_offset);
                ++self->para_size;
        }
}

/*
 * Driver that layouts all kinds of stuffs on the stack.
 */
static void
mips_function_stack_layout(struct cling_mips_function *self,
                struct cling_ast_function const *ast_func) {
        bool is_leaf;

        is_leaf = mips_function_is_leaf(ast_func);

        if (!is_leaf) {
                mips_function_args_layout(self, ast_func);
        }
        mips_function_save_registers(self, ast_func);
        if (!is_leaf || self->global->option->auto_newline) {
                mips_function_save_ra(self);
        }
        mips_function_local_layout(self, ast_func);
        mips_function_temp_layout(self, ast_func);
        mips_function_para_layout(self, ast_func);
}

static uint8_t mips_function_temp_alloc(struct cling_mips_function *self) {
        uint8_t regid;
        uint32_t offset;

        regid = mips_regalloc(self, mips_is_temp_register);
        if (regid != MIPS_REGR_NULL)
                return regid;

        if (self->last_temp >= self->temp_size) {
                self->last_temp=1;
        }

        for (int i = self->last_temp; i < self->temp_size; ++i) {
                if (self->temps[i].kind != MIPS_TEMP && 
                                self->temps[i].kind != MIPS_UNSAVED ||
                                self->temps[i].state != MIPS_HAS_REG) {
                        continue;
                }
                regid=self->temps[i].regid;
                offset=self->temps[i].offset;
                if (self->temps[i].kind == MIPS_TEMP)
                        mips_function_push_back(self, mips_sw(regid, offset, MIPS_SP));
                self->temps[i].state = MIPS_NO_REG;
                self->last_temp=i+1;
                return regid;
        }
}

/*
 * Try to allocate a register from one of those free
 * which `regkind' says yes.
 */
static int mips_regalloc(struct cling_mips_function *self,
                bool (*regkind)(uint8_t regid)) {
        for (int i = 0; i < CLING_MIPS_REG_MAX; ++i)
                if (!self->reg_used[i] && regkind(i)) {
                        self->reg_used[i] = true;
                        return i;
                }
        return MIPS_REGR_NULL;
}

static uint8_t mips_function_temp_map(struct cling_mips_function *self,
                uint16_t temp, int state , int flag) 
{
        struct cling_mips_temp *temp_var;
        uint8_t regid;
        assert(temp < self->temp_size);

        temp_var = &self->temps[temp];
        if (temp_var->kind == MIPS_SAVED ||
                        temp_var->kind == MIPS_ARGREG ||
                        (temp_var->kind == MIPS_TEMP && temp_var->state == MIPS_HAS_REG))
                return temp_var->regid;

        regid = mips_function_temp_alloc(self);
        temp_var->regid = regid;
        temp_var->state = state;
        if (flag == MIPS_WRITE) {
                return regid;
        }
        mips_function_push_back(self, mips_lw(regid, temp_var->offset, MIPS_SP));
        return regid;
}

static inline uint8_t mips_function_read(struct cling_mips_function *self, int temp) {
        return mips_function_temp_map(self, temp, MIPS_HAS_REG, MIPS_READ);
}

static inline uint8_t mips_function_locked_read(struct cling_mips_function *self, int temp) {
        return mips_function_temp_map(self, temp, MIPS_LOCKED, MIPS_READ);
}

static inline uint8_t mips_function_write(struct cling_mips_function *self, int temp) {
        return mips_function_temp_map(self, temp, MIPS_HAS_REG, MIPS_WRITE);
}

/*
 * When we are in _one_ instr, the read or write will lock the the temp
 * so that is cannot be taken away but after that, they must be unlocked
 * so others can make use of the reg.
 * Note that the actual temp may be mapped to a MIPS_ARG or MPIS_SAVED
 * so only if it is MIPS_LOCKED should we "unlock" it.
 */
static void mips_temp_unlock(struct cling_mips_function *self, int temp) {
        struct cling_mips_temp *temp_var;
        temp_var=&self->temps[temp];
        if (temp_var->state != MIPS_LOCKED)
                return;
        temp_var->state = MIPS_HAS_REG;
}

/*
 * Simple find and insert.
 */
static char *mips_global_label(struct cling_mips_global *self,
                uint32_t address) {
        struct cling_mips_label *value;
        value = mips_label_find(self->labels, address);
        if (value) {
                return value->label;
        }

        mips_ir_snprintf("__label_%d", self->label_count);
        ++self->label_count;
        value = mips_label_create(buffer, address);
        utillib_hashmap_insert(self->labels, value, value);
        return value->label;
}

/*
 * Translation of different ast_ir.
 */

/*
 * Three operands calculation.
 */
static void mips_emit_ternary_calc(struct cling_mips_function *self,
                struct cling_ast_ir const *ast_ir) {
        uint8_t dest_regid;
        uint8_t src_regid1, src_regid2;
        struct cling_mips_ir *ir;

        src_regid1 = mips_function_locked_read(self, ast_ir->binop.temp1);
        src_regid2 = mips_function_locked_read(self, ast_ir->binop.temp2);
        dest_regid = mips_function_write(self, ast_ir->binop.result);
        mips_temp_unlock(self, ast_ir->binop.temp1);
        mips_temp_unlock(self, ast_ir->binop.temp2);
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
 * Calculation that takes 2 operands in mips but 3 operands
 * in ast_ir.
 * like, div $t1, $t2, mflo $t3
 */
static void mips_emit_binary_calc(struct cling_mips_function *self,
                struct cling_ast_ir const *ast_ir) {
        uint8_t src_regid1, src_regid2, dest_regid;
        struct cling_mips_ir *ir;

        src_regid1 = mips_function_locked_read(self, ast_ir->binop.temp1);
        src_regid2 = mips_function_locked_read(self, ast_ir->binop.temp2);
        dest_regid = mips_function_write(self, ast_ir->binop.result);
        mips_temp_unlock(self, ast_ir->binop.temp1);
        mips_temp_unlock(self, ast_ir->binop.temp2);
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
        mips_function_push_back(self, mips_mflo(dest_regid));
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

        base_size = ast_ir->index.base_size;
        array_regid = mips_function_locked_read(self, ast_ir->index.array_addr);
        index_regid = mips_function_locked_read(self, ast_ir->index.index_result);
        dest_regid = mips_function_write(self, ast_ir->index.result);
        mips_temp_unlock(self, ast_ir->index.array_addr);
        mips_temp_unlock(self, ast_ir->index.index_result);
        /*
         * We borrow dest_regid to hold the elemsz.
         */
        mips_function_push_back(self, mips_li(dest_regid, MIPS_WORD_SIZE));
        mips_function_push_back(self, mips_mult(index_regid, dest_regid));
        mips_function_push_back(self, mips_mflo(dest_regid));
        mips_function_push_back(self, mips_add(dest_regid, dest_regid, array_regid));
        /*
         * Now we have A+index in dest_regid.
         */
}

/*
 * Allocate an .asciiz for this string in the .data
 * segment.
 * Notes every string will have its own label. They
 * are not pooled.
 */
static char const *mips_global_asciiz(struct cling_mips_global *self,
                char const *string) {
        mips_ir_snprintf("__asciiz_%d", self->label_count);
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

        src_regid1 = mips_function_locked_read(self, relop->binop.temp1);
        src_regid2 = mips_function_read(self, relop->binop.temp2);
        mips_temp_unlock(self, relop->binop.temp1);

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
                        mips_function_push_back(self,
                                        mips_beq(src_regid1, MIPS_ZERO, ast_ir->bez.addr));
                        break;
                case OP_BNE:
                        /*
                         * branch if not equals.
                         */
                        src_regid1 = mips_function_locked_read(self, ast_ir->bne.temp1);
                        src_regid2 = mips_function_read(self, ast_ir->bne.temp2);
                        mips_function_push_back(self,
                                        mips_bne(src_regid1, src_regid2, ast_ir->bne.addr));
                        mips_temp_unlock(self, ast_ir->bne.temp1);
                        break;
                default:
                        assert(false);
        }
}

static void mips_emit_ldnam(struct cling_mips_function *self,
                struct cling_ast_ir const *ast_ir) {
        struct cling_mips_name *named_var;
        uint8_t regid=mips_function_write(self, ast_ir->ldnam.temp);
        if (ast_ir->ldnam.scope) {
                /*
                 * Local Variable
                 */
                named_var=utillib_hashmap_find(&self->names, ast_ir->ldnam.name);
                mips_function_push_back(self, ast_ir->ldnam.size == MIPS_WORD_SIZE
                                ? mips_lw(regid, named_var->offset, MIPS_SP)
                                : mips_lb(regid, named_var->offset, MIPS_SP));
                return;
        }
        /*
         * Global Variable needs a deref
         */
        mips_function_push_back(self, mips_la(regid, ast_ir->ldnam.name));
        mips_function_push_back(self, ast_ir->ldnam.size == MIPS_WORD_SIZE?
                        mips_lw(regid, 0, regid):
                        mips_lb(regid, 0, regid));
}

static void mips_emit_ldadr(struct cling_mips_function *self,
                struct cling_ast_ir const *ast_ir) {
        struct cling_mips_name *name;
        uint8_t regid=mips_function_write(self, ast_ir->ldadr.temp);
        if (ast_ir->ldadr.scope) {
                name=utillib_hashmap_find(&self->names, ast_ir->ldadr.name);
                /*
                 * This is a load of address, not a lw!
                 */
                mips_function_push_back(self, mips_addi(regid, MIPS_SP, name->offset));
                return;
        }
        mips_function_push_back(self, mips_la(regid, ast_ir->ldadr.name));
}

static void mips_emit_deref(struct cling_mips_function *self,
                struct cling_ast_ir const *ast_ir) {
        uint8_t regid=mips_function_read(self, ast_ir->deref.addr);
        mips_function_push_back(self, ast_ir->deref.size == MIPS_WORD_SIZE
                        ? mips_lw(regid, 0, regid)
                        : mips_lb(regid, 0, regid));
}

static void mips_emit_stadr(struct cling_mips_function *self,
                struct cling_ast_ir const *ast_ir) {
        uint8_t addr_regid=mips_function_locked_read(self, ast_ir->stadr.addr);
        uint8_t val_regid=mips_function_read(self, ast_ir->stadr.value);
        mips_temp_unlock(self, ast_ir->stadr.addr);
        mips_function_push_back(self, ast_ir->stadr.size == MIPS_WORD_SIZE
                        ? mips_sw(val_regid, 0, addr_regid)
                        : mips_sb(val_regid, 0, addr_regid));
}

static void mips_emit_stnam(struct cling_mips_function *self,
                struct cling_ast_ir const *ast_ir) {
        uint8_t regid=mips_function_read(self, ast_ir->stnam.value);
        struct cling_mips_name *name=utillib_hashmap_find(&self->names, ast_ir->stnam.name);
        assert(name);
        mips_function_push_back(self, ast_ir->stnam.size == MIPS_WORD_SIZE 
                        ? mips_sw(regid, name->offset, MIPS_SP)
                        : mips_sb(regid, name->offset, MIPS_SP));
}

/*
 * Those jump address needed to fixed later.
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

        syscall = cling_ast_opcode_to_syscall(ast_ir->read.kind);
        mips_function_push_back(self, mips_li(MIPS_V0, syscall));
        mips_function_push_back(self, &cling_mips_syscall);
        regid = mips_function_write(self, ast_ir->read.temp);
        mips_function_push_back(self, ast_ir->read.kind == OP_RDINT
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
        syscall = cling_ast_opcode_to_syscall(ast_ir->write.kind);
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
 * Enter with a push or call, end with the first instruction
 * after call.
 */
static int mips_emit_call(struct cling_mips_function *self, 
                struct cling_ast_ir const *ast_ir) {
        uint8_t regid;
        uint32_t offset;
        struct cling_mips_ir *ir;

        mips_temp_context(self, MIPS_SAVE);
        mips_para_context(self, MIPS_SAVE);
        for (int i=0; i<ast_ir->call.argc; ++i) {
                regid=mips_function_read(self, ast_ir->call.argv[i]);
                if (i < MIPS_REG_ARGS_N) {
                        ir = mips_move(MIPS_A0 + i, regid);
                } else {
                        offset=MIPS_MEM_ARG_BLK+i<<2; 
                        ir = mips_sw(regid, offset, MIPS_SP);
                }
                mips_function_push_back(self, ir);
        }
        mips_function_push_back(self, mips_jal(ast_ir->call.name));
        mips_temp_context(self, MIPS_LOAD);
        mips_para_context(self, MIPS_LOAD);
        if (ast_ir->call.has_result) {
                regid = mips_function_write(self, ast_ir->call.result);
                mips_function_push_back(self, mips_move(regid, MIPS_V0));
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
                        case OP_NL:
                                mips_function_push_back(self, mips_jal(MIPS_AUTO_NEWLINE));
                                break;
                        case OP_NOP:
                                break;
                        case OP_SUB:
                        case OP_ADD:
                                mips_emit_ternary_calc(self, ast_ir);
                                break;
                        case OP_DIV:
                        case OP_MUL:
                                mips_emit_binary_calc(self, ast_ir);
                                break;
                        case OP_CAL:
                                mips_emit_call(self, ast_ir);
                                break;
                        case OP_NE:
                        case OP_EQ:
                        case OP_GT:
                        case OP_GE:
                        case OP_LE:
                        case OP_LT:
                                relop = ast_ir;
                                break;
                        case OP_RET:
                                if (ast_ir->ret.has_result) {
                                        uint8_t regid = mips_function_read(self, ast_ir->ret.result);
                                        mips_function_push_back(self, mips_move(MIPS_V0, regid));
                                }
                                if (!ast_ir_useless_jump(ast_ir, ast_pc))
                                        mips_function_push_back(self, mips_j(ast_ir->ret.addr));
                                break;
                        case OP_BEZ:
                                if (ast_ir_useless_jump(ast_ir, ast_pc))
                                        break;
                                if (relop) {
                                        mips_emit_branch_relop(self, relop, ast_ir);
                                        relop = NULL;
                                } else {
                                        mips_emit_branch(self, ast_ir);
                                }
                                break;
                        case OP_BNE:
                                if (ast_ir_useless_jump(ast_ir, ast_pc))
                                        break;
                                mips_emit_branch(self, ast_ir);
                                break;
                        case OP_JMP:
                                if (ast_ir_useless_jump(ast_ir, ast_pc))
                                        break;
                                mips_emit_jmp(self, ast_ir);
                                break;
                        case OP_LDIMM:
                                mips_emit_ldimm(self, ast_ir);
                                break;
                        case OP_LDNAM:
                                mips_emit_ldnam(self, ast_ir);
                                break;
                        case OP_LDADR:
                                mips_emit_ldadr(self, ast_ir);
                                break;
                        case OP_STNAM:
                                mips_emit_stnam(self, ast_ir);
                                break;
                        case OP_STADR:
                                mips_emit_stadr(self, ast_ir);
                                break;
                        case OP_DEREF:
                                mips_emit_deref(self, ast_ir);
                                break;
                        case OP_INDEX:
                                mips_emit_index(self, ast_ir);
                                break;
                        case OP_READ:
                                mips_emit_read(self, ast_ir);
                                break;
                        case OP_WRITE:
                                mips_emit_write(self, ast_ir);
                                break;
                        case OP_LDSTR:
                                mips_emit_ldstr(self, ast_ir);
                                break;
                        default:
                                assert(false);
                }
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
        self->string = string;
        return self;
}

static void mips_data_destroy(struct cling_mips_data *self) {
        free(self->label);
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
                        fprintf(file, "%lu\n", self->extend << 2);
                        break;
        }
}

/*
 * cling_mips_program
 */

static void mips_program_emit_data(struct cling_mips_program *self,
                struct cling_ast_program const *program) {
        struct cling_ast_ir const *ast_ir;
        struct cling_mips_data *data;

        if (self->option->auto_newline) {
                utillib_vector_push_back(&self->data, mips_string_create(MIPS_NEWLINE, "\\n"));
        }
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
                                data = mips_array_create(ast_ir->defarr.name, ast_ir->defarr.extend);
                                utillib_vector_push_back(&self->data, data);
                                break;
                }
        }
}

/*
 * Write a little procedurce to print a newline
 */
static void mips_program_newline(struct cling_mips_program *self) {
        struct cling_mips_label *label;
        struct utillib_vector *text = &self->text;
        label=mips_label_create(MIPS_AUTO_NEWLINE, utillib_vector_size(text));
        utillib_hashmap_insert(&self->labels, label, label);
        utillib_vector_push_back(text, mips_la(MIPS_A0, MIPS_NEWLINE));
        utillib_vector_push_back(text, mips_li(MIPS_V0, MIPS_PRINT_STRING));
        utillib_vector_push_back(text, &cling_mips_syscall);
        utillib_vector_push_back(text, mips_jr(MIPS_RA));
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
        if (self->option->auto_newline)
                mips_program_newline(self);
}

static void mips_global_init(struct cling_mips_global *self,
                struct cling_mips_program *program) {
        self->label_count = 0;
        self->data = &program->data;
        self->labels = &program->labels;
        self->option=program->option;
}

void cling_mips_program_init(struct cling_mips_program *self, struct cling_option const *option)
{
        self->option=option;
        utillib_vector_init(&self->text);
        utillib_vector_init(&self->data);
        utillib_hashmap_init(&self->labels, &mips_label_callback);
}

void cling_mips_program_destroy(struct cling_mips_program *self) {
        utillib_vector_destroy_owning(&self->text, mips_ir_destroy);
        utillib_vector_destroy_owning(&self->data, mips_data_destroy);
        utillib_hashmap_destroy_owning(&self->labels, NULL, mips_label_destroy);
}

void cling_mips_program_emit(struct cling_mips_program *self,
                struct cling_ast_program const *program) {
        struct cling_mips_global global;
        struct cling_mips_label *label;
        struct cling_mips_function function;
        struct cling_ast_function const *ast_func;
        uint32_t address;

        mips_global_init(&global, self);
        mips_program_setup(self);
        mips_program_emit_data(self, program);

        UTILLIB_VECTOR_FOREACH(ast_func, &program->funcs) {
                address = utillib_vector_size(&self->text);
                label = mips_label_create(ast_func->name, address);
                utillib_hashmap_insert(&self->labels, label, label);
                mips_function_init(&function, &self->text, &global, ast_func);
                mips_function_emit(&function, ast_func);
                mips_function_destroy(&function);
        }
}

/*
 * This is used for real run.
 */
void cling_mips_program_print(struct cling_mips_program const *self,
                FILE *file) {
        struct cling_mips_data const *data;
        struct cling_mips_ir const *ir;
        struct cling_mips_label const *label;
        uint32_t address = 0;

        if (!utillib_vector_empty(&self->data)) {
                fputs(".data\n", file);
                UTILLIB_VECTOR_FOREACH(data, &self->data) { mips_data_print(data, file); }
        }
        fputs(".text\n", file);
        fputs(".globl main\n", file);
        UTILLIB_VECTOR_FOREACH(ir, &self->text) {
                label = mips_label_find(&self->labels, address);
                if (label) {
                        fprintf(file, "\n%s:\n", label->label);
                }
                fputs("\t", file);
                mips_ir_fprint(ir, file);
                ++address;
        }
}


void mips_function_layout_print(struct cling_mips_function const *self) {
        char const *name;
        struct cling_mips_name *entry;
        struct cling_mips_temp *temp;
        struct utillib_pair *pair;

        puts("Local Names:");
        UTILLIB_HASHMAP_FOREACH(pair, &self->names) {
                name=pair->up_first;
                entry=pair->up_second;
                switch(entry->kind) {
                        case MIPS_SAVED:
                                printf("%s saved at %s, offset is %u\n", name,
                                                cling_mips_regster_tostring(entry->regid), entry->offset);
                                break;
                        case MIPS_ARGMEM:
                                printf("%s argmem\n", name);
                                break;
                        case MIPS_ARGREG:
                                printf("%s argreg at %s\n", name, 
                                                cling_mips_regster_tostring(entry->regid));
                                break;
                        case MIPS_UNSAVED:
                                printf("%s unsaved offset is %u\n", name, entry->offset);
                                break;
                        default:
                                assert(false);
                }
        }
        puts("Temps:");
        for (int i=0; i<self->temp_size; ++i) {
                temp=&self->temps[i];
                switch(temp->kind) {
                        case MIPS_SAVED:
                                printf("t%d alias to saved %s\n", i, 
                                                cling_mips_regster_tostring(temp->regid));
                                break;
                        case MIPS_ARGMEM:
                                printf("t%d alias to argmem\n", i);
                                break;
                        case MIPS_ARGREG:
                                printf("t%d alias to argreg %s\n", i,
                                                cling_mips_regster_tostring(temp->regid));
                                break;
                        case MIPS_UNSAVED:
                                printf("t%d alias to unsaved\n", i);
                                break;
                        case MIPS_TEMP:
                                printf("t%d real temp at %u\n", i, temp->offset);
                                break;
                        default:
                                assert(false);
                }
        }
        puts("");
}

void mips_ir_print(struct cling_mips_ir const *self) {
        mips_ir_fprint(self, stdout);
}

