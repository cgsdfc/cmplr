#ifndef UTILLIB_UNORDERED_OP_H
#define UTILLIB_UNORDERED_OP_H

#include <stddef.h>		// for size_t
#include <stdbool.h>
struct utillib_unordered_op {
  size_t (*hash) (void const *);
  bool (*equal) (void const *, void const *);
};

struct utillib_unordered_op * utillib_unordered_op_get_c_str(void);

#endif // UTILLIB_UNORDERED_OP_H
