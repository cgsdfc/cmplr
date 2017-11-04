#include "unordered_op.h"
#include "hash.h"
#include "equal.h"

struct utillib_unordered_op * utillib_unordered_op_get_c_str(void)
{
	static struct utillib_unordered_op static_op = {
		.hash = (void*) utillib_hash_c_str,
		.equal = (void*) utillib_equal_c_str,
	};
	return &static_op;
}

