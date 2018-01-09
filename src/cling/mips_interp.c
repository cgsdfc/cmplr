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
#include "misc.h"
#include <utillib/color.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/*
 * Map to the actual array index.
 */
#define memindex_1(address) ((address-1) >> 9)
#define memindex_2(address) ((((address-1) >> 6)) & ((1<<3)-1))
#define memoffset(address) ((address-1) & ((1<<6)-1))

static void print_memory_usage(struct cling_mips_interp const *self);

/*
 * From 1 to MIPS_MEM_MAX is valid memory address.
 */
static uint8_t *mips_mem_lookup(struct cling_mips_interp *self, int32_t address)
{
        if (0 == address) {
                self->error=MIPS_EC_NULL;
                return NULL;
        }
        if (address < 0) {
                self->error=MIPS_EC_NOMEM;
                return NULL;
        }
        if (address & 3) {
                self->error=MIPS_EC_ALIGN;
                return NULL;
        }
        struct cling_mips_memblk ** memblk_array, *memblk;
        unsigned int index;

        index=memindex_1(address);
        assert(index < MIPS_MEM_ARRAY_MAX);
        memblk_array=self->memory[index];
        if (!memblk_array) {
                memblk_array=calloc(MIPS_MEM_ARRAY_MAX,sizeof memblk_array[0]);
                self->memory[index]=memblk_array;
        }
        index=memindex_2(address);
        assert(index < MIPS_MEM_ARRAY_MAX);
        memblk=memblk_array[index];
        if (!memblk) {
                memblk=calloc( 1,sizeof *memblk);
                memblk_array[index]=memblk;
        }
        index=memoffset(address);
        if (index >= MIPS_MEMBLK_SIZE-4) {
                self->error=MIPS_EC_INDEX;
                printf("%d\n", index);
                return NULL;
        } 
        return memblk->mem + index;
}


static void mips_interp_load_label(struct cling_mips_interp *self, struct cling_mips_program const *program) {
        struct cling_mips_label const *program_label, *interp_label;
        struct utillib_pair const *pair;
        UTILLIB_HASHMAP_FOREACH(pair, &program->labels) {
                program_label=pair->up_first;
                interp_label=mips_label_create(program_label->label, program_label->address);
                utillib_hashmap_insert(&self->labels, interp_label, interp_label);
        }
}

/*
 * Memory comes directly from the stack pointer.
 * size is the number of words
 */
static uint32_t mips_interp_alloc(struct cling_mips_interp *self, size_t size) {
        self->regs[MIPS_SP]-=(size<<2);
        return self->regs[MIPS_SP];
}

static void mips_interp_load_data(struct cling_mips_interp *self, struct cling_mips_program const *program) {
        struct cling_mips_label *label;
        struct cling_mips_data *data;
        uint32_t address;

        UTILLIB_VECTOR_FOREACH(data, &program->data) {
                switch (data->type) {
                        case MIPS_SPACE:
                                address = mips_interp_alloc(self, data->extend);
                                break;
                        case MIPS_WORD:
                                address = mips_interp_alloc(self, 1);
                                break;
                        case MIPS_BYTE:
                                address = mips_interp_alloc(self, 1);
                                break;
                        case MIPS_ASCIIZ:
                                address=utillib_vector_size(&self->strings);
                                utillib_vector_push_back(&self->strings, data->string);
                                break;
                }
                label = mips_label_create(data->label, address);
                utillib_hashmap_insert(&self->labels, label, label);
        }
}

/*
 * Print with '\n'
 */
static void print_string(char const *string) {
        for (char const *p=string; *p; ++p) {
                if (*p == '\\') {
                        switch(*++p) {
                                case 'n':
                                        putchar('\n');
                                        break;
                                default: 
                                        fprintf(stderr, "warning: unsupported escaped char '%c'\n", *p);
                                        break;
                        }
                } else {
                        putchar(*p);
                }
        }
}

/*
 * Execution of different instructions.
 */
static int mips_do_syscall(struct cling_mips_interp *self) {
        int int_val;
        char char_val;
        char const *str_val;

        switch (self->regs[MIPS_V0]) {
                case MIPS_PRINT_INT:
                        printf("%d", (int)self->regs[MIPS_A0]);
                        break;
                case MIPS_PRINT_STRING:
                        str_val = utillib_vector_at(&self->strings, self->regs[MIPS_A0]);
                        print_string(str_val);
                        break;
                case MIPS_PRINT_CHAR:
                        printf("%c", (char)self->regs[MIPS_A0]);
                        break;
                case MIPS_READ_INT:
                        scanf("%d", &int_val);
                        self->regs[MIPS_V0] = int_val;
                        break;
                case MIPS_READ_CHAR:
                        /*
                         * The leading space tells scanf skip any space.
                         */
                        scanf(" %c", &char_val);
                        self->regs[MIPS_V0] = char_val;
                        break;
                case MIPS_EXIT:
                        return MIPS_EC_EXIT;
                default:
                case MIPS_READ_STRING:
                        assert(false);
        }
        return 0; 
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
        return 0;
}

static void mips_do_mflo(struct cling_mips_interp *self,
                struct cling_mips_ir const *ir) {
        uint8_t dest_regid = ir->operands[0].regid;
        self->regs[dest_regid] = self->lo;
}

static void mips_do_jal(struct cling_mips_interp *self,
                struct cling_mips_ir const *ir) {
        struct cling_mips_label const *label=mips_label_name_find(&self->labels, ir->operands[0].label);
        assert(label);
        self->regs[MIPS_RA] = self->pc + 1;
        self->pc = label->address;
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

static int mips_do_memory(struct cling_mips_interp *self, struct cling_mips_ir const *ir) {
        uint32_t address = mips_do_address(self, ir);
        uint8_t regid=ir->operands[0].regid;
        uint8_t *memory = mips_mem_lookup(self, address);
        if (!memory) {
                return 1;
        }
        switch(ir->opcode) {
                case MIPS_SW:
                        *(uint32_t*) memory=self->regs[regid];
                        break;
                case MIPS_LB:
                        self->regs[regid]=*(uint8_t*)memory;
                        break;
                case MIPS_SB:
                        *(uint8_t*) memory=self->regs[regid];
                        break;
                case MIPS_LW:
                        self->regs[regid]=*(uint32_t*)memory;
                        break;
                default:
                        assert(false); 
        }
        return 0;
}

static void mips_do_la(struct cling_mips_interp *self,
                struct cling_mips_ir const *ir) {
        struct cling_mips_label const *label;
        uint8_t dest_regid;
        dest_regid = ir->operands[0].regid;
        label=mips_label_name_find(&self->labels, ir->operands[1].label);
        assert(label);
        self->regs[dest_regid] = label->address;
}

/*
 * Value must be converted to signed int.
 */
static bool mips_cmp_zero(uint8_t opcode, int32_t value) {
        switch(opcode) {
                case MIPS_BNE:
                        return value != 0;
                case MIPS_BEQ:
                        return value == 0;
                case MIPS_BGEZ:
                        return value >= 0;
                case MIPS_BGTZ:
                        return value > 0;
                case MIPS_BLEZ:
                        return value <= 0;
                case MIPS_BLTZ:
                        return value < 0;
                default:
                        assert(false);
        }
}

static bool mips_do_branch(struct cling_mips_interp *self,
                struct cling_mips_ir const *ir) {
        struct cling_mips_label const *label;
        uint8_t src_regid1, src_regid2;
        int32_t value;
        bool cmp;
        switch(ir->opcode) {
                case MIPS_BNE:
                case MIPS_BEQ:
                        src_regid1=ir->operands[0].regid;
                        src_regid2=ir->operands[1].regid;
                        value=self->regs[src_regid1]-self->regs[src_regid2];
                        cmp=mips_cmp_zero(ir->opcode, value);
                        label=mips_label_name_find(&self->labels, ir->operands[2].label);
                        break;
                case MIPS_BGEZ:
                case MIPS_BGTZ:
                case MIPS_BLEZ:
                case MIPS_BLTZ:
                        src_regid1=ir->operands[0].regid;
                        value=self->regs[src_regid1];
                        cmp=mips_cmp_zero(ir->opcode, value);
                        label=mips_label_name_find(&self->labels, ir->operands[1].label);
                        break;
                default:
                        assert(false);
        }
        if (cmp) {
                self->pc=label->address;
                return true;
        }
        return false;
}

static void mips_do_j(struct cling_mips_interp *self, struct cling_mips_ir const *ir) {
        struct cling_mips_label const *label=mips_label_name_find(&self->labels, ir->operands[0].label);
        assert(label);
        self->pc=label->address;
}

/*
 * Three regs arith
 */
static int mips_do_arith1(struct cling_mips_interp *self,
                struct cling_mips_ir const *ir) {
        uint8_t dest_regid = ir->operands[0].regid;
        uint8_t src_regid1 = ir->operands[1].regid;
        uint8_t src_regid2 = ir->operands[2].regid;
        uint32_t result;

        switch(ir->opcode) {
                case MIPS_ADD:
                case MIPS_ADDU:
                        result=self->regs[src_regid1]+self->regs[src_regid2];
                        break;
                case MIPS_SUB:
                case MIPS_SUBU:
                        result=self->regs[src_regid1]-self->regs[src_regid2];
                        break;
                default:
                        assert(false);
        }
        self->regs[dest_regid]=result;
        return 0;
}

/*
 * div mult arith
 */
static int mips_do_arith2(struct cling_mips_interp *self,
                struct cling_mips_ir const *ir) {
        uint8_t src_regid1 = ir->operands[0].regid;
        uint8_t src_regid2 = ir->operands[1].regid;
        switch(ir->opcode) {
                case MIPS_DIV:
                        self->lo = self->regs[src_regid1] / self->regs[src_regid2];
                        break;
                case MIPS_MULT:
                        self->lo = self->regs[src_regid1] * self->regs[src_regid2];
                        break;
                default:
                        assert(false);
        }
}

int cling_mips_interp_exec(struct cling_mips_interp *self) 
{
        struct cling_mips_ir const *ir;
        const size_t instr_size = utillib_vector_size(self->instrs);

        while (true) {
fetch:
                ir = utillib_vector_at(self->instrs, self->pc); 
                switch (ir->opcode) {
                        case MIPS_NOP:
                                break;
                        case MIPS_SYSCALL:
                                if (MIPS_EC_EXIT == mips_do_syscall(self))
                                        return 0;
                                break;
                        case MIPS_MOVE:
                                mips_do_move(self, ir);
                                break;
                        case MIPS_ADDI:
                                mips_do_addi(self, ir);
                                break;
                        case MIPS_MFLO:
                                mips_do_mflo(self, ir);
                                break;
                        case MIPS_DIV:
                        case MIPS_MULT:
                                mips_do_arith2(self, ir);
                                break;
                        case MIPS_ADD:
                        case MIPS_SUB:
                                mips_do_arith1(self, ir);
                                break;
                        case MIPS_JAL:
                                mips_do_jal(self, ir);
                                goto fetch;
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
                                if (mips_do_branch(self, ir))
                                        goto fetch;
                                break;
                        case MIPS_LW:
                        case MIPS_SW:
                        case MIPS_LB:
                        case MIPS_SB:
                                if (0 != mips_do_memory(self, ir))
                                        return 1;
                                break;
                        case MIPS_LA:
                                mips_do_la(self, ir);
                                break;
                        case MIPS_LI:
                                mips_do_li(self, ir);
                                break;
                        case MIPS_ADDU:
                        case MIPS_SUBU:
                        default:
                                assert(false);
                }
                ++self->pc;
                if (self->pc >= instr_size)
                        return 0;
        }
}

static void print_memory_usage(struct cling_mips_interp const *self) {
        size_t used_memblk=0;
        for (int i=0; i<MIPS_MEM_ARRAY_MAX; ++i)
                if (self->memory[i])
                        for (int j=0; j<MIPS_MEM_ARRAY_MAX; ++j)
                                if (self->memory[i][j])
                                        ++used_memblk;
        puts("\nmemory usage:");
        printf("used:%lu\n", used_memblk*MIPS_MEMBLK_SIZE);
}

void cling_mips_interp_init(struct cling_mips_interp *self, struct cling_option const *option)
{
        self->option=option;
        self->error = 0;
        self->pc = 0;
        self->lo = 0;
        self->instrs=NULL;
        utillib_hashmap_init(&self->labels, &mips_label_strcallback);
        utillib_vector_init(&self->strings);
        memset(self->memory, 0, sizeof self->memory);
        memset(self->regs, 0, sizeof self->regs);
        self->regs[MIPS_SP]=MIPS_MEM_MAX;
}

void cling_mips_interp_load(struct cling_mips_interp *self,
                struct cling_mips_program const *program)
{
        self->instrs=&program->text;
        mips_interp_load_data(self, program);
        mips_interp_load_label(self, program);
}

void cling_mips_interp_unload(struct cling_mips_interp *self)
{



}

void cling_mips_interp_destroy(struct cling_mips_interp *self) {
        utillib_hashmap_destroy_owning(&self->labels, NULL, mips_label_destroy);
        utillib_vector_destroy(&self->strings);
        for (int i=0; i<MIPS_MEM_ARRAY_MAX; ++i) {
                if (self->memory[i])
                        for (int j=0; j<MIPS_MEM_ARRAY_MAX; ++j)
                                free(self->memory[i][j]);
                free(self->memory[i]);
        }
}

        UTILLIB_ETAB_BEGIN(cling_mips_ecode)
        UTILLIB_ETAB_ELEM(MIPS_EC_OK)
UTILLIB_ETAB_ELEM(MIPS_EC_EXIT)
        UTILLIB_ETAB_ELEM_INIT(MIPS_EC_ALIGN, "align error")
        UTILLIB_ETAB_ELEM_INIT(MIPS_EC_NULL, "null pointer error")
        UTILLIB_ETAB_ELEM_INIT(MIPS_EC_NOMEM, "no memory")
        UTILLIB_ETAB_ELEM_INIT(MIPS_EC_INDEX, "index out of range")
        UTILLIB_ETAB_END(cling_mips_ecode);

        static void mips_dump_regs(struct cling_mips_interp const *self, FILE *file) {
                char const *fmt;
                for (int i=0; i<CLING_MIPS_REG_MAX; ++i) {
                        if (self->regs[i])
                                fmt=COLOR_STRING_UNBOLD(GREEN, "0x%08"PRIX32);
                        else
                                fmt="0x%08"PRIX32;
                        fprintf(file, fmt, self->regs[i]);
                        fputs(((i+1)&7)?" ":"\n", file);
                }
        }

static void dump_regs(struct cling_mips_interp const *self) {
        mips_dump_regs(self, stdout);
}

void cling_mips_interp_print_error(struct cling_mips_interp const *self) {
        char const * errmsg=cling_mips_ecode_tostring(self->error);
        fprintf(stderr, "ERROR: %s (%d)\n", errmsg, self->error);
        fprintf(stderr, "pc=%u\n", self->pc);
        fprintf(stderr, "sp=%u\n", self->regs[MIPS_SP]);
        fputs("\t", stderr);
        mips_ir_fprint(utillib_vector_at(self->instrs, self->pc), stderr);
}

static void dump_labels(struct cling_mips_interp const *self) {
        struct utillib_pair const *pair;
        struct cling_mips_label const *label;
        UTILLIB_HASHMAP_FOREACH(pair, &self->labels) {
                label=pair->up_first;
                printf("%s:\n\t0x%08"PRIX32"\n", label->label, label->address);
        }
}
