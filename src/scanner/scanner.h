#ifndef SCANNER_SCANNER_H
#define SCANNER_SCANNER_H
#include "utillib/input_buf.h"
#include <stddef.h>

typedef struct scanner_base {
  utillib_input_buf buf;
  utillib_vector separators;

} scanner_base;

#endif
