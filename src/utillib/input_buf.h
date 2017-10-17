#ifndef UTILLIB_INPUT_BUF_H
#define UTILLIB_INPUT_BUF_H
#include "char_buf.h"
#include "position.h"
#include "vector.h"
#include "error.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#define INPUT_BUF_STDIN  "<stdin>"
#define INPUT_BUF_STRING  "<string>"
#define UTILLIB_PATH_MAX 1024

typedef struct utillib_input_buf {
  utillib_vector line_offset;
  const char *filename;
  FILE *file;
  size_t col;
  size_t row;
} utillib_input_buf;

/* constructor, destructor */
void utillib_input_buf_init(utillib_input_buf *, FILE *, char const*);
void utillib_input_buf_destroy(utillib_input_buf *);

/* file function */
long utillib_input_buf_ftell(utillib_input_buf *);
bool utillib_input_buf_feof(utillib_input_buf *);
int utillib_input_buf_getc(utillib_input_buf *);
int utillib_input_buf_ungetc(utillib_input_buf *, int);

/* file info access */
const char *utillib_input_buf_filename(utillib_input_buf *);
size_t utillib_input_buf_row(utillib_input_buf *);
size_t utillib_input_buf_col(utillib_input_buf *);
utillib_position const * utillib_input_buf_current_pos(utillib_input_buf*);

/* general char_buf interface */
const struct utillib_char_buf_ft *utillib_input_buf_getft(void);

/* error message display */
void utillib_input_buf_pretty_perror(utillib_input_buf *,
    utillib_error *);

#endif // UTILLIB_INPUT_BUF_H
