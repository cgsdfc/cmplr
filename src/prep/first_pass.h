#ifndef PREP_FIRST_PASS_H
#define PREP_FIRST_PASS_H
#include "scanner/scanner.h"
#include "token.h" // for PREP_XXX
#include <stdio.h> // for FILE*

typedef struct prep_first_pass_t {
  scanner_base_t fps_scan;
  FILE *fps_out;
} prep_first_pass_t;

int prep_first_pass_init(prep_first_pass_t *, FILE * , FILE *);
int prep_first_pass_scan(prep_first_pass_t *);
void prep_first_pass_destroy(prep_first_pass_t *);

#endif // PREP_FIRST_PASS_H
