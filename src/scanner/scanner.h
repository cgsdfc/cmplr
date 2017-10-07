#ifndef SCANNER_SCANNER_H
#define SCANNER_SCANNER_H
#include <stddef.h>
#include "utillib/input_buf.h"

typedef struct scanner_base
{
  utillib_input_buf buf;
  utillib_vector separators;

} scanner_base;

#endif
