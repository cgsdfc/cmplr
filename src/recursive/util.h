#ifndef RECURSIVE_UTIL_H
#define RECURSIVE_UTIL_H

#include <stdbool.h>
#include "lexer/lexer.h"
struct pcontext;
typedef bool (tfunction) (Token *);
typedef bool (pfunction) (struct pcontext *);
bool util_is_optional (struct pcontext *, pfunction *);
bool util_is_list (struct pcontext *, pfunction *, bool /* allow_empty */ );
bool util_is_sequence (struct pcontext *, ...);
bool util_is_one_of (struct pcontext *, ...);
bool util_is_in_parentheses (struct pcontext *, pfunction *);
bool util_is_in_braces (struct pcontext *, pfunction *);
bool util_is_in_brackets (struct pcontext *, pfunction *);
bool util_is_terminal (struct pcontext *, TokenType tt, bool /* pushing */ );
bool util_is_terminal_pred (struct pcontext *, tfunction *
			    pred, bool /* pushing */ );
Token *util_read_first_token (struct pcontext *);
void util_shift_one_token (struct pcontext *);
bool util_is_identifier (struct pcontext *);
bool util_is_semicolon (struct pcontext *);
bool util_is_nonshortcut_or (struct pcontext *, pfunction *, pfunction *);
bool util_is_colon (struct pcontext *);
bool util_is_comma (struct pcontext *);
bool util_is_question (struct pcontext *);
void util_push_node_null (struct pcontext *);
bool util_is_separated_list (struct pcontext *, TokenType, pfunction *,
			     bool /* allow_empty */ );
bool util_is_comma_sep_list (struct pcontext *, pfunction *, bool);
bool util_is_braceL (struct pcontext *);
bool util_is_braceR (struct pcontext *);
bool util_is_bracketL (struct pcontext *);
bool util_is_bracketR (struct pcontext *);
bool util_is_parenthesisL (struct pcontext *);
bool util_is_parenthesisR (struct pcontext *);
int /* construct */ util_token_type_to_construct (int /* token_type */ ,
						  int /* node_tag */ );

#endif // RECURSIVE_UTIL_H
