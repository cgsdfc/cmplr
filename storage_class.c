#include "storage_class.h"


bool parse_storage_class(storage_class *stcl)
{
  switch (TOKEN_TYPE(tk)) 
  {
    case tkt_kw_auto:
      *stcl=STCL_AUTO;
      return true;
    case tkt_kw_extern:
      *stcl=STCL_EXTERN;
      return true;
    case TKT_KW_Static:
      *stcl=STCL_TYPEDEF;
      return true;
    default:
      return false;
  }
}


