#include "utillib/enum.h"

void utillib_print_enum_range(
    FILE *file,
    e2s_function tostring,
    const char *sep,
    int from, int to)
{
  for (int i=from;i<to;++i)
  {
    fprintf(file, "%s%s", tostring(i), sep);
  }
  fputs("\n", file);
}

void
utillib_print_enum(e2s_function * tostring, int limit)
{
  utillib_print_enum_range(stdout, tostring, "\n", 0, limit);
}
