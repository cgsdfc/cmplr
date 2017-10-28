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
#include "string.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
/**
 * \file utillib/string.c
 * Automatically resizable string.
 */

/**
 * \function utillib_string_clear
 */
void utillib_string_clear(utillib_string *self) { self->size = 0; }

/**
 * \function utillib_string_erase_last
 */
void utillib_string_erase_last(utillib_string *self) { self->size--; }

/**
 * \function utillib_string_replace_last
 */
inline void utillib_string_replace_last(utillib_string *self, char x) {
  self->c_str[self->size - 1] = x;
}

/**
 * \function utillib_string_init
 */
void utillib_string_init(utillib_string *self) {
  self->c_str = NULL;
  self->capacity = 0;
  self->size = 0;
}

void utillib_string_init_c_str(utillib_string *self, char const* str) {
  utillib_string_init(self);
  size_t len = strlen(str);
  utillib_string_reserve(self, len);
  strcpy(self->c_str, str);
}


/**
 * \function utillib_string_reserve
 */
void utillib_string_reserve(utillib_string *self, size_t new_capa) {
  if (new_capa - 1 <= self->size) {
    return;
  }
  char *str = realloc(self->c_str, new_capa * sizeof(char));
  self->c_str = str;
  self->capacity = new_capa;
}

/**
 * \function string_append_aux
 */
static inline void string_append_aux(utillib_string *self, char x) {
  self->c_str[self->size++] = x;
}

/**
 * \function utillib_string_append_char
 */
void utillib_string_append_char(utillib_string *self, char x) {
  if (self->capacity - 1 <= self->size) {
    utillib_string_reserve(self, (self->size + 1) << 1);
  }
  string_append_aux(self, x);
}

/**
 * \function utillib_string_append
 */
void utillib_string_append(utillib_string *self, char const *str) {
  size_t new_capa = self->size + strlen(str) + 1;
  utillib_string_reserve(self, new_capa);
  for (; *str; ++str) {
    string_append_aux(self, *str);
  }
}

/**
 * \function utillib_string_c_str
 */
char const *utillib_string_c_str(utillib_string *self) {
  self->c_str[self->size] = 0;
  return self->c_str;
}

/**
 * \function utillib_string_c_str
 */
size_t utillib_string_size(utillib_string *self) { return self->size; }

/**
 * \function utillib_string_capacity
 */
size_t utillib_string_capacity(utillib_string *self) { return self->capacity; }

/**
 * \function utillib_string_destroy
 */
void utillib_string_destroy(utillib_string *self) { free(self->c_str); }

/**
 * \function utillib_string_empty
 */
bool utillib_string_empty(utillib_string *self) { return self->size == 0; }

/**
 * \function utillib_string_richcmp
 */
bool utillib_string_richcmp(utillib_string *self, utillib_string *t,
                            string_cmpop op) {
#define UTILLIB_STR_CMP(self, T, OP)                                           \
  (strcmp(UTILLIB_C_STR(self), UTILLIB_C_STR(T)) OP 0)
  switch (op) {
  case STRING_EQ:
    return UTILLIB_STR_CMP(self, t, ==);
  case STRING_GT:
    return UTILLIB_STR_CMP(self, t, >);
  case STRING_GE:
    return UTILLIB_STR_CMP(self, t, >=);
  case STRING_LT:
    return UTILLIB_STR_CMP(self, t, <);
  case STRING_LE:
    return UTILLIB_STR_CMP(self, t, <=);
  case STRING_NE:
    return UTILLIB_STR_CMP(self, t, !=);
  default:
    assert(false);
  }
}
