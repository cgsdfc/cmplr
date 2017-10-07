#include "input_buf.h"
#include "except.h"
#include <string.h>

UTILLIB_ETAB_BEGIN(utillib_input_buf_source)
  UTILLIB_ETAB_ELEM_INIT(INPUT_BUF_STDIN, "<stdin>")
  UTILLIB_ETAB_ELEM_INIT(INPUT_BUF_FILE, "<file>")
  UTILLIB_ETAB_ELEM_INIT(INPUT_BUF_STRING, "<string>")
UTILLIB_ETAB_END(utillib_input_buf_source)

int 
utillib_input_buf_init(utillib_input_buf *b,
    int source,
    char *arg)
{
  FILE *file;
  const char *filename;
  switch (source) {
    case INPUT_BUF_STDIN:
      file=stdin;
      filename=utillib_input_buf_source_tostring(source);
      break;
    case INPUT_BUF_FILE:
      file = fopen(arg, "r");
      filename=arg;
      break;
    case INPUT_BUF_STRING:
      file = fmemopen(arg, strlen(arg), "r");
      filename=utillib_input_buf_source_tostring(source);
      break;
    default:
      return UTILLIB_EINVAL;
  }
  if (file) {
    b->ch=fgetc(file);
    b->row=1;
    b->col=0;
    b->file=file;
    b->filename=filename;
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

utillib_position
utillib_input_buf_position(utillib_input_buf *b)
{
  utillib_position pos;
  pos.lineno=b->row;
  pos.column=b->col;
  return pos;
}

void
utillib_input_buf_destroy(utillib_input_buf *b)
{
  if (b->file != stdin) {
    fclose(b->file);
  }
}

