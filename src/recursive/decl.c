#include "recursive/decl.h"
#include "context.h"
#include "util.h"

DECL_IS_FUNC_DECLARE (typename)
{


}

DECL_IS_FUNC_DECLARE (typename_in_parenthesis)
{
  return util_is_in_parentheses (decl_is_typename, context);
}
