#ifndef PREP_SECOND_PASS_H
#define PREP_SECOND_PASS_H
#include "token.h"
#include <utillib/unordered_map.h>
#include <utillib/slist.h>
#include <scanner/scanner.h>

typedef struct prep_macro_entry_t {

} prep_macro_entry_t;

typedef struct prep_second_pass_t {
  scanner_base_t * cur_scan;
  utillib_slist incl_stack;
  utillib_unordered_map macro_map;
} prep_second_pass_t;

prep_second_pass_t * prep_second_pass_init_static(char const*);
#endif // PREP_SECOND_PASS_H
