#ifndef UTILLIB_SYMBOL_H
#define UTILLIB_SYMBOL_H
#include "enum.h"
#include "json.h"
#define UTILLIB_SYMBOL_EOF (&utillib_symbol_eof)
#define UTILLIB_SYMBOL_ERR  (&utillib_symbol_error)
#define UTILLIB_SYMBOL_EPS  (&utillib_symbol_epsilon)

#define UTILLIB_SYMBOL_BEGIN(NAME) static const struct utillib_symbol NAME []={
#define UTILLIB_SYMBOL_NON_TERMINAL(VALUE) [VALUE]={ .value=(VALUE), .kind=UT_SYMBOL_NON_TERMINAL, .name= # VALUE },
#define UTILLIB_SYMBOL_TERMINAL(VALUE) { .value=(VALUE), .kind=UT_SYMBOL_TERMINAL, .name= # VALUE },
#define UTILLIB_SYMBOL_END(NAME) {0}};

UTILLIB_ENUM_BEGIN(utillib_symbol_special_kind)
  UTILLIB_ENUM_ELEM_INIT(UT_SYM_EOF, 0)
  UTILLIB_ENUM_ELEM_INIT(UT_SYM_EPS, -1)
  UTILLIB_ENUM_ELEM_INIT(UT_SYM_NULL, -2)
  UTILLIB_ENUM_ELEM_INIT(UT_SYM_ERR, -3)
UTILLIB_ENUM_END(utillib_symbol_special_kind)

UTILLIB_ENUM_BEGIN(utillib_symbol_kind)
  UTILLIB_ENUM_ELEM_INIT(UT_SYMBOL_TERMINAL, 1)
  UTILLIB_ENUM_ELEM_INIT(UT_SYMBOL_NON_TERMINAL, 2)
UTILLIB_ENUM_END(utillib_symbol_kind)

/**
 * \struct utillib_symbol
 * An abstract representation
 * of a symbol (both terminal and non terminal)
 * in formal language theory.
 * Usually `utillib_symbol's are static initialized
 * to define bigger constructs of a language.
 */

struct utillib_symbol {
  int kind;
  int value;
  char const * name;
};

extern struct utillib_symbol utillib_symbol_epsilon;
extern struct utillib_symbol utillib_symbol_eof;
extern struct utillib_symbol utillib_symbol_error;
utillib_json_value_t * utillib_symbol_json_object_create(void *data, size_t offset);;
utillib_json_value_t * utillib_symbol_json_object_pointer_create(void *data, size_t offset);

#endif // UTILLIB_SYMBOL_H
