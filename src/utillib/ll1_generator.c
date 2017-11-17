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

#include "ll1_generator.h"
#include "json.h"
#include "ll1_builder.h"
#include "ll1_builder_impl.h"
#include "print.h"
#include "rule.h"
#include "string.h"
#include <assert.h>

/**
 * \file utillib/ll1_builder.c
 * Frontend of the Utillib.LL(1).
 */

static bool ll1_generator_check_table(struct utillib_ll1_generator *self) {
  struct utillib_ll1_builder *builder = &self->builder;
  size_t error_size = utillib_ll1_builder_check(builder);
  if (0 == error_size)
    return true;
  utillib_ll1_builder_print_errors(builder);
  return false;
}

/**
 * \function ll1_generator_table_tostring
 * Fills in a string with the LL(1) table in C
 * `int[][]' format.
 */

static void ll1_generator_write_table(struct utillib_ll1_generator *self,
                                      FILE *file) {
  struct utillib_rule_index const *rule_index = &self->rule_index;

  fputs("static int ll1_parser_table[]={\n", file);

  for (int i = 0; i < rule_index->non_terminals_size; ++i) {
    fputs("\t", file);
    for (int j = 0; j < rule_index->terminals_size; ++j) {
      struct utillib_rule const *rule = utillib_vector2_at(&self->table, i, j);
      int rule_id;
      if (NULL == rule) {
        rule_id = UT_RULE_NULL;
      } else if (rule == UTILLIB_RULE_EPS) {
        rule_id = UT_RULE_EPS;
      } else {
        rule_id = rule->id;
      }
      fprintf(file, "%4d, ", rule_id);
    }
    fputs("\n", file);
  }
  fputs("};\n", file);
}

static void ll1_generator_write(struct utillib_ll1_generator *self,
                                FILE *file) {
  fputs(utillib_LICENSE_str, file);
  fputs("\n", file);
  fputs("/* LL(1) parser table */\n"
        "/* Generated by Utillib.LL(1). DONNOT EDIT !*/\n",
        file);
  ll1_generator_write_table(self, file);
}

void utillib_ll1_generator_init_from_code(
    struct utillib_ll1_generator *self, struct utillib_symbol const *symbols,
    struct utillib_rule_literal const *rules) {
  self->builder_val = NULL;
  utillib_rule_index_init(&self->rule_index, symbols, rules);
  utillib_ll1_builder_init(&self->builder, &self->rule_index);
  utillib_ll1_builder_build_table(&self->builder, &self->table);
}

bool utillib_ll1_generator_generate(struct utillib_ll1_generator *self,
                                    const char *filename) {
  if (!ll1_generator_check_table(self))
    return false;
  FILE *file = fopen(filename, "w");
  if (!file) {
    perror("ERROR");
    return false;
  }
  ll1_generator_write(self, file);
  fclose(file);
  return true;
}

void utillib_ll1_generator_dump_set(struct utillib_ll1_generator *self,
                                    int kind, int symbol_id) {
  if (symbol_id <= 0) {
    utillib_error_printf("ERROR: Negative symbol_id `%d' has no LL1 set\n",
                         symbol_id);
    return;
  }
  struct utillib_symbol const *symbol =
      utillib_rule_index_symbol_at(&self->rule_index, symbol_id);
  if (symbol->kind != UT_SYMBOL_NON_TERMINAL) {
    utillib_error_printf(
        "ERROR: Only non terminal symbols have FIRST/FOLLOW\n");
    return;
  }
  struct utillib_json_value_t *val =
      ll1_builder_set_json_object_create(&self->builder, kind, symbol);
  utillib_json_pretty_print(val, stdout);
  utillib_json_value_destroy(val);
}

void utillib_ll1_generator_dump_all(struct utillib_ll1_generator *self) {
  if (!self->builder_val) {
    self->builder_val = utillib_ll1_builder_json_object_create(&self->builder);
  }
  utillib_json_pretty_print(self->builder_val, stdout);
}

void utillib_ll1_generator_destroy(struct utillib_ll1_generator *self) {
  utillib_rule_index_destroy(&self->rule_index);
  utillib_ll1_builder_destroy(&self->builder);
  utillib_vector2_destroy(&self->table);
  if (self->builder_val) {
    utillib_json_value_destroy(self->builder_val);
  }
}
