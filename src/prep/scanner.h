#ifndef PREP_SCANNER_H
#define PREP_SCANNER_H
#include <scanner/scanner.h> // for scanner_base_t
#include <scanner/token.h>   // scanner_token
#include <stdio.h>
#include <utillib/vector.h> // for utiilib_vector

typedef struct prep_scanner_t {
  scanner_base_t psp_scan;
  scanner_input_buf psp_buf;
  utillib_vector *psp_err;
} prep_scanner_t;

void prep_scanner_init(prep_scanner_t *, FILE *, char const *,
                       utillib_vector *);
int prep_scanner_yylex(prep_scanner_t *);
void prep_scanner_destroy(prep_scanner_t *);
#endif // PREP_SCANNER_H
