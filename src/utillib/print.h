#ifndef UTILLIB_PRINT_H
#define UTILLIB_PRINT_H
#include <stdarg.h> // for va_list
#include <stdio.h>
#define UTILLIB_STATIC_BUF_SIZE BUFSIZ
typedef char const *(utillib_tostring_func_t)(void *);
void utillib_print_indent(FILE *, size_t, int);
char *utillib_static_buf(void);
char const *utillib_static_vsprintf(char const *fmt, va_list ap);
char const *utillib_static_sprintf(char const *fmt, ...);
char const *utillib_int_str(void *);
#endif // UTILLIB_PRINT_H
