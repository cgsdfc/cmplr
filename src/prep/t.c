#include <utillib/unordered_map.h>

typedef struct prep_macro_engine_t {
  utillib_unordered_map defs;
} prep_macro_engine_t;

/**
 * \function prep_skip_space
 * Returns pointer to the first non-space
 * char in `s'. Space here means `isspace'
 * returns true.
 */
char const *prep_skip_space(char const *s) {
  while (*s && isspace(*s))
    ++s;
  return s;
}

char const *prep_parse_identifier(char const *s) {
  if (isalpha(*s) || *s == '_')
    while (*s && (isalnum(*s) || *s == '_'))
      ++s;
  return s;
}

/**
 * \function prep_macro_engine_add_define
 * Adds a macro definition to `self'.
 * \param define A string in the form
 * `macro_name macro_definition`.
 * One or more space (excluding newline) can follow and precede `macro_name' and
 * any amount of white space
 * is allowed after `macro_definition'.
 */
int prep_macro_engine_add_s(prep_macro_engine_t *self, char const *s) {
  lexical_string_scanner_t scanner;
  lexical_string_scanner_init(&scanner, s);
}
