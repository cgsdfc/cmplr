#include "scanner.h"
#include <stddef.h>
#include "utillib/enum.h"
typedef ptrdiff_t (match_function) (scanner_string);
typedef int (char_predicate) (int);

UTILLIB_ENUM_BEGIN(scanner_char)
  UTILLIB_ENUM_ELEM_INIT(CH_UNDERSCORE, '_')
  UTILLIB_ENUM_ELEM_INIT(CH_0, '0')
  UTILLIB_ENUM_ELEM_INIT(CH_x, 'x')
  UTILLIB_ENUM_ELEM_INIT(CH_BACKSLASH, '\\')
  UTILLIB_ENUM_ELEM_INIT(CH_DQUOTE, '\"')
UTILLIB_ENUM_END(scanner_char)


ptrdiff_t scanner_match_hexdecimal(scanner_string);
ptrdiff_t scanner_match_decimal(scanner_string);
ptrdiff_t scanner_match_identifier(scanner_string);
ptrdiff_t scanner_match_octal(scanner_string);
