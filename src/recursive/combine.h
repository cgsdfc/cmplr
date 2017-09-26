#ifndef RECURSIVE_REPEATED_H
#define RECURSIVE_REPEATED_H
#include <stdbool.h>
struct parser_info;
struct parser_function;
struct pred_function;
bool repeated (struct parser_function *, struct parser_info *);
bool one_of (struct parser_info *, unsigned, ...);
bool
accept_when (struct parser_info *, struct parser_function *,
	     struct pred_function *)
#endif				// RECURSIVE_REPEATED_H
