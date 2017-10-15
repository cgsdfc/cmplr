#include "char_buf.h"
#include "error.h"
#include <assert.h>
#include <stdio.h>
static int utillib_FILE_ungetc_adapted(void *buf, int c) {
  return ungetc(c, buf);
}

const utillib_char_buf_ft *utillib_get_FILE_ft(void) {
  static const utillib_char_buf_ft ft = {
      .cb_getc = (utillib_getc_func_t *)fgetc,
      .cb_ungetc = (utillib_ungetc_func_t *)utillib_FILE_ungetc_adapted,
      .cb_feof = (utillib_feof_func_t *)feof,
      .cb_close = (utillib_fclose_func_t *)fclose,
  };
  return &ft;
}

int utillib_char_buf_getc(void *buf, utillib_char_buf_ft const *ft) {
  assert(ft->cb_getc);
  return ft->cb_getc(buf);
}

bool utillib_char_buf_feof(void *buf, utillib_char_buf_ft const *ft) {
  return ft->cb_feof(buf);
}

int utillib_char_buf_ungetc(void *buf, int c, utillib_char_buf_ft const *ft) {
  assert(ft->cb_ungetc);
  return ft->cb_ungetc(buf, c);
}

void utillib_char_buf_destroy(void *buf, utillib_char_buf_ft const *ft) {
  int r = ft->cb_close(buf);
  if (r != 0) {
    utillib_die("cannot close char buffer");
  }
}
