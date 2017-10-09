#include "error.h"

UTILLIB_ETAB_BEGIN(scanner_error)
UTILLIB_ETAB_ELEM_INIT(SCANNER_EOF, "end of input")
UTILLIB_ETAB_ELEM_INIT(SCANNER_UNMATCHED, "unmatched token")
UTILLIB_ETAB_END(scanner_error)

void scanner_error_print_impl(FILE *file, char const *basename,
                              utillib_position *pos, int level) {
  fprintf(file, "%s:%s:%s:", basename, utillib_position_tostring(pos),
          utillib_error_lv_tostring(level));
}

void
scanner_error_print (
