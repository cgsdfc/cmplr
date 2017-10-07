#include "position.h"
#include <assert.h>

char const *
utillib_position_tostring(utillib_position *pos) 
{
  static char buf[100];
  snprintf(buf, sizeof buf, "%lu:%lu:", pos->lineno, pos->column);
  return buf;
}

