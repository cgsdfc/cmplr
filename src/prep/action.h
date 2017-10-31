#ifndef PREP_ACTION_H
#define PREP_ACTION_H
#include "parser.h"
#include "scanner.h"

void prep_act_parser_push_error(prep_parser_t *, int, char const *, ...);
void prep_act_scanner_push_error(prep_scanner_t *, char const *)
#endif				// PREP_ACTION_H
