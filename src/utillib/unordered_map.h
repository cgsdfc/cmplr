#ifndef UTILLIB_UNORDERED_MAP_H
#define UTILLIB_UNORDERED_MAP_H 
#include <stdbool.h>
#include <stdlib.h> 
#include <stddef.h>
#include "enum.h"
#include "vector.h"
#define UTILLIB_PAIR_FIRST(P) ((*(P))[0])
#define UTILLIB_PAIR_SECOND(P) ((*(P))[1])

typedef void const *utillib_key_t;
typedef void const *utillib_value_t;
typedef size_t (utillib_hash_func_t) (void const*);
typedef bool (utillib_equal_func_t) (void const*, void const*);
typedef void const* utillib_pair_t[2];

UTILLIB_ENUM_BEGIN(utillib_unordered_map_retval_t)
  // for mode find_only
  UTILLIB_ENUM_ELEM(KEY_FOUND)
  UTILLIB_ENUM_ELEM(KEY_MISSING)
  // for mode force_insert
  UTILLIB_ENUM_ELEM(KEY_EXISTS)
  UTILLIB_ENUM_ELEM(KEY_INSERTED)
UTILLIB_ENUM_END(utillib_unordered_map_retval_t)

typedef struct utillib_unordered_ft
{
  utillib_hash_func_t * un_hash;
  utillib_equal_func_t * un_equal;
} utillib_unordered_ft;

typedef struct utillib_unordered_map
{
  utillib_unordered_ft * un_ft;
  utillib_vector un_bucket;
  size_t un_capacity;
} utillib_unordered_map;

void utillib_unordered_map_init(utillib_unordered_map *, utillib_unordered_ft *);
void utillib_unordered_map_destroy(utillib_unordered_map*);
int utillib_unordered_map_insert(utillib_unordered_map *, utillib_key_t, utillib_value_t);
int utillib_unordered_map_erase(utillib_unordered_map *, utillib_key_t);
utillib_pair_t * utillib_unordered_map_find(utillib_unordered_map *, utillib_key_t);
utillib_unordered_ft const* utillib_unordered_map_const_charp_ft(void);
#endif // UTILLIB_UNORDERED_MAP_H
