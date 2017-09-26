#include <stdbool.h>

struct pcontext;
#define DECL_IS_FUNC_DECLARE(FUNC) \
bool decl_is_ ## FUNC (struct pcontext * context)

DECL_IS_FUNC_DECLARE (typename_in_parenthesis);
