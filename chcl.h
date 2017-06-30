#ifndef CHAR_CLASS_H
#define CHAR_CLASS_H 1
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "dfa.h"

int alloc_char_class(const char *chcl);
int cond_char_class(dfa_state*,int);

#endif

