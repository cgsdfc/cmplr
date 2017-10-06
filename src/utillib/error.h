#include "enum.h"
#include <errno.h>

UTILLIB_ENUM_BEGIN(utillib_error)
  UTILLIB_ENUM_ELEM_INIT(UTILLIB_FILE_NOT_FOUND, ENOENT)
UTILLIB_ENUM_END(utillib_error)
