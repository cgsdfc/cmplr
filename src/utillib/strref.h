#ifndef UTILLIB_STRREF_H
#define UTILLIB_STRREF_H
#include "unordered_map.h"

typedef struct utillib_strref {
  utillib_unordered_map strref_map;
} utillib_strref;

void utillib_strref_init(utillib_strref *);
char const *utillib_strref_incr(utillib_strref *, char const *);
void utillib_strref_decr(utillib_strref *, char const *);
void utillib_strref_destroy(utillib_strref *);

#endif // UTILLIB_STRREF_H
