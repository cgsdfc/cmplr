#include "parser.h"
#include "directive.h"
#include "token.h"
#include <ctype.h>
#include <scanner/input_buf.h>
#include <scanner/match.h>
#include <utillib/error.h>

static scanner_str_entry_t const *get_directives(void) {
  SCANNER_STR_ENTRY_BEGIN(directive)
  SCANNER_STR_ENTRY_ELEM("define", DR_DEFINE)
  SCANNER_STR_ENTRY_ELEM("undef", DR_UNDEF)
  SCANNER_STR_ENTRY_ELEM("if", DR_IF)
  SCANNER_STR_ENTRY_ELEM("ifndef", DR_IFNDEF)
  SCANNER_STR_ENTRY_ELEM("defined", DR_DEFINED)
  SCANNER_STR_ENTRY_ELEM("error", DR_ERROR)
  SCANNER_STR_ENTRY_ELEM("line", DR_LINE)
  SCANNER_STR_ENTRY_ELEM("include", DR_INCLUDE)
  SCANNER_STR_ENTRY_ELEM("ifdef", DR_IFDEF)
  SCANNER_STR_ENTRY_ELEM("assert", DR_ASSERT)
  SCANNER_STR_ENTRY_ELEM("cpu", DR_CPU)
  SCANNER_STR_ENTRY_ELEM("elif", DR_ELIF)
  SCANNER_STR_ENTRY_ELEM("else", DR_ELSE)
  SCANNER_STR_ENTRY_ELEM("include_next", DR_INCLUDE_NEXT)
  SCANNER_STR_ENTRY_ELEM("pragma", DR_PRAGMA)
  SCANNER_STR_ENTRY_ELEM("system", DR_SYSTEM)
  SCANNER_STR_ENTRY_ELEM("unassert", DR_UNASSERT)
  SCANNER_STR_ENTRY_ELEM("warning", DR_WARNING)
  SCANNER_STR_ENTRY_END(directive)
}

static scanner_str_entry_t const *get_pdmacro(void) {
  SCANNER_STR_ENTRY_BEGIN(predefined_macro)
  SCANNER_STR_ENTRY_ELEM("__FILE__", PREP_FILE)
  SCANNER_STR_ENTRY_ELEM("__LINE__", PREP_LINE)
  SCANNER_STR_ENTRY_ELEM("__DATE__", PREP_DATE)
  SCANNER_STR_ENTRY_ELEM("__TIME__", PREP_TIME)
  SCANNER_STR_ENTRY_ELEM("__STDC__", PREP_STDC)
  SCANNER_STR_ENTRY_ELEM("__GNUC__", PREP_GNUC)
  SCANNER_STR_ENTRY_ELEM("__STDC_VERSION__", PREP_STDC_VERSION)
  SCANNER_STR_ENTRY_ELEM("__GNUC_MINOR__", PREP_GNUC_MINOR)
  SCANNER_STR_ENTRY_ELEM("__GNUG__", PREP_GNUG)
  SCANNER_STR_ENTRY_ELEM("__cplusplus", PREP_CPLUS_PLUS)
  SCANNER_STR_ENTRY_ELEM("__STRICT_ANSI_", PREP_STRICT_ANSI)
  SCANNER_STR_ENTRY_ELEM("__BASE_FILE__", PREP_BASE_FILE)
  SCANNER_STR_ENTRY_ELEM("__INCLUDE_LEVEL__", PREP_INCLUDE_LEVEL)
  SCANNER_STR_ENTRY_ELEM("__VERSION__", PREP_VERSION)
  SCANNER_STR_ENTRY_ELEM("__OPTIMIZE__", PREP_OPTIMIZE)
  SCANNER_STR_ENTRY_ELEM("__REGISTER_PREFIX__", PREP_REGISTER_PREFIX)
  SCANNER_STR_ENTRY_ELEM("__USER_LABEL_PREFIX__", PREP_USER_LABEL_PREFIX)
  SCANNER_STR_ENTRY_END(predefined_macro)
  return predefined_macro;
}

int prep_parser_init(prep_parser_t *self) {
  self->psp_cur_scan = NULL;
  utillib_unordered_map_ft func = utillib_unordered_map_const_charp_ft();
  utillib_slist_init(&(self->psp_incl_stack));
  utillib_unordered_map_init_from_array(&(self->psp_macro_map), func,
                                        get_pdmacro());
  utillib_unordered_map_init_from_array(&(self->psp_directives), func,
                                        get_directives());
}

static void destroy_include_stack(utillib_slist *inc) {
  UTILLIB_SLIST_FOREACH(prep_scanner_t *, scan, inc) {
    prep_scanner_destroy(scan);
  }
  utillib_slist_destroy(inc);
}

void prep_parser_destroy(prep_parser_t *self) {
  utillib_unordered_map_destroy(&(self->psp_macro_map));
  utillib_unordered_map_destroy(&(self->psp_directives));
  destroy_include_stack(&(self->psp_incl_stack));
}

static prep_parser_t *prep_static_instance(void) {
  static prep_parser_t x;
  return &x;
}

static void prep_parser_cleanup(void) {
  prep_parser_destroy(prep_static_instance());
}

prep_parser_t *prep_static_init(void) {
  prep_parser_t *self = prep_static_instance();
  prep_parser_init(self);
  utillib_error_cleanup_func(prep_parser_cleanup);
  return self;
}
