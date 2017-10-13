#ifndef SCANNER_MATCH_H
#define SCANNER_MATCH_H

#include "scanner.h"
#include <ctype.h>
#include <string.h>
#include <strings.h>

int scanner_match_identifier(scanner_base_t *);
int scanner_match_string_double(scanner_base_t *);
int scanner_match_string_single(scanner_base_t *);
int scanner_match_string_angle(scanner_base_t *);
int scanner_skip_space(scanner_base_t *);
int scanner_skip_cpp_comment(scanner_base_t *);
int scanner_skip_c_comment(scanner_base_t *);
#endif // SCANNER_MATCH_H
