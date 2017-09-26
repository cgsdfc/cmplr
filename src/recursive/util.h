#include <stdbool.h>
#include "lexer/lexer.h"
struct pcontext;
typedef bool (pfunction) (struct pcontext *);
typedef struct util_surround
{
  TokenType left, right;
} util_surround;

bool util_is_optional (pfunction * parse, struct pcontext *context);
bool util_is_list (pfunction * parse, struct pcontext *context);
bool util_is_sequence (struct pcontext *context, ...);
bool util_is_one_of (struct pcontext *context, ...);
bool util_is_in_parentheses (pfunction * parse, struct pcontext *context);
bool util_is_in_braces (pfunction * parse, struct pcontext *context);
bool util_is_in_brackets (pfunction * parse, struct pcontext *context);
bool util_is_terminal (struct pcontext *context, TokenType tt);
