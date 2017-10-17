#ifndef PREP_SECOND_PASS_H
#define PREP_SECOND_PASS_H
#include "token.h"
#include "first_pass.h"
#include <scanner/scanner.h>

typedef struct prep_second_pass_t {
  scanner_base_t psc_scan;
  prep_first_pass_t psc_first;
} prep_second_pass_t;

int prep_second_pass_init(prep_second_pass_t *, char *);
void prep_second_pass_destroy(prep_second_pass_t *);
#endif // PREP_SECOND_PASS_H
