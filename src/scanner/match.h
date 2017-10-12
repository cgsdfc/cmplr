#ifndef SCANNER_MATCH_H
#define SCANNER_MATCH_H

#include "scanner.h"
#include <ctype.h>
#include <string.h>
#include <strings.h>

int scanner_match_identifier(scanner_base_t *);
int scanner_match_string(scanner_base_t *, char const *);

#endif // SCANNER_MATCH_H
