#include "unordered_op.h"
#include "equal.h"
#include "hash.h"

struct utillib_unordered_op const utillib_unordered_strop = {
    .hash = (void *)utillib_hash_c_str, .equal = (void *)utillib_equal_c_str,
};
