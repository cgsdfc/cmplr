#ifndef SCANNER_UTIL_H
#define SCANNER_UTIL_H

#include <ctype.h>
static int isidmiddle(int c) { return isalnum(c) || c == '_'; }

static int isidbegin(int c) { return isalpha(c) || c == '_'; }
#endif // SCANNER_UTIL_H
