#ifndef SCANNER_RESERVED_H
#define SCANNER_RESERVED_H
#include "scanner.h"

typedef struct scanner_str_val_pair
{
  scanner_string_lit str;
  scanner_enum_value val;
} scanner_str_val_pair;

typedef scanner_str_val_pair const scanner_str_val_pair_array[];

#define SCANNER_STR_VAL_PAIR(STR, VAL) { .str=# STR, .val=(VAL) },
#define SCANNER_MATCH_RESERVED_DECL(NAME) scanner_enum_value NAME(scanner_string_lit lit)
#define SCANNER_MATCH_RESERVED_BEGIN(NAME) SCANNER_MATCH_RESERVED_DECL(NAME) {\
  static scanner_str_val_pair_array NAME ## _tab = {
#define SCANNER_MATCH_RESERVED_IMPL scanner_match_reserved
#define SCANNER_MATCH_RESERVED_END(NAME) (scanner_string_lit)NULL, (scanner_enum_value) 0};\
  return SCANNER_MATCH_RESERVED_IMPL(NAME ## _tab, lit); }

scanner_enum_value SCANNER_MATCH_RESERVED_IMPL(
    scanner_str_val_pair_array array,
    scanner_string_lit lit);

#endif // SCANNER_RESERVED_H
