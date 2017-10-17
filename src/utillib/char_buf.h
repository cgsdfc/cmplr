#ifndef UTILLIB_CHAR_BUF_H
#define UTILLIB_CHAR_BUF_H
// abstract interface for inputting single char
#include <stdbool.h>
#define UTILLIB_CHAR_BUF_STATIC_BUF_SIZE BUFSIZ
typedef int utillib_getc_func_t(void *);
typedef int utillib_ungetc_func_t(void *, int);

typedef struct utillib_char_buf_ft {
  utillib_getc_func_t *cb_getc;
  utillib_ungetc_func_t *cb_ungetc;
} utillib_char_buf_ft;

int utillib_char_buf_getc(void *, utillib_char_buf_ft const *);
int utillib_char_buf_ungetc(void *, int, utillib_char_buf_ft const *);
const utillib_char_buf_ft *utillib_get_FILE_ft(void);
char * utillib_char_buf_static_buf(void);
char const * utillib_char_buf_static_sprintf(char const*, ...);
#endif // UTILLIB_CHAR_BUF_H
