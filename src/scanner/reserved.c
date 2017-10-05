// common reserved words recognition
// utilities here.
#include <string.h>
#include "reserved.h"
#include "error.h"

scanner_enum_value
SCANNER_MATCH_RESERVED_IMPL(
    scanner_str_val_pair_array array,
    scanner_string_lit lit)
{
  for (scanner_str_val_pair const* p=array;
      p->str != NULL || p->val != 0;
      ++p)
  {
    if (0 == strcmp(p->str, lit))
    {
      return p->val;
    }
  }
  return SCANNER_UNMATCHED;
}
