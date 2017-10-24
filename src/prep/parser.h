#ifndef PREP_SECOND_PASS_H
#define PREP_SECOND_PASS_H
#include "scanner.h"
#include <utillib/slist.h>
#include <utillib/unordered_map.h>
#include <utillib/vector.h>
typedef struct prep_arg_t prep_arg_t;
typedef struct prep_macro_entry_t {

} prep_macro_entry_t;

typedef struct prep_parser_t {
  prep_scanner_t *psp_cur_scan;
  utillib_slist psp_incl_stack;
  utillib_unordered_map psp_macro_map;
  utillib_unordered_map psp_directives;
  utillib_vector psp_errors;
} prep_parser_t;

void prep_parser_init(prep_parser_t *, prep_arg_t *);
void prep_parser_destroy(prep_parser_t *);
#endif // PREP_SECOND_PASS_H
