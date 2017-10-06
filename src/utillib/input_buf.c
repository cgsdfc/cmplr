#include "input_buf.h"
#include "error.h"

#include <string.h>

// if filename == NULL, open stdin and read from it
int 
utillib_input_buf_init(utillib_input_buf *b,
    const char *filename)
{
  memset (b, 0, sizeof *b);
  if (filename == NULL) {
    b->file=stdin;
    b->filename="<stdin>";
    return 0;
  }
  FILE *file = fopen(filename, "r");
  if (file) {
    b->file=file;
    b->filename=filename;
    b->ch=fgetc(file);
    b->row=1;
    return 0;
  }
  return UTILLIB_FILE_NOT_FOUND;
}

int
utillib_input_buf_getchar(utillib_input_buf *b)
{
  int ch=b->ch;
  b->ch=fgetc(b->file);
  switch (ch) {
    case '\n':
      b->row++;
      b->col=0;
      break;
    case EOF:
      return EOF;
    default:
      b->col++;
      break;
  }
  return ch;
}

int
utillib_input_buf_ungetc(utillib_input_buf *b)
{
  switch (b->ch)
  {
    case '\n':
      b->row--;
      break;
    default:
      b->col--;
      break;
  }
  return ungetc(b->ch,b->file);
}

const char *
utillib_input_buf_filename(utillib_input_buf *b)
{
  return b->filename;
}

size_t 
utillib_input_buf_row(utillib_input_buf *b)
{
  return b->row;
}

size_t
utillib_input_buf_col(utillib_input_buf *b)
{
  return b->col;
}

void
utillib_input_buf_destroy(utillib_input_buf *b)
{
  if (b->file != stdin) {
    fclose(b->file);
  }
}

