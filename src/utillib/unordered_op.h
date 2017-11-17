#ifndef UTILLIB_UNORDERED_OP_H
#define UTILLIB_UNORDERED_OP_H

#include "config.h"
#include <stddef.h> /* for size_t */
struct utillib_unordered_op {
  size_t (*hash)(void const *);
  bool (*equal)(void const *, void const *);
};

extern struct utillib_unordered_op const utillib_unordered_strop;

#endif /* UTILLIB_UNORDERED_OP_H */
