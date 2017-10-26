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
static const size_t utillib_string_init_capacity = 5;
void utillib_string_clear(utillib_string *s) { s->size = 0; }
void utillib_string_erase_last(utillib_string *s) { s->size--; }

int utillib_string_init(utillib_string *s) {
  char *c_str = calloc(sizeof *c_str, utillib_string_init_capacity);
  if (!c_str) {
    return -1;
  }
  s->c_str = c_str;
  s->capacity = utillib_string_init_capacity;
  s->size = 0;
  return 0;
}

int utillib_string_reserve(utillib_string *s, size_t new_capa) {
  if (new_capa - 1 <= s->size) {
    return 0;
  }
  char *newspace = realloc(s->c_str, new_capa * sizeof *newspace);
  if (!newspace) {
    return -1;
  }
  s->c_str = newspace;
  s->capacity = new_capa;
  return 0;
}

static void utillib_string_append_aux(utillib_string *s, char x) {
  /* assert(x); */
  assert(s->size < s->capacity - 1);
  s->c_str[s->size++] = x;
}

int utillib_string_append(utillib_string *s, char x) {
  int r;
  if (s->capacity - 1 == s->size) {
    r = utillib_string_reserve(s, s->size << 1);
    if (r != 0) {
      return r;
    }
  }
  utillib_string_append_aux(s, x);
  return 0;
}

char const *utillib_string_c_str(utillib_string *s) {
  s->c_str[s->size] = 0;
  return s->c_str;
}

size_t utillib_string_size(utillib_string *s) { return s->size; }

void utillib_string_destroy(utillib_string *s) { free(s->c_str); }

bool utillib_string_empty(utillib_string *s) { return s->size == 0; }

bool utillib_string_richcmp(utillib_string *s, utillib_string *t,
                            string_cmpop op) {
#define UTILLIB_STR_CMP(S, T, OP)                                              \
  (strcmp(UTILLIB_C_STR(S), UTILLIB_C_STR(T)) OP 0)
  switch (op) {
  case STRING_EQ:
    return UTILLIB_STR_CMP(s, t, ==);
  case STRING_GT:
    return UTILLIB_STR_CMP(s, t, >);
  case STRING_GE:
    return UTILLIB_STR_CMP(s, t, >=);
  case STRING_LT:
    return UTILLIB_STR_CMP(s, t, <);
  case STRING_LE:
    return UTILLIB_STR_CMP(s, t, <=);
  case STRING_NE:
    return UTILLIB_STR_CMP(s, t, !=);
  default:
    assert(false);
  }
}
