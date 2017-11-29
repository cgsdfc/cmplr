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
#include "symbols.h"
#include "error.h"
#include "symbol_table.h"
#include "opg_parser.h"
#include "ir.h"
#include "ast.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

/**
 * File cling/rd_parser.c
 * All the recursive decent subroutines
 * go here.
 */

static void rd_parser_fatal(struct cling_rd_parser *self)
{
  longjmp(self->fatal_saver, self->context);
}

static void rd_parser_skip_target_init(struct cling_rd_parser *self,
    size_t expected)
{
  self->expected=expected;
  memset(self->tars, -1, sizeof self->tars);
}

static size_t rd_parser_skipto(struct cling_rd_parser *self,
    struct utillib_token_scanner *input) 
{
  size_t code;
  while (true) {
    code=utillib_token_scanner_lookahead(input);
    if (code == UT_SYM_EOF || code == UT_SYM_ERR)
      rd_parser_fatal(self);
    for (int const * pi=self->tars; *pi!=-1; ++pi)
      if (*pi == code) 
        return code;
    utillib_token_scanner_shiftaway(input);
  }
  assert(false);
}

static struct utillib_json_value *
single_const_decl(struct cling_rd_parser *self,
    struct utillib_token_scanner *input);

static struct utillib_json_value *
multiple_const_decl(struct cling_rd_parser *self,
    struct utillib_token_scanner *input);

static struct utillib_json_value *
scanf_stmt(struct cling_rd_parser *self, struct utillib_token_scanner *input);

static struct utillib_json_value *var_defs(struct cling_rd_parser *self,
                                           struct utillib_token_scanner *input,
                                           char const *first_iden);

static struct utillib_json_value * composite_stmt(struct cling_rd_parser *self,
    struct utillib_token_scanner *input);

static struct utillib_json_value * normal_arglist(struct cling_rd_parser *self,
    struct utillib_token_scanner *input);

static struct utillib_json_value *program(struct cling_rd_parser *self,
                                          struct utillib_token_scanner *input);

/*
 * init/destroy
 */
void cling_rd_parser_init(struct cling_rd_parser *self,
    struct cling_symbol_table *symbols,
    struct cling_entity_list * entities) {
  self->symbols=symbols;
  self->entities=entities;
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
    utillib_json_value_destroy(self->root);
#ifndef NDEBUG
    printf("@@ longjmp from `%s' context @@\n",cling_symbol_kind_tostring(code));
#else
    utillib_vector_push_back(&self->elist,
        cling_premature_error(input, context));
#endif
    return NULL;
  }
}

void cling_rd_parser_report_errors(struct cling_rd_parser const *self) {
  UTILLIB_VECTOR_FOREACH(struct cling_error const *, error,
                         &self->elist) {
    cling_error_print(error);
  }
  fprintf(stderr,"%lu errors detected.\n", utillib_vector_size(&self->elist));
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
 * error: expected, unexpected, redefined
 * all skip to SYM_COMMA, SYM_SEMI.
 */
static struct utillib_json_value *
const_defs(struct cling_rd_parser *self, struct utillib_token_scanner *input,
           size_t expected_initializer) {

  char const *str;
  struct utillib_json_value * name;
  size_t initilizer;
  struct utillib_json_value *object;
  size_t code;
  size_t const context=SYM_CONST_DEF;
  struct utillib_json_value *array = utillib_json_array_create_empty();

  while (true) {
    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_IDEN) {
      rd_parser_skip_target_init(self, SYM_IDEN); 
      goto expected;
    }
    str = utillib_token_scanner_semantic(input);
    if (cling_symbol_table_exist_name(self->symbols, str, 0)) {
      rd_parser_skip_target_init(self, SYM_IDEN);
      goto redefined;
    }

    name=utillib_json_string_create(&str);
    utillib_token_scanner_shiftaway(input);

    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_EQ) {
      rd_parser_skip_target_init(self, SYM_EQ);
      goto expected;
    }
    utillib_token_scanner_shiftaway(input);

    code=utillib_token_scanner_lookahead(input);
    if (code != expected_initializer) {
      rd_parser_skip_target_init(self, expected_initializer);
      goto expected;
    }

    initilizer = utillib_token_scanner_semantic(input);
    utillib_token_scanner_shiftaway(input);

    object = utillib_json_object_create_empty();
    utillib_json_object_push_back(object, "identifier", name);
    cling_symbol_table_reserve(self->symbols, name->as_ptr);
    utillib_json_object_push_back(object, "initilizer",
        utillib_json_size_t_create(&initilizer));
    utillib_json_array_push_back(array, object);

    code = utillib_token_scanner_lookahead(input);
    switch (code) {
    case SYM_SEMI:
      goto return_array;
before_comma:
    case SYM_COMMA:
      utillib_token_scanner_shiftaway(input);
      break;
    default:
      /* unexpected */
      rd_parser_skip_target_init(self, SYM_COMMA);
      goto unexpected;
    }
  }
return_array:
  return array;

expected:
  utillib_vector_push_back(&self->elist, cling_expected_error(
        input, self->expected,
        code,self->context));
  goto skip;

redefined:
  utillib_vector_push_back(&self->elist, 
      cling_redined_error(input, str, self->context));
  goto skip;

unexpected:
      utillib_vector_push_back(&self->elist,
          cling_unexpected_error(input,
            code, self->context));
skip:
  self->tars[0]=SYM_COMMA;
  self->tars[1]=SYM_SEMI;
  switch(rd_parser_skipto(self,input)) {
    case SYM_COMMA:
      goto before_comma;
    case SYM_SEMI:
      goto return_array;
  }
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
  assert(code == SYM_KW_CONST);
  utillib_token_scanner_shiftaway(input);

  self->context=SYM_CONST_DECL;
  struct utillib_json_value *object;
  code = utillib_token_scanner_lookahead(input);

  if (code != SYM_KW_CHAR && code != SYM_KW_INT) {
    rd_parser_skip_target_init(self, SYM_TYPENAME);
    goto error;
  }
  object = utillib_json_object_create_empty();
  utillib_token_scanner_shiftaway(input);
  utillib_json_object_push_back(object, "type",
      utillib_json_size_t_create(&code));
  utillib_json_object_push_back(
      object, "const_defs", /* expected_initializer */
      const_defs(self, input, code == SYM_KW_INT ? SYM_UINT : SYM_CHAR));

  code = utillib_token_scanner_lookahead(input);
  if (code != SYM_SEMI) {
    rd_parser_skip_target_init(self, SYM_SEMI);
    goto error;
  }
  utillib_token_scanner_shiftaway(input);
  return object;

error:
  utillib_vector_push_back(&self->elist, cling_expected_error(
        input, self->expected,
        code, self->context));
  switch (self->expected) {
    case SYM_SEMI:
      return object;
    case SYM_TYPENAME:
      /* This is a serious error: const without typename */
      self->tars[0]=SYM_SEMI;
      self->tars[1]=SYM_KW_CONST;
      switch(rd_parser_skipto(self,input)) {
        case SYM_SEMI:
          utillib_token_scanner_shiftaway(input);
          return utillib_json_null_create();
        case SYM_KW_CONST:
          /*
           * This const is deliberately left for 
           * `multiple_const_decl' to parse.
           */
          return utillib_json_null_create();
        default:
          assert(false);
      }
    default:
      assert(false);
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
  struct utillib_json_value *object;
  while (true) {
    object=single_const_decl(self, input);
    if (object != &utillib_json_null) {
      cling_ast_insert_const(object,self->symbols);
      utillib_json_array_push_back(array, object);
    }
    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_KW_CONST)
      return array;
  }
}



/**
 * \funtion var_defs
 * lookahead: [ , ;
 * examples:
 * i,j,k
 * i[10], i, k[10]
 * return: array of object{type:(SYM_KW_INT|SYM_KW_CHAR),
 *  extend:size_t}
 */
static struct utillib_json_value *var_defs(struct cling_rd_parser *self,
    struct utillib_token_scanner *input,
    char const *first_iden) {
  size_t code;
  size_t const context=SYM_VAR_DEF;
  struct utillib_json_value *array = utillib_json_array_create_empty();
  struct utillib_json_value *object = utillib_json_object_create_empty();
  if (cling_symbol_table_exist_name(self->symbols, first_iden, 0)) {
    rd_parser_skip_target_init(self, SYM_IDEN);
    goto redefined;
  }
  utillib_json_object_push_back(object, "identifier",
      utillib_json_string_create(&first_iden));
  cling_symbol_table_reserve(self->symbols, first_iden);
  utillib_json_array_push_back(array, object);

  while (true) {
    size_t extend;
    code = utillib_token_scanner_lookahead(input);
    switch (code) {
      /* Those that can follow iden are SYM_LK and SYM_COMMA*/
      case SYM_LK:
        utillib_token_scanner_shiftaway(input);
        code = utillib_token_scanner_lookahead(input);
        if (code != SYM_UINT) {
          rd_parser_skip_target_init(self, SYM_UINT);
          goto expected;
        }
        extend = utillib_token_scanner_semantic(input);
        utillib_json_object_push_back(object, "extend",
            utillib_json_size_t_create(&extend));
        utillib_token_scanner_shiftaway(input);
        code = utillib_token_scanner_lookahead(input);
        if (code != SYM_RK) {
          rd_parser_skip_target_init(self, SYM_RK);
          goto expected;
        }
before_rk:
        utillib_token_scanner_shiftaway(input);
        break;
before_comma:
      case SYM_COMMA:
        utillib_token_scanner_shiftaway(input);
        code = utillib_token_scanner_lookahead(input);
        if (code != SYM_IDEN) {
          rd_parser_skip_target_init(self, SYM_IDEN);
          goto expected;
        }
        first_iden = utillib_token_scanner_semantic(input);
        if (cling_symbol_table_exist_name(self->symbols, first_iden, 0)) {
          rd_parser_skip_target_init(self, SYM_IDEN);
          goto redefined;
        }
        object = utillib_json_object_create_empty();
        utillib_json_object_push_back(object, "identifier",
            utillib_json_string_create(&first_iden));
        cling_symbol_table_reserve(self->symbols, first_iden);
        utillib_json_array_push_back(array, object);
        utillib_token_scanner_shiftaway(input);
        break;
return_array:
      default:
        return array;
    }
  }
expected:
  utillib_vector_push_back(&self->elist, cling_expected_error(
        input, self->expected,
        code, self->context));
  goto skip;
redefined:
  utillib_vector_push_back(&self->elist, cling_redined_error(
        input, first_iden, self->context));

skip:
  switch(self->expected) {
    case SYM_UINT:
      self->tars[0]=SYM_RK;
      self->tars[1]=SYM_COMMA;
      self->tars[2]=SYM_SEMI;
      switch (rd_parser_skipto(self, input)) {
        case SYM_RK:
          goto before_rk;
        case SYM_COMMA:
          goto before_comma;
        case SYM_SEMI:
          goto return_array;
        default:
          assert(false);
      }
    case SYM_IDEN:
      self->tars[0]=SYM_COMMA;
      self->tars[1]=SYM_SEMI;
      switch(rd_parser_skipto(self, input)) {
        case SYM_COMMA:
          goto before_comma;
        case SYM_SEMI:
          goto return_array;
        default:
          assert(false);
      }
    default:
      assert(false);
  }
}

/**
 * \funtion singel_var_decl
 */

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
    utillib_vector_push_back(&self->elist, cling_expected_error(
          input, SYM_SEMI,
          code, SYM_VAR_DECL));
    return object;
  }
  utillib_token_scanner_shiftaway(input);
  return object;
}

static struct utillib_json_value *
maybe_multiple_var_decls(struct cling_rd_parser *self,
    struct utillib_token_scanner *input, size_t *type,
    char const **first_iden) {
  size_t code = utillib_token_scanner_lookahead(input);
  assert(code == SYM_KW_INT || code == SYM_KW_CHAR);
  utillib_token_scanner_shiftaway(input);
  *type = code;

  struct utillib_json_value *object;
  struct utillib_json_value *array = utillib_json_array_create_empty();

  while (true) {
    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_IDEN) {
      rd_parser_skip_target_init(self, SYM_IDEN);
      goto expected_iden;
    }
    /* Must strdup first_iden */
    *first_iden = strdup(utillib_token_scanner_semantic(input));
    utillib_token_scanner_shiftaway(input);
    code = utillib_token_scanner_lookahead(input);
    switch (code) {
      case SYM_LP:
        /* We may have function here */
        goto return_array;
      case SYM_LK:
      case SYM_COMMA:
      case SYM_SEMI:
        object=singel_var_decl(self, input, *type, *first_iden);
        cling_ast_insert_variable(object,self->symbols);
        utillib_json_array_push_back(array, object);
        code = utillib_token_scanner_lookahead(input);
        if (code != SYM_KW_INT && code != SYM_KW_CHAR)
          return array;
        *type = code;
        utillib_token_scanner_shiftaway(input);
        break;
      default:
        /* We are screw */
        utillib_vector_push_back(&self->elist, cling_unexpected_error(
              input, code, SYM_VAR_DECL));
        rd_parser_skip_target_init(self, SYM_LP);
        goto expected_maybe_lp; 
    }
  }
return_array:
    return array;

expected_iden:
  utillib_vector_push_back(&self->elist, cling_expected_error(
        input,self->expected,
        code, SYM_VAR_DECL));
  /* Fail through */
expected_maybe_lp:
  self->tars[0]=SYM_SEMI;
  switch(rd_parser_skipto(self, input)) {
    case SYM_SEMI:
      utillib_token_scanner_shiftaway(input);
      goto return_array;
  }
}

static struct utillib_json_value * single_function_decl(struct cling_rd_parser *self,
    struct utillib_token_scanner *input, 
    size_t context,
    char const * name) {
  size_t code=utillib_token_scanner_lookahead(input);
  assert (SYM_LP == code);
  utillib_token_scanner_shiftaway(input);

  struct utillib_json_value * object=utillib_json_object_create_empty();
  utillib_json_object_push_back(object, "name",
      utillib_json_string_create(&name));
  utillib_json_object_push_back(object, "arglist",
      normal_arglist(self, input));
  code=utillib_token_scanner_lookahead(input);
  if (code != SYM_LB) {
    rd_parser_skip_target_init(self, SYM_LB);
    goto error;
  }
  utillib_json_object_push_back(object, "composite_stmt",
      composite_stmt(self, input));
  if (code != SYM_RB) {
    rd_parser_skip_target_init(self, SYM_LB);
    goto error;
  }
  return object;
error:
  utillib_vector_push_back(&self->elist,
      cling_expected_error(input,
        self->expected, code, self->context));
  switch (self->expected) {}


}

/**
 * \function scanf_stmt
 * return: nonempty array of identifier
 * or null for error
 */
static struct utillib_json_value *
scanf_stmt(struct cling_rd_parser *self, struct utillib_token_scanner *input) {
  size_t code = utillib_token_scanner_lookahead(input);
  assert(code == SYM_KW_SCANF);
  utillib_token_scanner_shiftaway(input);

  char const *str;
  self->context=SYM_SCANF_STMT;
  struct utillib_json_value *array;

  code=utillib_token_scanner_lookahead(input);
  if (code !=  SYM_LP) {
    rd_parser_skip_target_init(self, SYM_LP);
    goto expected_lp;
  }
  utillib_token_scanner_shiftaway(input);
  array = utillib_json_array_create_empty();

  while (true) {
    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_IDEN) {
      rd_parser_skip_target_init(self, SYM_IDEN);
      goto expected_iden;
    }
before_iden:
    str = utillib_token_scanner_semantic(input);
    utillib_json_array_push_back(array, utillib_json_string_create(&str));
    utillib_token_scanner_shiftaway(input);
    code = utillib_token_scanner_lookahead(input);
    switch (code) {
    case SYM_COMMA:
      utillib_token_scanner_shiftaway(input);
      break;
    case SYM_RP:
      utillib_token_scanner_shiftaway(input);
      code = utillib_token_scanner_lookahead(input);
      if (code != SYM_SEMI) {
        goto expected_semi;
      }
before_semi:
      utillib_token_scanner_shiftaway(input);
      goto return_array;
    default:
      rd_parser_skip_target_init(self, SYM_RP);
      goto unexpected;
    }
  }
return_array:
  /*
   * Unique return
   */
    return array;
expected_lp:
    /*
     * If we are missing '(' after `scanf', skip
     * to ';' directly and return null if succeed.
     */
  utillib_vector_push_back(&self->elist, 
      cling_expected_error(
        input, self->expected,
        code, self->context));
  self->tars[0]=SYM_SEMI;
  switch (rd_parser_skipto(self, input)) {
    case SYM_SEMI:
      utillib_token_scanner_shiftaway(input);
      return utillib_json_null_create();
  }
expected_iden:
  /*
   * If we are missing iden after '(' or ',',
   * try to skip to the next iden or ';'.
   */
  utillib_vector_push_back(&self->elist,
      cling_expected_error(input, self->expected,
        code, self->context));
  self->tars[0]=SYM_IDEN;
  self->tars[1]=SYM_SEMI;
  switch(rd_parser_skipto(self, input)) {
    case SYM_IDEN:
      goto before_iden;
    case SYM_SEMI:
      goto before_semi;
    default:
      assert(false);
  }
expected_semi:
  /*
   * If we miss a semi, we just report that
   * but not try to skip anything for too little
   * context to make a decision.
   */
  utillib_vector_push_back(&self->elist,
      cling_expected_error(input, SYM_SEMI,
        code, self->context));
  goto return_array;
unexpected:
  /*
   * If we see an unexpected token after
   * iden, we handle it by skipping to a semi,
   * since the scanf_stmt is incomplete.
   */
  utillib_vector_push_back(&self->elist,
      cling_unexpected_error(input, code, self->context));
  self->tars[0]=SYM_SEMI;
  switch (rd_parser_skipto(self, input)) {
  case SYM_SEMI:
    goto before_semi;
  }

}

static struct utillib_json_value *program(struct cling_rd_parser *self,
    struct utillib_token_scanner *input) {

  self->root=utillib_json_object_create_empty();
  utillib_json_object_push_back(self->root, "multiple_const_decl",
      multiple_const_decl(self, input));
  return self->root;
}

static struct utillib_json_value * normal_arglist(struct cling_rd_parser *self,
    struct utillib_token_scanner *input)
{
  size_t code=utillib_token_scanner_lookahead(input);
  assert (code == SYM_LP);
  utillib_token_scanner_shiftaway(input);
  self->context=SYM_NARGS;
  size_t type;
  char const *name;
  struct utillib_json_value * object;
  struct utillib_json_value * array=utillib_json_array_create_empty();

  while (true) {
    code=utillib_token_scanner_lookahead(input);
    switch (code) {
    case SYM_KW_INT:
    case SYM_KW_CHAR:
      object=utillib_json_object_create_empty();
      type=code==SYM_KW_INT?CL_INT:CL_CHAR;
      utillib_json_object_push_back(object, "type", 
          utillib_json_size_t_create(&type));
      utillib_token_scanner_shiftaway(input);
      code=utillib_token_scanner_lookahead(input);
      if (code != SYM_IDEN) {
        rd_parser_skip_target_init(self, SYM_IDEN);
        goto expected_iden;
      }
      name=utillib_token_scanner_lookahead(input);
      utillib_json_object_push_back(object, "name",
          utillib_json_string_create(&name));
      utillib_token_scanner_shiftaway(input);
      break;
    case SYM_COMMA:
      utillib_json_array_push_back(array, object);
      break;
return_array:
    case SYM_RP:
      return array;
    default:
      goto unexpected;
    }
  }
unexpected:
  utillib_vector_push_back(&self->elist,
      cling_unexpected_error(input, code, self->context));
  goto skip;

expected_iden:
  utillib_vector_push_back(&self->elist,
      cling_expected_error(input, self->expected, code, self->context));
skip:
  switch (rd_parser_skipto(self, input)) {
    case SYM_RP:
      goto return_array;
  }
}

static struct utillib_json_value * assign_stmt(struct cling_rd_parser *self,
    struct utillib_token_scanner *input)
{
  size_t code=utillib_token_scanner_lookahead(input);
  assert (code == SYM_IDEN);
  self->context=SYM_ASSIGN_STMT;
  struct utillib_json_value * lhs;
  struct utillib_json_value * rhs;
  struct utillib_json_value * object;

  struct cling_opg_parser opg_parser;
  cling_opg_parser_init(&opg_parser, SYM_SEMI, &self->elist);
  lhs=cling_opg_parser_parse(&opg_parser, input);
  if (lhs == &utillib_json_null) {
    rd_parser_skip_target_init(self, SYM_EXPR);
    goto expected_lhs;
  }

  object=utillib_json_object_create_empty();
  cling_ast_add_lhs(object, lhs);
  cling_ast_add_op(object, OP_STORE);
  code=utillib_token_scanner_lookahead(input);
  if (code != SYM_EQ) {
    goto expected_eq;
  }
before_rhs:
  utillib_token_scanner_shiftaway(input);
  rhs=cling_opg_parser_parse(&opg_parser, input);
  if (rhs == &utillib_json_null) {
    goto expected_rhs;
  }
  cling_ast_add_rhs(object, rhs);

return_object:
  cling_opg_parser_destroy(&opg_parser);
  return object;

expected_lhs:
  self->tars[0]=SYM_EQ;
  self->tars[1]=SYM_SEMI;
  switch (rd_parser_skipto(self, input)) {
  case SYM_EQ:
    goto before_rhs;
  case SYM_SEMI:
    cling_opg_parser_destroy(&opg_parser);
    return utillib_json_null_create();
  }
expected_eq:
  utillib_vector_push_back(&self->elist,
      cling_expected_error(input, code, 
        self->expected, self->context));
expected_rhs:
  self->tars[0]=SYM_SEMI;
  switch(rd_parser_skipto(self, input)) {
  case SYM_SEMI:
    cling_ast_add_rhs(object, utillib_json_null_create());
    goto return_object;
  }

}

static struct utillib_json_value * return_stmt(struct cling_rd_parser *self,
    struct utillib_token_scanner *input)
{
  size_t code=utillib_token_scanner_lookahead(input);
  assert (code == SYM_KW_RETURN);
  utillib_token_scanner_shiftaway(input);
  struct utillib_vector * elist=&self->elist;
  self->context=SYM_RETURN_STMT;
  struct utillib_json_value * expr;
  struct cling_opg_parser opg_parser;

  code=utillib_token_scanner_lookahead(input);
  switch (code) {
  case SYM_SEMI:
    return utillib_json_null_create();
  case SYM_LP:
    utillib_token_scanner_shiftaway(input);
    cling_opg_parser_init(&opg_parser, SYM_RP, elist);
    expr=cling_opg_parser_parse(&opg_parser, input);
    if (expr == &utillib_json_null) {
      rd_parser_skip_target_init(self, SYM_EXPR);
      cling_opg_parser_destroy(&opg_parser);
      goto expected_expr;
    }
    code=utillib_token_scanner_lookahead(input);
    if (code != SYM_RP) {
      rd_parser_skip_target_init(self, SYM_EXPR);
      goto expected_rp;
    }
    utillib_token_scanner_shiftaway(input);
    cling_opg_parser_destroy(&opg_parser);
    return expr;
  default:
    rd_parser_skip_target_init(self, SYM_EXPR);
    goto unexpected;
  }
expected_rp:
  utillib_vector_push_back(elist,
      cling_expected_error(input,
        self->expected, code, self->context));
  goto skip;

unexpected:
  utillib_vector_push_back(elist,
      cling_unexpected_error(input, code, self->context));
  goto skip;

expected_expr:
skip:
  self->tars[0]=SYM_SEMI;
  switch (rd_parser_skipto(self, input)) {
  case SYM_SEMI:
    return utillib_json_null_create();
  }
}

static struct utillib_json_value * if_stmt(struct cling_rd_parser *self,
    struct utillib_token_scanner *input)
{


}

static struct utillib_json_value * switch_stmt(struct cling_rd_parser *self,
    struct utillib_token_scanner *input)
{



}

static struct utillib_json_value * printf_stmt(struct cling_rd_parser *self,
    struct utillib_token_scanner *input)
{



}

static struct utillib_json_value * for_stmt(struct cling_rd_parser *self,
    struct utillib_token_scanner *input)
{



}

static struct utillib_json_value * composite_stmt(struct cling_rd_parser *self,
    struct utillib_token_scanner *input)
{



}


