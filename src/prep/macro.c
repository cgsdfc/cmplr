#include "macro.h"

static prep_macro_t *macro_create(int kind, void *maybe_function) {
  prep_macro_t *self = malloc(sizeof *self);
  self->kind = kind;
  self->as_function = maybe_function;
  return self;
}

static prep_macro_entry_t *prep_macro_entry_create(int kind, char const *data) {
  prep_macro_entry_t *self = malloc(sizeof *self);
  self->kind = kind;
  self->as_str = data;
  return self;
}

static prep_macro_plainstr_t *macro_plainstr_create(char const *name,
                                                    char const *defs) {
  prep_macro_plainstr_t *self = malloc(sizeof *self);
  self->name = name;
  self->defs = defs;
  return self;
}

static void macro_function_init(prep_macro_funtion_t *self, char const *name) {
  self->name = name;
  utillib_vector_init(&self->args);
}

static char const *macro_function_parse_args(prep_macro_funtion_t *self,
                                             char const *defs) {
  for (char const *str = defs; *str; ++str) {
    switch (*str) {
    case ')':
      return ++str;
    case ',':
      utillib_vector_push_back(&self->args, strdup(utillib_string_c_str(&buf)));
    }

    static void macro_function_parse_body(prep_macro_funtion_t * self,
                                          char const *body) {}
  }
}

/**
 * \function macro_function_parse
 * Parses a string as the part of a function-like macro definition.
 * This part begin with `(' character right after the name of the macro,
 * i.e `#define Add(A, B) ((A) + (B))'
 * ~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~^~
 * and ends with the last printable character of this line.
 */
static void macro_function_parse(prep_macro_funtion_t *self, char const *defs) {
  char const *body = macro_function_parse_args(self, defs);
  macro_function_parse_body(self, body);
}

static prep_macro_funtion_t *macro_function_create(char const *name,
                                                   char const *defs) {
  prep_macro_funtion_t *self = malloc(sizeof *self);
  macro_function_init(self, name);
  macro_function_parse(self, defs);
  return self;
}

prep_macro_definition_t *prep_macro_create(char const *name,
                                           char const *maybe_function) {
  char *maybe_lp = *maybe_function;
  void *macro;
  if (*maybe_lp == '(') {
    macro = macro_
  }
}

prep_macro_expansion_t *prep_macro_expansion_create(prep_macro_t * self,
