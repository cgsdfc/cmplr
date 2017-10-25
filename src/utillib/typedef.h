#ifndef UTILLIB_TYPEDEF_H
#define UTILLIB_TYPEDEF_H
#include <stdbool.h>
#include <stddef.h>

typedef void(utillib_destroy_func_t)(void *);
typedef void *utillib_element_t;
typedef utillib_element_t *utillib_element_pointer_t;
typedef void const *utillib_key_t;
typedef void *utillib_value_t;
typedef size_t(utillib_hash_func_t)(utillib_key_t);
typedef bool(utillib_equal_func_t)(utillib_key_t, utillib_key_t);

#endif // UTILLIB_TYPEDEF_H
