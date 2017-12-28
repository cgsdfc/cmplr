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
#ifndef CLING_MISC_H
#define CLING_MISC_H
#include <utillib/hashmap.h>
#include <inttypes.h>

#define MIPS_WORD_SIZE sizeof(int)
#define MIPS_BYTE_SIZE sizeof(char)

#define positive_number(X) ((X) ? (X) : 1)
#define quoted_string(X) "'%s'"

struct cling_mips_label {
  char *label;
  uint32_t address;
};

/*
 * mips_label
 */
struct cling_mips_label *mips_label_create(char const *label, uint32_t address);
void mips_label_destroy(struct cling_mips_label *self);
struct cling_mips_label * mips_label_find(struct utillib_hashmap const *self, uint32_t address);
struct cling_mips_label * mips_label_name_find(struct utillib_hashmap const *self, char const *name);
struct cling_mips_label * mips_label_find(struct utillib_hashmap const *self, uint32_t address);
extern const struct utillib_hashmap_callback mips_label_strcallback;
extern const struct utillib_hashmap_callback mips_label_callback;
extern const struct utillib_hashmap_callback cling_string_hash;

int cling_symbol_to_type(int symbol);
int symbol_to_ast_opcode(size_t symbol);
int cling_type_to_size(int type);
int cling_symbol_to_size(int symbol);
char const *size_tostring(int size);

int cling_symbol_to_immediate(int symbol, char const *string);
int cling_type_to_immediate(int type, char const *string);
int cling_ast_opcode_to_syscall(int opcode);
int cling_type_to_read(int type);
int cling_size_to_write(int size);
#endif /* CLING_MISC_H */
