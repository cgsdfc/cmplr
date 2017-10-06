#include "scanner/reserved.h"
#include "directive.h"
#include "predefined_macro.h"

SCANNER_MATCH_RESERVED_BEGIN(prep_match_directive)

  SCANNER_STR_VAL_PAIR(define, DR_DEFINE)
  SCANNER_STR_VAL_PAIR(undef, DR_UNDEF)
  SCANNER_STR_VAL_PAIR(if, DR_IF)
  SCANNER_STR_VAL_PAIR(ifndef, DR_IFNDEF)
  SCANNER_STR_VAL_PAIR(defined, DR_DEFINED)
  SCANNER_STR_VAL_PAIR(line, DR_LINE)
  SCANNER_STR_VAL_PAIR(error, DR_ERROR)
  SCANNER_STR_VAL_PAIR(include, DR_INCLUDE)
  SCANNER_STR_VAL_PAIR(ifdef, DR_IFDEF)
  SCANNER_STR_VAL_PAIR(assert, DR_ASSERT)
  SCANNER_STR_VAL_PAIR(cpu, DR_CPU)
  SCANNER_STR_VAL_PAIR(elif, DR_ELIF)
  SCANNER_STR_VAL_PAIR(else, DR_ELSE)
  SCANNER_STR_VAL_PAIR(ident, DR_IDENT)
  SCANNER_STR_VAL_PAIR(import, DR_IMPORT)
  SCANNER_STR_VAL_PAIR(include_next, DR_INCLUDE_NEXT)
  SCANNER_STR_VAL_PAIR(machine, DR_MACHINE)
  SCANNER_STR_VAL_PAIR(pragma, DR_PRAGMA)
  SCANNER_STR_VAL_PAIR(system, DR_SYSTEM)
  SCANNER_STR_VAL_PAIR(pragma once, DR_PRAGMA_ONCE)
  SCANNER_STR_VAL_PAIR(unassert, DR_UNASSERT)
  SCANNER_STR_VAL_PAIR(warning, DR_WARNING)

SCANNER_MATCH_RESERVED_END(prep_match_directive)


SCANNER_MATCH_RESERVED_BEGIN(prep_match_predefined_macro)
 
  SCANNER_STR_VAL_PAIR(__FILE__, PD_FILE)
  SCANNER_STR_VAL_PAIR(__LINE__, PD_LINE)
  SCANNER_STR_VAL_PAIR(__DATE__, PD_DATE)
  SCANNER_STR_VAL_PAIR(__TIME__, PD_TIME)
  SCANNER_STR_VAL_PAIR(__STDC__, PD_STDC)
  SCANNER_STR_VAL_PAIR(__STDC_VERSION__, PD_STDC_VERSION)
  SCANNER_STR_VAL_PAIR(__GNUC__, PD_GNUC)
  SCANNER_STR_VAL_PAIR(__GNUC_MINOR__, PD_GNUC_MINOR)
  SCANNER_STR_VAL_PAIR(__GNUG__, PD_GNUG)
  SCANNER_STR_VAL_PAIR(__cplusplus, PD_CPLUS_PLUS)
  SCANNER_STR_VAL_PAIR(__STRICT_ANSI__, PD_STRICT_ANSI)
  SCANNER_STR_VAL_PAIR(__BASE_FILE__, PD_BASE_FILE)
  SCANNER_STR_VAL_PAIR(__INCLUDE_LEVEL__, PD_INCLUDE_LEVEL)
  SCANNER_STR_VAL_PAIR(__VERSION__, PD_VERSION)
  SCANNER_STR_VAL_PAIR(__OPTIMIZE__, PD_OPTIMIZE)
  SCANNER_STR_VAL_PAIR(__REGISTER_PREFIX__, PD_REGISTER_PREFIX)
  SCANNER_STR_VAL_PAIR(__USER_LABEL_PREFIX__, PD_USER_LABEL_PREFIX)

SCANNER_MATCH_RESERVED_END(prep_match_predefined_macro)

