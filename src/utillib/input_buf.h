#ifndef UTILLIB_INPUT_BUF_H
#define UTILLIB_INPUT_BUF_H
#include "char_buf.h"
#include "enum.h"
#include "position.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
UTILLIB_ENUM_BEGIN(utillib_input_buf_source)
UTILLIB_ENUM_ELEM(INPUT_BUF_STDIN)
UTILLIB_ENUM_ELEM(INPUT_BUF_FILE)
UTILLIB_ENUM_ELEM(INPUT_BUF_STRING)
UTILLIB_ENUM_END(utillib_input_buf_source)

typedef struct utillib_input_buf {
  const char *filename;
  FILE *file;
  size_t col;
  size_t row;
} utillib_input_buf;

bool utillib_input_buf_feof(utillib_input_buf *);
int utillib_input_buf_init(utillib_input_buf *, int, char *);
int utillib_input_buf_getc(utillib_input_buf *);
int utillib_input_buf_ungetc(utillib_input_buf *, int);
void utillib_input_buf_destroy(utillib_input_buf *);
const char *utillib_input_buf_filename(utillib_input_buf *);
size_t utillib_input_buf_row(utillib_input_buf *);
size_t utillib_input_buf_col(utillib_input_buf *);
utillib_position utillib_input_buf_position(utillib_input_buf *);
const utillib_char_buf_ft *utillib_input_buf_ft(void);
void utillib_input_buf_perror(utillib_input_buf *, FILE *, int);
#endif // UTILLIB_INPUT_BUF_H
