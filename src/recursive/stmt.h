#ifndef RECURSIVE_STMT_H
#define RECURSIVE_STMT_H
#include "lexer/lexer.h"

#define STMT_IS_FUNC_DECLARE(FUNC) \
bool stmt_is_ ## FUNC(Lexer *lexer)

STMT_IS_FUNC_DECLARE (jump);
STMT_IS_FUNC_DECLARE (iterate);
STMT_IS_FUNC_DECLARE (select);
STMT_IS_FUNC_DECLARE (label);
STMT_IS_FUNC_DECLARE (compound);
STMT_IS_FUNC_DECLARE (exprstmt);
STMT_IS_FUNC_DECLARE (statement);
STMT_IS_FUNC_DECLARE (_);
STMT_IS_FUNC_DECLARE (_);
STMT_IS_FUNC_DECLARE (_);
#endif // RECURSIVE_STMT_H
