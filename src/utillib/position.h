#ifndef UTILLIB_POSITION
#define UTILLIB_POSITION
#include <stddef.h>
#include <stdio.h>

typedef struct utillib_position {
  size_t lineno;
  size_t column;
} utillib_position;

#define UTILLIB_POS_ROW(p) ((p)->lineno)
#define UTILLIB_POS_COL(p) ((p)->column)
#define UTILLIB_POS_COLUMN(p) UTILLIB_POS_COL(p)
#define UTILLIB_POS_LINENO(p) UTILLIB_POS_ROW(p)

utillib_position const *utillib_position_current(void);
#endif // LEXER_POSITION
