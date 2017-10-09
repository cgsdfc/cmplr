#ifndef RECURSIVE_TERMINAL_H
#define RECURSIVE_TERMINAL_H

#include "lexer/lexer.h"
#include "recursive/node_base.h"
#include <stdbool.h>

#define TERMINAL_DECLARE_PRED(FUNC) bool terminal_is_##FUNC(Token *t)

#define TERMINAL_DEFINE_PRED(FUNC, TYPE)                                       \
  bool terminal_is_##FUNC(Token *t) { return TOKEN_TYPE(t) == (TYPE); }

TERMINAL_DECLARE_PRED(braceL);
TERMINAL_DECLARE_PRED(braceR);
TERMINAL_DECLARE_PRED(identifier);
TERMINAL_DECLARE_PRED(while);
TERMINAL_DECLARE_PRED(else);
TERMINAL_DECLARE_PRED(type_qualifier);
TERMINAL_DECLARE_PRED(primary);
TERMINAL_DECLARE_PRED(storage_specifier);
TERMINAL_DECLARE_PRED(parenthesisR);
TERMINAL_DECLARE_PRED(parenthesisL);
TERMINAL_DECLARE_PRED(additive_op);
TERMINAL_DECLARE_PRED(unary_op);
TERMINAL_DECLARE_PRED(timing_op);
TERMINAL_DECLARE_PRED(shift_op);
TERMINAL_DECLARE_PRED(relation_op);
TERMINAL_DECLARE_PRED(equality_op);
TERMINAL_DECLARE_PRED(bit_and_op);
TERMINAL_DECLARE_PRED(bit_xor_op);
TERMINAL_DECLARE_PRED(bit_or_op);
TERMINAL_DECLARE_PRED(log_and_op);
TERMINAL_DECLARE_PRED(log_or_op);
TERMINAL_DECLARE_PRED(colon);
TERMINAL_DECLARE_PRED(comma);
TERMINAL_DECLARE_PRED(semicolon);
TERMINAL_DECLARE_PRED(assign_op);
TERMINAL_DECLARE_PRED(question);
TERMINAL_DECLARE_PRED(type_qualifier);
TERMINAL_DECLARE_PRED(storage_specifier);
TERMINAL_DECLARE_PRED(integral_keyword);

#endif // RECURSIVE_TERMINAL_H
