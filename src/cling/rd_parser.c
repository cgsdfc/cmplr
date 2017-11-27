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

#include "rd_parser.h"
#include "rd_parser_impl.h"
#include "symbols.h"
#include <utillib/json.h>
#include <utillib/scanner.h>

#include <assert.h>
#include <stdlib.h>

/**
 * File cling/rd_parser.c
 * All the recursive decent subroutines
 * go here.
 */

static struct utillib_json_value *
scanf_stmt(struct cling_rd_parser *self, struct utillib_token_scanner *input);

static struct utillib_json_value *var_defs(struct cling_rd_parser *self,
                                           struct utillib_token_scanner *input,
                                           char const *first_iden);

static struct utillib_json_value *program(struct cling_rd_parser *self,
                                          struct utillib_token_scanner *input);

/*
 * init/destroy
 */
void cling_rd_parser_init(struct cling_rd_parser *self) {
  utillib_vector_init(&self->elist);
}

void cling_rd_parser_destroy(struct cling_rd_parser *self) {
  utillib_vector_destroy_owning(&self->elist, free);
}

/**
 * \function cling_rd_parser_parse
 * Parses the `input' using recursive decent method.
 * If the input somehow represents something recognizable
 * returns a non-NULL value which can be further analyzed.
 * However, if a fatal error is detected when the recursion
 * if quite deep or there is no meaningful recovery, it returns
 * NULL so the caller should wait for nothing but print errors
 * and terminate the program.
 * If NULL is returned, memory is probably leaked.
 */
struct utillib_json_value *
cling_rd_parser_parse(struct cling_rd_parser *self,
                      struct utillib_token_scanner *input) {
  int code;
  switch (code = setjmp(self->fatal_saver)) {
  case 0:
    return program(self, input);
  default:
#ifndef NDEBUG
    printf("@@ longjmp from `%s' context @@\n", cling_symbols[code].name);
#endif
    return NULL;
  }
}

void cling_rd_parser_report_errors(struct cling_rd_parser const *self) {
  UTILLIB_VECTOR_FOREACH(struct rd_parser_error const *, error,
                         &self->elist) {
    rd_parser_error_print(error);
  }
}

/*
 * Recursive Decent Subroutines
 */

/**
 * \function const_int_defs
 * Handles const integer definitions.
 * examples:
 * i=0
 * i=0, j=1, k=2
 * context: SYM_CONST_DEF
 * skipto: SYM_SEMI
 * return array of object{identifier:string, initilizer:size_t}
 * lookahead: SYM_KW_CHAR | SYM_KW_INT
 */
static struct utillib_json_value *
const_defs(struct cling_rd_parser *self, struct utillib_token_scanner *input,
           size_t expected_initializer) {
  char const *str;
  size_t expected;
  size_t initilizer;
  struct utillib_json_value *object;
  size_t code;
  struct utillib_json_value *array = utillib_json_array_create_empty();
  struct utillib_symbol const *context = &cling_symbols[SYM_CONST_DEF];

  /* Main loop */
  while (true) {
    object = utillib_json_object_create_empty();
    utillib_json_array_push_back(array, object);
    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_IDEN) {
      expected = SYM_IDEN;
      goto error;
    }
    str = utillib_token_scanner_semantic(input);
    utillib_token_scanner_shiftaway(input);
    utillib_json_object_push_back(object, "identifier",
                                  utillib_json_string_create(&str));
    free(str);

    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_EQ) {
      expected = SYM_EQ;
      goto error;
    }
    utillib_token_scanner_shiftaway(input);

    code = utillib_token_scanner_lookahead(input);
    if (code != expected_initializer) {
      expected = expected_initializer;
      goto error;
    }
    initilizer = utillib_token_scanner_semantic(input);
    utillib_token_scanner_shiftaway(input);
    utillib_json_object_push_back(object, "initilizer",
                                  utillib_json_size_t_create(&initilizer));

    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_COMMA)
      return array;
    utillib_token_scanner_shiftaway(input);
  }
/* Error handling */
error:
  utillib_vector_push_back(&self->elist, rd_parser_expected_error(
                                             input, &cling_symbols[expected],
                                             cling_symbol_cast(code), context));
  switch (expected) {
  /* Patches this failure a little bit */
  case SYM_IDEN:
    utillib_json_object_push_back(object, "identifier",
                                  utillib_json_null_create());
  /* Falls through */
  case SYM_UINT:
  case SYM_CHAR:
  case SYM_EQ:
    utillib_json_object_push_back(object, "initilizer",
                                  utillib_json_null_create());
    break;
  }
  if (rd_parser_skipto(input, SYM_SEMI)) {
    longjmp(self->fatal_saver, context->value);
  }
  return array;
}

/**
 * \function single_const_decl
 * Handles a single piece of const declaration.
 * examples:
 * const int i=0;
 * const int i=0, j=1, k=2;
 * const char c='ch';
 * lookahead: SYM_KW_CONST
 * context: SYM_CONST_DECL
 * skipto: SYM_SEMI
 * return: object{type=size_t(SYM_KW_INT|SYM_KW_CHAR),
 * const_decl=array}
 *
 */
static struct utillib_json_value *
single_const_decl(struct cling_rd_parser *self,
                  struct utillib_token_scanner *input) {
  size_t code = utillib_token_scanner_lookahead(input);
  size_t expected;
  utillib_token_scanner_shiftaway(input);
  assert(code == SYM_KW_CONST);

  struct utillib_json_value *const_decl = utillib_json_object_create_empty();
  code = utillib_token_scanner_lookahead(input);
  struct utillib_symbol const *context = &cling_symbols[SYM_CONST_DECL];
  struct utillib_symbol const *expected_initializer;

  switch (code) {
  /* Similar handling */
  case SYM_KW_CHAR:
  case SYM_KW_INT:
    utillib_token_scanner_shiftaway(input);
    expected_initializer = &cling_symbols[code];
    utillib_json_object_push_back(const_decl, "type",
                                  utillib_json_size_t_create(&code));
    utillib_json_object_push_back(
        const_decl, "const_defs",
        const_defs(self, input, code == SYM_KW_INT ? SYM_UINT : SYM_CHAR));
    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_SEMI) {
      expected = SYM_SEMI;
      goto error;
    }
    utillib_token_scanner_shiftaway(input);
    return const_decl;
  default:
    expected = SYM_TYPENAME;
    goto error;
  }
error:
  utillib_vector_push_back(&self->elist, rd_parser_expected_error(
                                             input, &cling_symbols[expected],
                                             cling_symbol_cast(code), context));
  if (rd_parser_skipto(input, SYM_SEMI)) {
    longjmp(self->fatal_saver, context->value);
  }
  utillib_token_scanner_shiftaway(input); // SYM_SEMI
  /* Patches a little bit */
  switch (expected) {
  case SYM_TYPENAME:
    /* Without typename what is left is null */
    utillib_json_value_destroy(const_decl);
    return utillib_json_null_create();
  case SYM_SEMI:
    return const_decl;
  }
}

/**
 * \function multiple_const_decl
 * Recogizes multiple const_decls.
 * lookahead: SYM_KW_CONST
 * return: array of const_decl
 * skipto: nothing
 */
static struct utillib_json_value *
multiple_const_decl(struct cling_rd_parser *self,
                    struct utillib_token_scanner *input) {
  size_t code = utillib_token_scanner_lookahead(input);
  assert(code == SYM_KW_CONST);
  struct utillib_json_value *array = utillib_json_array_create_empty();
  while (true) {
    utillib_json_array_push_back(array, single_const_decl(self, input));
    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_KW_CONST)
      return array;
  }
}
/**
 * \function scanf_stmt
 * return: object{type:size_t=SYM_SCANF_STMT,
 * iden_list:nonempty array of identifier}
 */

static struct utillib_json_value *
scanf_stmt(struct cling_rd_parser *self, struct utillib_token_scanner *input) {
  size_t code = utillib_token_scanner_lookahead(input);
  utillib_token_scanner_shiftaway(input);
  assert(code == SYM_KW_SCANF);

  struct utillib_json_value *object = utillib_json_object_create_empty();
  struct utillib_symbol const *context = &cling_symbols[SYM_SCANF_STMT];
  utillib_json_object_push_back(object, "type",
                                utillib_json_size_t_create(&code));
  struct utillib_json_value *array = utillib_json_array_create_empty();
  utillib_json_object_push_back(object, "iden_list", array);
  size_t expected;
  char const *str;
  size_t skipto;

  /* scanf( */
  if (utillib_token_scanner_lookahead(input) != SYM_LP) {
    expected = SYM_LP;
    goto error;
  }
  utillib_token_scanner_shiftaway(input);

  /* scanf(iden */
  code = utillib_token_scanner_lookahead(input);
  if (code == SYM_RP) {
    /* Empty arglist case */
    utillib_token_scanner_shiftaway(input);
    utillib_json_value_destroy(object);
    utillib_vector_push_back(&self->elist,
                             rd_parser_noargs_error(input, "scanf"));
    return utillib_json_null_create();
  }

  if (code != SYM_IDEN) {
    expected = SYM_IDEN;
    goto error;
  }

  while (true) {
    str = utillib_token_scanner_semantic(input);
    utillib_token_scanner_shiftaway(input);
    utillib_json_array_push_back(array, utillib_json_string_create(&str));
    free(str);
    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_COMMA)
      break;
    utillib_token_scanner_shiftaway(input);
    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_IDEN) {
      expected = SYM_IDEN;
      goto error;
    }
  }
  if (code != SYM_RP) {
    expected = SYM_RP;
    goto error;
  }
  utillib_token_scanner_shiftaway(input);
  /* Success */
  return object;

error:
  /* Expect error */
  utillib_vector_push_back(&self->elist, rd_parser_expected_error(
                                             input, &cling_symbols[expected],
                                             cling_symbol_cast(code), context));
  switch (expected) {
  case SYM_LP:
    skipto = SYM_SEMI;
    break;
  case SYM_IDEN:
    skipto = SYM_RP;
    break;
  case SYM_RP:
    skipto = SYM_SEMI;
    break;
  default:
    assert(false && "one expected was left out");
  }
  if (rd_parser_skipto(input, skipto)) {
    longjmp(self->fatal_saver, context->value);
  }
  return object;
}

/**
 * lookahead: [ , ;
 * examples:
 * i,j,k
 * i[10], i, k[10]
 * return: array of object{type:(SYM_KW_INT|SYM_KW_CHAR),
 * is_array:bool, extend:size_t}
 */
static struct utillib_json_value *var_defs(struct cling_rd_parser *self,
                                           struct utillib_token_scanner *input,
                                           char const *first_iden) {
  size_t code;
  size_t expected;
  struct utillib_json_value *array = utillib_json_array_create_empty();
  struct utillib_json_value *object = utillib_json_object_create_empty();
  utillib_json_object_push_back(object, "identifier",
                                utillib_json_string_create(&first_iden));
  utillib_json_array_push_back(array, object);
  free(first_iden);

  while (true) {
    bool is_array = false;
    size_t extend;
    code = utillib_token_scanner_lookahead(input);
    switch (code) {
    /* Those that can follow iden are SYM_LK and SYM_COMMA*/
    case SYM_LK:
      is_array = true;
      utillib_json_object_push_back(object, "is_array",
                                    utillib_json_bool_create(&is_array));
      utillib_token_scanner_shiftaway(input);
      code = utillib_token_scanner_lookahead(input);
      if (code != SYM_UINT) {
        expected = SYM_UINT;
        goto error;
      }
      extend = utillib_token_scanner_semantic(input);
      utillib_json_object_push_back(object, "extend",
                                    utillib_json_size_t_create(&extend));
      utillib_token_scanner_shiftaway(input);
      code = utillib_token_scanner_lookahead(input);
      if (code != SYM_RK) {
        expected = SYM_RK;
        goto error;
      }
      utillib_token_scanner_shiftaway(input);
      break;
    case SYM_COMMA:
      object = utillib_json_object_create_empty();
      utillib_json_array_push_back(array, object);
      utillib_token_scanner_shiftaway(input);
      code = utillib_token_scanner_lookahead(input);
      if (code != SYM_IDEN) {
        expected = SYM_IDEN;
        goto error;
      }
      first_iden = utillib_token_scanner_semantic(input);
      utillib_json_object_push_back(object, "identifier",
                                    utillib_json_string_create(&first_iden));
      free(first_iden);
      utillib_token_scanner_shiftaway(input);
      break;
    default:
      return array;
    }
  }
error:
  utillib_vector_push_back(&self->elist, rd_parser_expected_error(
                                             input, &cling_symbols[expected],
                                             cling_symbol_cast(code),
                                             &cling_symbols[SYM_VAR_DEF]));
  if (rd_parser_skipto(input, SYM_SEMI))
    longjmp(self->fatal_saver, SYM_VAR_DEF);
  if (expected == SYM_UINT)
    utillib_json_object_push_back(object, "extend", utillib_json_null_create());
  if (expected == SYM_IDEN)
    utillib_json_object_push_back(object, "identifier",
                                  utillib_json_null_create());
  return array;
}

static struct utillib_json_value *
singel_var_decl(struct cling_rd_parser *self,
                struct utillib_token_scanner *input, size_t type,
                char const *first_iden) {
  struct utillib_json_value *object = utillib_json_object_create_empty();
  size_t code;

  utillib_json_object_push_back(object, "type",
                                utillib_json_size_t_create(&type));
  utillib_json_object_push_back(object, "var_defs",
                                var_defs(self, input, first_iden));
  code = utillib_token_scanner_lookahead(input);
  if (code != SYM_SEMI) {
    utillib_vector_push_back(&self->elist, rd_parser_expected_error(
                                               input, &cling_symbols[SYM_SEMI],
                                               cling_symbol_cast(code),
                                               &cling_symbols[SYM_VAR_DECL]));
    return object;
  }
  utillib_token_scanner_shiftaway(input);
  return object;
}

static struct utillib_json_value *
maybe_multiple_var_decls(struct cling_rd_parser *self,
                         struct utillib_token_scanner *input, size_t *type,
                         char const **first_iden) {
  size_t code;
  size_t expected;
  bool has_var_decl = false;
  struct utillib_json_value *array = utillib_json_array_create_empty();
  code = utillib_token_scanner_lookahead(input);
  assert(code == SYM_KW_INT || code == SYM_KW_CHAR);
  *type = code;
  utillib_token_scanner_shiftaway(input);

  while (true) {
    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_IDEN) {
      expected = SYM_IDEN;
      goto error;
    }
    *first_iden = utillib_token_scanner_semantic(input);
    utillib_token_scanner_shiftaway(input);
    code = utillib_token_scanner_lookahead(input);
    switch (code) {
    case SYM_LP:
      goto maybe_return_array;
    case SYM_LK:
    case SYM_COMMA:
    case SYM_SEMI:
      has_var_decl = true;
      utillib_json_array_push_back(
          array, singel_var_decl(self, input, *type, *first_iden));
      code = utillib_token_scanner_lookahead(input);
      if (code != SYM_KW_INT && code != SYM_KW_CHAR)
        return array;
      *type = code;
      utillib_token_scanner_shiftaway(input);
      break;
    default:
      utillib_vector_push_back(&self->elist, rd_parser_unexpected_error(
                                                 input, cling_symbol_cast(code),
                                                 &cling_symbols[SYM_VAR_DECL]));
      goto skip;
    }
  }
maybe_return_array:
  if (has_var_decl)
    return array;
  utillib_json_value_destroy(array);
  return utillib_json_null_create();
error:
  utillib_vector_push_back(&self->elist, rd_parser_expected_error(
                                             input, &cling_symbols[expected],
                                             cling_symbol_cast(code),
                                             &cling_symbols[SYM_VAR_DECL]));
skip:
  if (rd_parser_skipto(input, SYM_SEMI))
    longjmp(self->fatal_saver, SYM_VAR_DECL);
  goto maybe_return_array;
}

static struct utillib_json_value *program(struct cling_rd_parser *self,
                                          struct utillib_token_scanner *input) {
  size_t type;
  char const *first_iden;
  struct utillib_json_value *val =
      maybe_multiple_var_decls(self, input, &type, &first_iden);
  if (val == &utillib_json_null) {
    printf("first_iden %s, type %s\n", first_iden, cling_symbols[type].name);
  }
  return val;
}
