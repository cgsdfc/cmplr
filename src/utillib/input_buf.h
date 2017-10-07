#ifndef UTILLIB_INPUT_BUF_H
#define UTILLIB_INPUT_BUF_H
#include <stdio.h>
#include <stddef.h>
#include "position.h"
#include "enum.h"
UTILLIB_ENUM_BEGIN(utillib_input_buf_source)
  UTILLIB_ENUM_ELEM(INPUT_BUF_STDIN)
  UTILLIB_ENUM_ELEM(INPUT_BUF_FILE)
  UTILLIB_ENUM_ELEM(INPUT_BUF_STRING)
UTILLIB_ENUM_END(utillib_input_buf_source)

typedef struct utillib_input_buf
{
  const char * filename;
  FILE * file;
  size_t col;
  size_t row;
  int ch;
} utillib_input_buf;

int utillib_input_buf_init(utillib_input_buf*, int, char *);
int utillib_input_buf_getchar(utillib_input_buf *);
void utillib_input_buf_destroy(utillib_input_buf *);
const char * utillib_input_buf_filename(utillib_input_buf *);
size_t utillib_input_buf_row(utillib_input_buf *);
size_t utillib_input_buf_col(utillib_input_buf *);
utillib_position utillib_input_buf_position(utillib_input_buf *);
#endif // UTILLIB_INPUT_BUF_H
