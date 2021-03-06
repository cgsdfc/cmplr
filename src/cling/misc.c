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
#include "misc.h"
#include "ast_ir.h"
#include "mips.h"
#include "symbol_table.h"
#include "symbols.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <utillib/strhash.h>

int symbol_to_type(int symbol) {
        switch (symbol) {
                case SYM_INTEGER:
                case SYM_KW_INT:
                        return CL_INT;
                case SYM_CHAR:
                case SYM_KW_CHAR:
                        return CL_CHAR;
                case SYM_KW_VOID:
                        return CL_VOID;
                default:
                        assert(false);
        }
}

/*
 * I know these converter is ugly, but what else?
 */
int symbol_to_ast_opcode(size_t symbol) {
        int opcode;
        switch (symbol) {
                case SYM_ADD:
                        opcode = OP_ADD;
                        break;
                case SYM_MINUS:
                        opcode = OP_SUB;
                        break;
                case SYM_MUL:
                        opcode = OP_MUL;
                        break;
                case SYM_DIV:
                        opcode = OP_DIV;
                        break;
                case SYM_DEQ:
                        opcode = OP_EQ;
                        break;
                case SYM_NE:
                        opcode = OP_NE;
                        break;
                case SYM_LT:
                        opcode = OP_LT;
                        break;
                case SYM_LE:
                        opcode = OP_LE;
                        break;
                case SYM_GT:
                        opcode = OP_GT;
                        break;
                case SYM_GE:
                        opcode = OP_GE;
                        break;
                default:
                        assert(false);
        }
        return opcode;
}

char const *size_tostring(int size) {
        switch (size) {
                case MIPS_WORD_SIZE:
                        return "int";
                case MIPS_BYTE_SIZE:
                        return "char";
                default:
                        return "void";
        }
}

int symbol_to_size(int symbol) {
        switch (symbol) {
                case SYM_INTEGER:
                        return MIPS_WORD_SIZE;
                case SYM_CHAR:
                        return MIPS_BYTE_SIZE;
                default:
                        assert(false);
        }
}
int type_to_size(int type) {
        switch (type) {
                case CL_INT:
                        return MIPS_WORD_SIZE;
                case CL_CHAR:
                        return MIPS_BYTE_SIZE;
                case CL_VOID:
                        return 0;
                default:
                        assert(false);
        }
}

int symbol_to_immediate(int symbol, char const *string) {
        int int_val;
        char char_val;
        switch (symbol) {
                case SYM_CHAR:
                case SYM_KW_CHAR:
                        sscanf(string, "%c", &char_val);
                        return char_val;
                case SYM_INTEGER:
                case SYM_KW_INT:
                        sscanf(string, "%d", &int_val);
                        return int_val;
                default:
                        assert(false);
        }
}

int type_to_immediate(int type, char const *string) {
        int int_val;
        char char_val;
        switch (type) {
                case CL_CHAR:
                        sscanf(string, "%c", &char_val);
                        return char_val;
                case CL_INT:
                        sscanf(string, "%d", &int_val);
                        return int_val;
                default:
                        assert(false);
        }
}

const struct utillib_hashmap_callback string_hash = {
        .hash_handler = mysql_strhash, .compare_handler = strcmp,
};

int ast_opcode_to_syscall(int opcode) {
        switch (opcode) {
                case OP_RDCHR:
                        return MIPS_READ_CHAR;
                case OP_RDINT:
                        return MIPS_READ_INT;
                case OP_WRCHR:
                        return MIPS_PRINT_CHAR;
                case OP_WRSTR:
                        return MIPS_PRINT_STRING;
                case OP_WRINT:
                        return MIPS_PRINT_INT;
                default:
                        assert(false);
        }
}

int type_to_read(int type) {
        switch (type) {
                case CL_CHAR:
                        return OP_RDCHR;
                case CL_INT:
                        return OP_RDINT;
                default:
                        assert(false);
        }
}

int size_to_write(int size) {
        switch (size) {
                case MIPS_WORD_SIZE:
                        return OP_WRINT;
                case MIPS_BYTE_SIZE:
                        return OP_WRCHR;
                default:
                        assert(false);
        }
}

/*
 * Label Management.
 */

struct mips_label *mips_label_create(char const *label,
                uint32_t address) {
        struct mips_label *self = malloc(sizeof *self);
        self->label = strdup(label);
        self->address = address;
        return self;
}

void mips_label_destroy(struct mips_label *self) {
        free(self->label);
        free(self);
}

/*
 * Address as key.
 */
static int mips_label_compare(struct mips_label const *lhs,
                struct mips_label const *rhs) {
        return lhs->address - rhs->address;
}

static size_t mips_label_hash(struct mips_label const *self) {
        return self->address;
}

const struct utillib_hashmap_callback mips_label_callback = {
        .compare_handler = mips_label_compare, .hash_handler = mips_label_hash,
};

struct mips_label *
mips_label_find(struct utillib_hashmap const *self, uint32_t address) {
        struct mips_label key;
        key.address = address;
        return utillib_hashmap_find(self, &key);
}

struct mips_label *
mips_label_name_find(struct utillib_hashmap const *self, char const *name) {
        struct mips_label key;
        key.label = name;
        return utillib_hashmap_find(self, &key);
}

static int mips_label_strcmp(struct mips_label const *lhs,
                struct mips_label const *rhs) {
        return strcmp(lhs->label, rhs->label);
}

static size_t mips_label_strhash(struct mips_label const *self) {
        return mysql_strhash(self->label);
}

const struct utillib_hashmap_callback mips_label_strcallback = {
        .hash_handler = mips_label_strhash, .compare_handler = mips_label_strcmp,
};
