#ifndef UTILLIB_UNION_H
#define UTILLIB_UNION_H
typedef union utillib_union_val_t {
	void *as_voidp;
	size_t as_size_t;
	long as_long;
} utillib_union_val_t;
#endif				// UTILLIB_UNION_H
