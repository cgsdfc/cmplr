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
#include "mips.h"
#include "ast_ir.h"
#include "symbol_table.h"
#include "symbols.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <utillib/strhash.h>

int cling_symbol_to_type(int symbol) {
  switch (symbol) {
  case SYM_INTEGER:
  case SYM_UINT:
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

const struct utillib_hashmap_callback cling_string_hash = {
    .hash_handler = mysql_strhash, .compare_handler = strcmp,
};

union cling_primary *cling_primary_copy(union cling_primary const *self) {
  union cling_primary *other = malloc(sizeof *other);
  memcpy(other, self, sizeof *other);
  return other;
}

inline size_t cling_primary_inthash(union cling_primary const *lhs) {
  return lhs->signed_int;
}

inline int cling_primary_intcmp(union cling_primary const *lhs,
                                union cling_primary const *rhs) {
  return lhs->signed_int - rhs->signed_int;
}

void cling_primary_toint(union cling_primary *self, size_t type) {
  switch (type) {
  case SYM_UINT:
    self->signed_int = (int)self->unsigned_int;
    break;
  case SYM_CHAR:
    self->signed_int = (char)self->signed_char;
    break;
  case SYM_INTEGER:
    break;
  }
}

void cling_primary_init(union cling_primary *self, size_t type,
                        char const *rawstr) {
  switch (type) {
  case SYM_UINT:
    sscanf(rawstr, "%u", &self->unsigned_int);
    return;
  case SYM_INTEGER:
    sscanf(rawstr, "%d", &self->signed_int);
    return;
  case SYM_STRING:
    self->string = rawstr;
    return;
  case SYM_CHAR:
    sscanf(rawstr, "%c", &self->signed_char);
    return;
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
    opcode=OP_ADD;
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
    cling_default_assert(symbol, cling_symbol_kind_tostring);
  }
  return opcode;
}

char const *size_tostring(int size) {
  switch(size) {
    case MIPS_WORD_SIZE:
      return "int";
    case MIPS_BYTE_SIZE:
      return "char";
    default:
      return "void";
  }
}


int cling_symbol_to_size(int symbol) {
  switch(symbol) {
    case SYM_INTEGER:
    case SYM_UINT:
      return MIPS_WORD_SIZE;
    case SYM_CHAR:
      return MIPS_BYTE_SIZE;
    default:
      cling_default_assert(symbol, cling_symbol_entry_kind_tostring);
  }
}
int cling_type_to_size(int type) {
  switch(type) {
    case CL_INT:
      return MIPS_WORD_SIZE;
    case CL_CHAR:
      return MIPS_BYTE_SIZE;
    case CL_VOID:
      return 0;
    default:
      cling_default_assert(type, cling_symbol_entry_kind_tostring);
  }
}

int cling_symbol_to_immediate(int symbol, char const *string) {
  int int_val;
  char char_val;
  switch(symbol) {
    case SYM_CHAR:
    case SYM_KW_CHAR:
      sscanf(string, "%c", &char_val);
      return char_val;
    case SYM_INTEGER:
    case SYM_KW_INT:
    case SYM_UINT:
      sscanf(string, "%d", &int_val);
      return int_val;
    default:
      assert(false);
  }
}

int cling_type_to_immediate(int type, char const *string){
  int int_val;
  char char_val;
  switch(type) {
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

