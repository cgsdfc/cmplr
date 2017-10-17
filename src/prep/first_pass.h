#ifndef PREP_FIRST_PASS_H
#define PREP_FIRST_PASS_H
#include <scanner/scanner.h>
#include <utillib/input_buf.h>
#include <utillib/vector.h>

typedef struct prep_first_pass_t {
  utillib_input_buf fps_buf;
  scanner_base_t fps_scan;
  utillib_vector *fps_err;
} prep_first_pass_t;

int prep_first_pass_init(prep_first_pass_t *, char *, utiilib_vector*);
int prep_first_pass_getc(prep_first_pass_t *);
int prep_first_pass_ungetc(prep_first_pass_t*, int);
void prep_first_pass_destroy(prep_first_pass_t *);
utillib_char_buf_ft const * prep_first_pass_getft(void);

#endif // PREP_FIRST_PASS_H
