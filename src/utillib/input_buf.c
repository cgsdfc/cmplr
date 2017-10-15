#include "input_buf.h"
#include "error.h"
#include "except.h"
#include <string.h>
#include <unistd.h>

UTILLIB_ETAB_BEGIN(utillib_input_buf_source)
UTILLIB_ETAB_ELEM_INIT(INPUT_BUF_STDIN, "<stdin>")
UTILLIB_ETAB_ELEM_INIT(INPUT_BUF_FILE, "<file>")
UTILLIB_ETAB_ELEM_INIT(INPUT_BUF_STRING, "<string>")
UTILLIB_ETAB_END(utillib_input_buf_source)

const utillib_char_buf_ft *utillib_input_buf_getft(void) {
  static const utillib_char_buf_ft ft = {
      .cb_getc = (utillib_getc_func_t *)utillib_input_buf_getc,
      .cb_ungetc = (utillib_ungetc_func_t *)utillib_input_buf_ungetc,
      .cb_feof = (utillib_feof_func_t *)utillib_input_buf_feof,
      .cb_close = (utillib_fclose_func_t *)utillib_input_buf_destroy,
  };
  return &ft;
}

bool utillib_input_buf_feof(utillib_input_buf *b) { return feof(b->file); }

int utillib_input_buf_init(utillib_input_buf *b, char *arg, int source) {
  FILE *file;
  const char *filename;
  switch (source) {
  case INPUT_BUF_STDIN:
    file = stdin;
    filename = utillib_input_buf_source_tostring(source);
    break;
  case INPUT_BUF_FILE:
    file = fopen(arg, "r");
    filename = arg;
    break;
  case INPUT_BUF_STRING:
    file = fmemopen(arg, strlen(arg), "r");
    filename = utillib_input_buf_source_tostring(source);
    break;
  default:
    return UTILLIB_EINVAL;
  }
  if (file) {
    b->row = 1;
    b->col = 0;
    b->file = file;
    b->filename = filename;
    return 0;
  }
  return UTILLIB_FILE_NOT_FOUND;
}

int utillib_input_buf_getc(utillib_input_buf *b) {
  int ch = fgetc(b->file);
  switch (ch) {
  case '\n':
    b->row++;
    b->col = 0;
    break;
  case EOF:
    return EOF;
  default:
    b->col++;
    break;
  }
  return ch;
}

int utillib_input_buf_ungetc(utillib_input_buf *b, int ch) {
  switch (ch) {
  case '\n':
    b->row--;
    break;
  default:
    b->col--;
    break;
  }
  return ungetc(ch, b->file);
}

const char *utillib_input_buf_filename(utillib_input_buf *b) {
  return b->filename;
}

size_t utillib_input_buf_row(utillib_input_buf *b) { return b->row; }

size_t utillib_input_buf_col(utillib_input_buf *b) { return b->col; }

utillib_position utillib_input_buf_position(utillib_input_buf *b) {
  utillib_position pos;
  pos.lineno = b->row;
  pos.column = b->col;
  return pos;
}

void utillib_input_buf_destroy(utillib_input_buf *b) {
  if (b->file != stdin) {
    fclose(b->file);
  }
}

// print the (path:row:col:lv) 4 tuple to FILE
char const *utillib_input_buf_tostring(utillib_input_buf *b, char *buf,
                                       size_t size, int lv) {
  char *absdir = getcwd(buf, sizeof buf);
  int nwrite = snprintf(buf, size, "%s/%s:%lu:%lu:%s", absdir, b->filename,
                        b->row, b->col, utillib_error_lv_tostring(lv));
  return nwrite >= size ? NULL : buf;
}

void utillib_input_buf_perror(utillib_input_buf *b, char const *errmsg) {
  static char buf[BUFSIZ]; // 4096
  char const *path_row_col_lv =
      utillib_input_buf_tostring(b, buf, BUFSIZ, ERROR_LV_ERROR);
  if (path_row_col_lv) {
    fprintf(stderr, "%s %s\n", path_row_col_lv, errmsg);
  } else {
    utillib_die("static buffer overflow");
  }
}

long utillib_input_buf_ftell(utillib_input_buf *b)
{
  return ftell(b->file);
}
