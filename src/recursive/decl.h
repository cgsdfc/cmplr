#ifndef RECURSIVE_DECL_H
#define RECURSIVE_DECL_H
#include <stdbool.h>

struct pcontext;
#define DECL_IS_FUNC_DECLARE(FUNC)\
bool decl_is_ ## FUNC (struct pcontext * context)

DECL_IS_FUNC_DECLARE (typename_in_parenthesis);
DECL_IS_FUNC_DECLARE (declare);
DECL_IS_FUNC_DECLARE(declarator);
DECL_IS_FUNC_DECLARE(parameter_declare);
#endif // RECURSIVE_DECL_H
