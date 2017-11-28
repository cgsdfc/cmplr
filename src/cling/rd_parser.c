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
    return scanf_stmt(self, input);
  default:
#ifndef NDEBUG
    printf("@@ longjmp from `%s' context @@\n",cling_symbol_kind_tostring(code));
#endif
    return NULL;
  }
}

void cling_rd_parser_report_errors(struct cling_rd_parser const *self) {
  UTILLIB_VECTOR_FOREACH(struct rd_parser_error const *, error,
                         &self->elist) {
    rd_parser_error_print(error);
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
 */
static struct utillib_json_value *
const_defs(struct cling_rd_parser *self, struct utillib_token_scanner *input,
           size_t expected_initializer) {

  struct rd_parser_skip_target target;
  char const *str;
  struct utillib_json_value * name;
  size_t initilizer;
  struct utillib_json_value *object;
  size_t code;
  size_t const context=SYM_CONST_DEF;
  struct utillib_json_value *array = utillib_json_array_create_empty();
  bool has_const_defs=false;

  while (true) {
    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_IDEN) {
      rd_parser_skip_target_init(&target, SYM_IDEN); 
      goto error;
    }
    str = utillib_token_scanner_semantic(input);
    name=utillib_json_string_create(&str);
    has_const_defs=true;
    utillib_token_scanner_shiftaway(input);

    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_EQ) {
      rd_parser_skip_target_init(&target, SYM_EQ);
      goto error;
    }
    utillib_token_scanner_shiftaway(input);

    code=utillib_token_scanner_lookahead(input);
    if (code != expected_initializer) {
      rd_parser_skip_target_init(&target, expected_initializer);
      goto error;
    }

    initilizer = utillib_token_scanner_semantic(input);
    utillib_token_scanner_shiftaway(input);

    object = utillib_json_object_create_empty();
    utillib_json_object_push_back(object, "identifier", name);
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
      rd_parser_skip_target_init(&target, SYM_COMMA);
      utillib_vector_push_back(&self->elist,
          rd_parser_unexpected_error(input,
            code, context));
      goto unexpected;
    }
  }
return_array:
  if (has_const_defs)
    return array;
  utillib_json_value_destroy(array);
  return utillib_json_null_create();

error:
  utillib_vector_push_back(&self->elist, rd_parser_expected_error(
        input, target.expected,
        code,context));
unexpected:
  target.tars[0]=SYM_COMMA;
  target.tars[1]=SYM_SEMI;
  switch(rd_parser_skipto(&target,input)) {
    case SYM_COMMA:
      goto before_comma;
    case SYM_SEMI:
      goto return_array;
    case UT_SYM_EOF:
      goto fatal;
  }
fatal:
  longjmp(self->fatal_saver, context);
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

  struct rd_parser_skip_target target;
  const size_t context=SYM_CONST_DECL;
  struct utillib_json_value *object;
  code = utillib_token_scanner_lookahead(input);

  if (code != SYM_KW_CHAR && code != SYM_KW_INT) {
    rd_parser_skip_target_init(&target, SYM_TYPENAME);
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
    rd_parser_skip_target_init(&target, SYM_SEMI);
    goto error;
  }
  utillib_token_scanner_shiftaway(input);
  return object;

error:
  utillib_vector_push_back(&self->elist, rd_parser_expected_error(
        input, target.expected,
        code, context));
  switch (target.expected) {
    case SYM_SEMI:
      return object;
    case SYM_TYPENAME:
      /* This is a serious error: const without typename */
      target.tars[0]=SYM_SEMI;
      target.tars[1]=SYM_KW_CONST;
      switch(rd_parser_skipto(&target,input)) {
        case UT_SYM_EOF:
          goto fatal;
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

fatal:
  longjmp(self->fatal_saver, context);
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
      rd_parser_insert_const(self, input, object);
      utillib_json_array_push_back(array, object);
    }
    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_KW_CONST)
      return array;
  }
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
  struct rd_parser_skip_target target;
  const size_t context=SYM_SCANF_STMT;
  struct utillib_json_value *array;

  code=utillib_token_scanner_lookahead(input);
  if (code !=  SYM_LP) {
    rd_parser_skip_target_init(&target, SYM_LP);
    goto expected_lp;
  }
  utillib_token_scanner_shiftaway(input);
  array = utillib_json_array_create_empty();

  while (true) {
    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_IDEN) {
      rd_parser_skip_target_init(&target, SYM_IDEN);
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
      rd_parser_skip_target_init(&target, SYM_RP);
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
      rd_parser_expected_error(
        input, target.expected,
        code, context));
  target.tars[0]=SYM_SEMI;
  switch (rd_parser_skipto(&target, input)) {
    case SYM_SEMI:
      utillib_token_scanner_shiftaway(input);
      return utillib_json_null_create();
    case UT_SYM_EOF:
      goto fatal;
  }
expected_iden:
  /*
   * If we are missing iden after '(' or ',',
   * try to skip to the next iden or ';'.
   */
  utillib_vector_push_back(&self->elist,
      rd_parser_expected_error(input, target.expected,
        code, context));
  target.tars[0]=SYM_IDEN;
  target.tars[1]=SYM_SEMI;
  switch(rd_parser_skipto(&target, input)) {
    case SYM_IDEN:
      goto before_iden;
    case SYM_SEMI:
      goto before_semi;
    case UT_SYM_EOF:
      goto fatal;
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
      rd_parser_expected_error(input, SYM_SEMI,
        code, context));
  goto return_array;
unexpected:
  /*
   * If we see an unexpected token after
   * iden, we handle it by skipping to a semi,
   * since the scanf_stmt is incomplete.
   */
  utillib_vector_push_back(&self->elist,
      rd_parser_unexpected_error(input, code, context));
  target.tars[0]=SYM_SEMI;
  switch (rd_parser_skipto(&target, input)) {
  case SYM_SEMI:
    goto before_semi;
  case UT_SYM_EOF:
    goto fatal;
  }
fatal:
  longjmp(self->fatal_saver, context);

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
  struct rd_parser_skip_target target;
  struct utillib_json_value *array = utillib_json_array_create_empty();
  struct utillib_json_value *object = utillib_json_object_create_empty();
  utillib_json_object_push_back(object, "identifier",
      utillib_json_string_create(&first_iden));
  utillib_json_array_push_back(array, object);

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
          rd_parser_skip_target_init(&target, SYM_UINT);
          goto error;
        }
        extend = utillib_token_scanner_semantic(input);
        utillib_json_object_push_back(object, "extend",
            utillib_json_size_t_create(&extend));
        utillib_token_scanner_shiftaway(input);
        code = utillib_token_scanner_lookahead(input);
        if (code != SYM_RK) {
          rd_parser_skip_target_init(&target, SYM_RK);
          goto error;
        }
before_rk:
        utillib_token_scanner_shiftaway(input);
        break;
before_comma:
      case SYM_COMMA:
        object = utillib_json_object_create_empty();
        utillib_json_array_push_back(array, object);
        utillib_token_scanner_shiftaway(input);
        code = utillib_token_scanner_lookahead(input);
        if (code != SYM_IDEN) {
          rd_parser_skip_target_init(&target, SYM_IDEN);
          goto error;
        }
        first_iden = utillib_token_scanner_semantic(input);
        utillib_json_object_push_back(object, "identifier",
            utillib_json_string_create(&first_iden));
        utillib_token_scanner_shiftaway(input);
        break;
return_array:
      default:
        return array;
    }
  }
error:
  utillib_vector_push_back(&self->elist, rd_parser_expected_error(
        input, target.expected,
        code, SYM_VAR_DEF));
  switch(target.expected) {
    case SYM_UINT:
      target.tars[0]=SYM_RK;
      target.tars[1]=SYM_COMMA;
      target.tars[2]=SYM_SEMI;
      switch (rd_parser_skipto(&target, input)) {
        case UT_SYM_EOF:
          goto fatal;
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
      target.tars[0]=SYM_COMMA;
      target.tars[1]=SYM_SEMI;
      switch(rd_parser_skipto(&target, input)) {
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
fatal:
  longjmp(self->fatal_saver, SYM_VAR_DEF);
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

  struct rd_parser_skip_target target;
  bool has_var_decl = false;
  struct utillib_json_value *array = utillib_json_array_create_empty();

  while (true) {
    code = utillib_token_scanner_lookahead(input);
    if (code != SYM_IDEN) {
      rd_parser_skip_target_init(&target, SYM_IDEN);
      goto expected_iden;
    }
    /* Must strdup first_iden */
    *first_iden = strdup(utillib_token_scanner_semantic(input));
    utillib_token_scanner_shiftaway(input);
    code = utillib_token_scanner_lookahead(input);
    switch (code) {
      case SYM_LP:
        /* We may have function here */
        goto maybe_return_array;
      case SYM_LK:
      case SYM_COMMA:
      case SYM_SEMI:
        has_var_decl = true;
        utillib_json_array_push_back(
            array, singel_var_decl(self, input, *type, *first_iden));
        free(first_iden);
        code = utillib_token_scanner_lookahead(input);
        if (code != SYM_KW_INT && code != SYM_KW_CHAR)
          return array;
        *type = code;
        utillib_token_scanner_shiftaway(input);
        break;
      default:
        /* We are screw */
        utillib_vector_push_back(&self->elist, rd_parser_unexpected_error(
              input, code, SYM_VAR_DECL));
        rd_parser_skip_target_init(&target, SYM_LP);
        goto expected_maybe_lp; 
    }
  }
maybe_return_array:
  if (has_var_decl)
    return array;
  utillib_json_value_destroy(array);
  return utillib_json_null_create();

expected_iden:
  utillib_vector_push_back(&self->elist, rd_parser_expected_error(
        input,target.expected,
        code, SYM_VAR_DECL));
  /* Fail through */
expected_maybe_lp:
  target.tars[0]=SYM_SEMI;
  switch(rd_parser_skipto(&target, input)) {
    case UT_SYM_EOF:
      longjmp(self->fatal_saver, SYM_VAR_DECL);
    case SYM_SEMI:
      utillib_token_scanner_shiftaway(input);
      goto maybe_return_array;
  }
}

static struct utillib_json_value *program(struct cling_rd_parser *self,
    struct utillib_token_scanner *input) {
  size_t type;
  char const *first_iden;
  struct utillib_json_value *val =
  maybe_multiple_var_decls(self, input, &type, &first_iden);
  return val;
}

static struct utillib_json_value * normal_arglist(struct cling_rd_parser *self,
    struct utillib_token_scanner *input)
{

}

static struct utillib_json_value * composite_stmt(struct cling_rd_parser *self,
    struct utillib_token_scanner *input)
{



}

static struct utillib_json_value * single_function_decl(struct cling_rd_parser *self,
    struct utillib_token_scanner *input, 
    size_t context,
    char const * name) {
  size_t code=utillib_token_scanner_lookahead(input);
  assert (SYM_LP == code);
  utillib_token_scanner_shiftaway(input);

  struct rd_parser_skip_target target;
  struct utillib_json_value * object=utillib_json_object_create_empty();
  utillib_json_object_push_back(object, "name",
      utillib_json_string_create(&name));
  utillib_json_object_push_back(object, "arglist",
      normal_arglist(self, input));
  code=utillib_token_scanner_lookahead(input);
  if (code != SYM_LB) {
    rd_parser_skip_target_init(&target, SYM_LB);
    goto error;
  }
  utillib_json_object_push_back(object, "composite_stmt",
      composite_stmt(self, input));
  if (code != SYM_RB) {
    rd_parser_skip_target_init(&target, SYM_LB);
    goto error;
  }
  return object;
error:
  utillib_vector_push_back(&self->elist,
      rd_parser_expected_error(input,
        target.expected, code, context));
  switch (target.expected) {}


}


