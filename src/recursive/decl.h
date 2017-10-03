#ifndef RECURSIVE_DECL_H
#define RECURSIVE_DECL_H
#include <stdbool.h>
#include "recursive/context.h"
#include "recursive/util.h"
#include "recursive/terminal.h"
#include "recursive/error.h"

struct pcontext;
#define DECL_IS_FUNC_DECLARE(FUNC)\
bool decl_is_ ## FUNC (struct pcontext * context)

DECL_IS_FUNC_DECLARE (translation);
DECL_IS_FUNC_DECLARE (external);
DECL_IS_FUNC_DECLARE (declare);
DECL_IS_FUNC_DECLARE (function);
DECL_IS_FUNC_DECLARE (typename);
DECL_IS_FUNC_DECLARE (typename_in_parenthesis);
DECL_IS_FUNC_DECLARE (optional_comma);
DECL_IS_FUNC_DECLARE (assign);
#endif // RECURSIVE_DECL_H
