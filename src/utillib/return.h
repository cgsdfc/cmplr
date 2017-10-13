#include "enum.h"

UTILLIB_ENUM_BEGIN(utillib_return_value)
UTILLIB_ENUM_ELEM(RETURN_ACCEPT) // accept the input
UTILLIB_ENUM_ELEM(RETURN_REJECT) // reject the input
UTILLIB_ENUM_ELEM(RETURN_ERROR)  // error when parsing
UTILLIB_ENUM_END(utillib_return_value)
