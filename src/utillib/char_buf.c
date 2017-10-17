#include "char_buf.h"
#include "error.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
static int utillib_FILE_ungetc_adapted(void *buf, int c) {
  return ungetc(c, buf);
}

const utillib_char_buf_ft *utillib_get_FILE_ft(void) {
  static const utillib_char_buf_ft ft = {
      .cb_getc = (utillib_getc_func_t *)fgetc,
      .cb_ungetc = (utillib_ungetc_func_t *)utillib_FILE_ungetc_adapted,
  };
  return &ft;
}

int utillib_char_buf_getc(void *buf, utillib_char_buf_ft const *ft) {
  assert(ft->cb_getc);
  return ft->cb_getc(buf);
}

int utillib_char_buf_ungetc(void *buf, int c, utillib_char_buf_ft const *ft) {
  assert(ft->cb_ungetc);
  return ft->cb_ungetc(buf, c);
}


char * utillib_char_buf_static_buf(void)
{
  static char static_buf[UTILLIB_CHAR_BUF_STATIC_BUF_SIZE];
  return static_buf;
}

char const * utillib_char_buf_static_sprintf(char const* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  char * buf=utillib_char_buf_static_buf();
  if (vsnprintf(buf, UTILLIB_CHAR_BUF_STATIC_BUF_SIZE, fmt, ap) < 
      UTILLIB_CHAR_BUF_STATIC_BUF_SIZE){
    return buf;
  } 
  utillib_die("utillib_char_buf_static_sprintf: buffer overflowed");
}
