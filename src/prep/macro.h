#ifndef PREP_MACRO_H
#define PREP_MACRO_H
#include <utillib/enum.h>
#include <utillib/vector.h>

UTILLIB_ENUM_BEGIN(macro_kind_t)
UTILLIB_ENUM_ELEM(MACRO_KIND_FUNCTION)
UTILLIB_ENUM_ELEM(MACRO_KIND_PLAINSTR)
UTILLIB_ENUM_END(macro_kind_t)

UTILLIB_ENUM_BEGIN(macro_entry_kind_t)
UTILLIB_ENUM_ELEM(MACRO_ENTRY_ARG)
UTILLIB_ENUM_ELEM(MACRO_ENTRY_ID)
UTILLIB_ENUM_ELEM(MACRO_ENTRY_STR)
UTILLIB_ENUM_END(macro_entry_kind_t)

typedef struct prep_macro_t {
  int kind;
  union {
    void *as_plainstr;
    void *as_function;
  };
} prep_macro_t;

/*
 * #define Add(A, B) ((A) + (B))
 * #define M_PI 3.14444
 */

typedef struct prep_macro_function_t {
  char const *name;
  utillib_vector args;
  char const *body;
} prep_macro_function_t;

typedef struct prep_macro_plainstr_t {
  char const *name;
  char const *defs;
} prep_macro_plainstr_t;

typedef struct prep_macro_entry_t {
  int kind;
  union {
    char const *as_id;
    size_t as_arg;
    char const *as_str;
  };
} prep_macro_entry_t;

typedef struct prep_macro_expasion_t {
  int kind;
  union {
    char const *as_plainstr;
    void *as_function;
  };
} prep_macro_expasion_t;

typedef struct prep_macro_expasion_func_t {
  utillib_vector body;
} prep_macro_expasion_func_t;

#endif // PREP_MACRO_H
