#ifndef UTILLIB_CONFIG_H
#define UTILLIB_CONFIG_H

#ifdef __MSC_VER
typedef int bool
#define true(1)
#define false(0)
#else
#include <stdbool.h>
#endif

#endif /* UTILLIB_CONFIG_H */
