#define _BSD_SOURCE
#include "strref.h"
#include "typedef.h"
#include <string.h>

static void free_str_in_pair(utillib_pair_t *pair) {
  free((utillib_element_t)UTILLIB_PAIR_FIRST(pair));
}

void utillib_strref_init(utillib_strref *self) {
  utillib_unordered_map_init(&self->strref_map,
                             utillib_unordered_map_const_charp_ft());
}

char const *utillib_strref_incr(utillib_strref *self, char const *str) {
  utillib_pair_t *pair = utillib_unordered_map_find(&self->strref_map, str);
  if (pair) {
    size_t cur_refcnt = (size_t)UTILLIB_PAIR_SECOND(pair);
    ++cur_refcnt;
    UTILLIB_PAIR_SECOND(pair) = (utillib_value_t)cur_refcnt;
    return UTILLIB_PAIR_FIRST(pair);
  }
  char const *dup = strdup(str);
  utillib_unordered_map_emplace(&self->strref_map, dup, (utillib_value_t)0);
  return dup;
}

void utillib_strref_decr(utillib_strref *self, char const *str) {
  utillib_pair_t *pair = utillib_unordered_map_find(&self->strref_map, str);
  if (pair) {
    size_t cur_refcnt = (size_t)UTILLIB_PAIR_SECOND(pair);
    if (cur_refcnt) {
      --cur_refcnt;
      UTILLIB_PAIR_SECOND(pair) = (utillib_value_t)(cur_refcnt);
      return;
    }
    utillib_unordered_map_erase(&self->strref_map, str);
  }
}

void utillib_strref_destroy(utillib_strref *self) {
  utillib_unordered_map_destroy_owning(&self->strref_map,
      (utillib_destroy_func_t*)free_str_in_pair);
}
