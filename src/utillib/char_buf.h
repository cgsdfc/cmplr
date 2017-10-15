#ifndef UTILLIB_CHAR_BUF_H
#define UTILLIB_CHAR_BUF_H
// abstract interface for inputting single char
#include <stdbool.h>

typedef int utillib_getc_func_t(void *);
typedef int utillib_ungetc_func_t(void *, int);
typedef bool utillib_feof_func_t(void *);
typedef int utillib_fclose_func_t(void *);

typedef struct utillib_char_buf_ft {
  utillib_getc_func_t *cb_getc;
  utillib_ungetc_func_t *cb_ungetc;
  utillib_feof_func_t *cb_feof;
  utillib_fclose_func_t *cb_close;
} utillib_char_buf_ft;

int utillib_char_buf_getc(void *, utillib_char_buf_ft const *);
int utillib_char_buf_ungetc(void *, int, utillib_char_buf_ft const *);
bool utillib_char_buf_feof(void *, utillib_char_buf_ft const *);
const utillib_char_buf_ft *utillib_get_FILE_ft(void);
void utillib_char_buf_destroy(void *, utillib_char_buf_ft const *);
#endif // UTILLIB_CHAR_BUF_H
