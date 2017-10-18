#define _GNU_SOURCE
#include "match.h"
#include "scanner.h"
#include <stdio.h>
#include <utillib/error.h>

SCANNER_MATCH_ENTRY_BEGIN(main_match)
SCANNER_MATCH_ENTRY(scanner_match_identifier, 1)
SCANNER_MATCH_ENTRY(scanner_match_string_double, 2)
SCANNER_MATCH_ENTRY(scanner_match_any_char, 0)
SCANNER_MATCH_ENTRY_END(main_match)

int main() {
  char const *filename = "./t.c";
  FILE *file = fopen(filename, "r");
  scanner_t scan;
  scanner_init(&scan, file, filename, main_match);
  int r = scanner_read_all(SCANNER_TO_BASE(&scan), stdout);
  scanner_destroy(&scan);
  exit(r);
}
