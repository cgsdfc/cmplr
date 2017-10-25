#ifndef UTILLIB_UNORDERED_MAP_H
#define UTILLIB_UNORDERED_MAP_H
#include "enum.h"
#include "slist.h"
#include "typedef.h"
#include "vector.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#define UTILLIB_PAIR_FIRST(P) ((P)->up_first)
#define UTILLIB_PAIR_SECOND(P) ((P)->up_second)

typedef struct utillib_pair_t {
  void const *up_first;
  void *up_second;
} utillib_pair_t;

UTILLIB_ENUM_BEGIN(utillib_unordered_map_retval_t)
// for mode find_only
UTILLIB_ENUM_ELEM(KEY_FOUND)
UTILLIB_ENUM_ELEM(KEY_MISSING)
// for mode force_insert
UTILLIB_ENUM_ELEM(KEY_EXISTS)
UTILLIB_ENUM_ELEM(KEY_INSERTED)
UTILLIB_ENUM_END(utillib_unordered_map_retval_t)

typedef struct {
  utillib_hash_func_t *un_hash;
  utillib_equal_func_t *un_equal;
} const *utillib_unordered_map_ft, utillib_unordered_map_ft_impl;

typedef struct utillib_unordered_map {
  utillib_unordered_map_ft un_ft;
  utillib_vector un_bucket;
  double un_max_lf;
  size_t un_nbucket;
  size_t un_size;
  // manage memory of utillib_pair_t
  utillib_pair_t *un_free;
} utillib_unordered_map;

typedef struct utillib_unordered_map_iterator {
  utillib_vector_iterator iter_slot;
  utillib_slist_iterator iter_node;
} utillib_unordered_map_iterator;

/* utillib_unordered_map_iterator */
void utillib_unordered_map_iterator_init(utillib_unordered_map_iterator *,
                                         utillib_unordered_map *);
bool utillib_unordered_map_iterator_has_next(utillib_unordered_map_iterator *);
void utillib_unordered_map_iterator_next(utillib_unordered_map_iterator *);
utillib_pair_t *
utillib_unordered_map_iterator_get(utillib_unordered_map_iterator *);

/* constructor destructor */
void utillib_unordered_map_init_from_array(
    utillib_unordered_map *, utillib_unordered_map_ft,
    utillib_pair_t const *); // NULL teminated
void utillib_unordered_map_init(utillib_unordered_map *,
                                utillib_unordered_map_ft);
void utillib_unordered_map_destroy(utillib_unordered_map *);
void utillib_unordered_map_destroy_owning(utillib_unordered_map *,
                                          utillib_destroy_func_t *);

/* modifier */
int utillib_unordered_map_emplace(utillib_unordered_map *, utillib_key_t,
                                  utillib_value_t);
int utillib_unordered_map_insert(utillib_unordered_map *,
                                 utillib_pair_t const *);
int utillib_unordered_map_erase(utillib_unordered_map *, utillib_key_t);
void utillib_unordered_map_set_max_load_factor(utillib_unordered_map *, double);
void utillib_unordered_map_rehash(utillib_unordered_map *, size_t);
void utillib_unordered_map_clear(utillib_unordered_map *);

/* observer */
utillib_pair_t *utillib_unordered_map_find(utillib_unordered_map *,
                                           utillib_key_t);
utillib_unordered_map_ft utillib_unordered_map_const_charp_ft(void);
size_t utillib_unordered_map_size(utillib_unordered_map *);
size_t utillib_unordered_map_bucket_count(utillib_unordered_map *);
bool utillib_unordered_map_empty(utillib_unordered_map *);
double utillib_unordered_map_load_factor(utillib_unordered_map *);
#endif // UTILLIB_UNORDERED_MAP_H
