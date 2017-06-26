#include "type_qualifier.h"

bool parse_type_qualifier(token_type tkt, type_qualifier *qualifier)
{
  switch (tkt) {
    case TKT_KW_CONST:
      *qualifier=TQF_CONST;
      return true;
    case TKT_KW_VOLATILE:
      *qualifier=TQF_VOLATILE;
      return true;
    default:
      return false;
  }
}
