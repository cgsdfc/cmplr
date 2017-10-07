#include "scanner.h"

int scanner_scan(scanner_base *s)
{
  int ch;
  while ((ch=utillib_input_buf_getchar(&(s->buf)))!=EOF)
  {
    size_t size=utillib_vector_size(&(s->sep));
    for (int i=0;i<size;++i) {
      scanner_separator *sep=utillib_vector_at(&(s->sep), i);
      switch (sep(&(s->buf, &retval))) {
        case SCANNER_SKIP:
          break;
        case SCANNER_UNMATCHED:
          continue;
        case SCANNER_RETURN:
          return retval;
      }
  }
}
