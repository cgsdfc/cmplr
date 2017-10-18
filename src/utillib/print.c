#include "print.h"
#include "error.h"
#include <stdarg.h> // for va_list

void utillib_print_indent(FILE *file, size_t nchar, int ch) {
  while (nchar--) {
    fputc(ch, file);
  }
}
/* returns a local static char buffer */
char *utillib_static_buf(void) {
  static char static_buf[UTILLIB_STATIC_BUF_SIZE];
  return static_buf;
}

/* use the static buffer to return a formatted str */
/* use vsnprintf to detect buffer overflow */
char const *utillib_static_vsprintf(char const *fmt, va_list ap) {
  char *buf = utillib_static_buf();
  if (vsnprintf(buf, UTILLIB_STATIC_BUF_SIZE, fmt, ap) <
      UTILLIB_STATIC_BUF_SIZE) {
    return buf;
  }
  utillib_die("buffer overflowed");
}

char const *utillib_static_sprintf(char const *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  return utillib_static_vsprintf(fmt, ap);
}

char const *utillib_int_str(void *X) {
  return utillib_static_sprintf("%ld", (long)X);
}
