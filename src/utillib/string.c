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
#define _GNU_SOURCE
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
 * Removes all the chars of self but does not free memory.
 * Self becomes empty after that.
 */
void utillib_string_clear(struct utillib_string *self) { self->size = 0; }

/**
 * \function utillib_string_erase_last
 * Removes the last char from self.
 * If self is empty, the behaviour is undefined.
 */
void utillib_string_erase_last(struct utillib_string *self) { self->size--; }

/**
 * \function utillib_string_replace_last
 */
inline void utillib_string_replace_last(struct utillib_string *self, char x) {
  self->c_str[self->size - 1] = x;
}

/**
 * \function utillib_string_init
 * Initilizes self to be an empty string having the same content
 * as `""'.
 * Both size and capacity are zero.
 */
void utillib_string_init(struct utillib_string *self) {
  self->c_str = NULL;
  self->capacity = 0;
  self->size = 0;
}

/**
 * \function utillib_string_init_c_str
 * Initilizes to have the same content of a C str.
 * Ensures `size' + 1 <= `capacity'.
 * and `size' == strlen(str).
 */
void utillib_string_init_c_str(struct utillib_string *self, char const *str) {
  utillib_string_init(self);
  size_t len = strlen(str);
  utillib_string_reserve(self, len + 1);
  strcpy(self->c_str, str);
  self->size = len;
}

/**
 * \function utillib_string_reserve
 * Make room for self and set `capacity' to `new_capa'
 * if the invariant `size + 1 <= capacity' is respected
 * by `new_capa'.
 * Otherwise, it does nothing.
 */
void utillib_string_reserve(struct utillib_string *self, size_t new_capa) {
  if (new_capa <= self->capacity) {
    return;
  }
  char *str = realloc(self->c_str, new_capa * sizeof(char));
  self->c_str = str;
  self->capacity = new_capa;
}

/**
 * \function string_append_aux
 * Append a single char to self without checking.
 */
static inline void string_append_aux(struct utillib_string *self, char x) {
  self->c_str[self->size++] = x;
}

/**
 * \function utillib_string_append_char
 * Append a single char to self and grows in 2's power.
 */
void utillib_string_append_char(struct utillib_string *self, char x) {
  if (self->capacity < self->size + 2) {
    utillib_string_reserve(self, (self->size + 1) << 1);
  }
  string_append_aux(self, x);
}

/**
 * \function utillib_string_append
 */
void utillib_string_append(struct utillib_string *self, char const *str) {
  size_t new_capa = self->size + strlen(str) + 1;
  utillib_string_reserve(self, new_capa);
  for (; *str; ++str) {
    string_append_aux(self, *str);
  }
}

/**
 * \function utillib_string_c_str
 * Returns a RO null-terminated C string of self.
 * If self is empty, the returned value equals to "".
 */
char const *utillib_string_c_str(struct utillib_string *self) {
  if (!self->c_str) {
    return "";
  }
  self->c_str[self->size] = 0;
  return self->c_str;
}

/**
 * \function utillib_string_size
 * \return size.
 */
size_t utillib_string_size(struct utillib_string *self) { return self->size; }

/**
 * \function utillib_string_capacity
 * \return capacity. Self can hold at most `capacity-1' chars.
 * If it returns zero, it means self is empty and can hold no char at all.
 */
size_t utillib_string_capacity(struct utillib_string *self) {
  return self->capacity;
}

/**
 * \function utillib_string_destroy
 */
void utillib_string_destroy(struct utillib_string *self) {
  free(self->c_str);
  self->c_str = NULL;
}

/**
 * \function utillib_string_empty
 */
bool utillib_string_empty(struct utillib_string *self) {
  return self->size == 0;
}

/**
 * \function utillib_string_richcmp
 * Returns yes if `self' and `t' satisfy the relation `op'.
 */
bool utillib_string_richcmp(struct utillib_string *self,
                            struct utillib_string *t, enum string_cmpop op) {
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
  }
}

/**
 * \function utillib_string_fmemopen
 * Opens in read mode to allow stream-like operation.
 * Client should close the returned pointer.
 */

FILE *utillib_string_fmemopen(struct utillib_string *self) {
  char const *buf = utillib_string_c_str(self);
  return fmemopen((void *)buf, self->size, "r");
}
