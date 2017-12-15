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

#define MIPS_WORD_SIZE sizeof(int)
#define MIPS_BYTE_SIZE sizeof(char)

/*
 * This union is **untyped**
 * so all its usage is based
 * on context and it cannot hold
 * memory on heap.
 */
union cling_primary {
  char signed_char;
  char const *string;
  int signed_int;
  unsigned int unsigned_int;
};

/*
 * Makes a bitwise copy of self and put
 * it on the heap.
 */
union cling_primary *cling_primary_copy(union cling_primary const *self);

/*
 * Hashes the signed_int field.
 */
size_t cling_primary_inthash(union cling_primary const *lhs);

/*
 * Compares the signed_int field of 2 cling_primary.
 */
int cling_primary_intcmp(union cling_primary const *lhs,
                         union cling_primary const *rhs);

/*
 * Converts from `rawstr' to appropriate cling_primary
 * according to `type'.
 */
void cling_primary_init(union cling_primary *self, size_t type,
                        char const *rawstr);
/*
 * As long as cling_primary holds integral
 * value, converts it to int according to
 * `type' and stores it back into `self'.
 */
void cling_primary_toint(union cling_primary *self, size_t type);
int cling_symbol_to_type(int symbol);
int cling_type_to_wide(int type);
int symbol_to_ast_opcode(size_t symbol);
int cling_type_to_size(int type);
int cling_symbol_to_size(int symbol);
char const *size_tostring(int size);

int cling_symbol_to_immediate(int symbol, char const *string);
int cling_type_to_immediate(int type, char const *string);
int cling_ast_opcode_to_syscall(int opcode);
int cling_type_to_write(int type);
int cling_type_to_read(int type);

extern const struct utillib_hashmap_callback cling_string_hash;

#endif /* CLING_MISC_H */
