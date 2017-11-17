#define _BSD_SOURCE
#include "strref.h"
#include "unordered_op.h"
#include <string.h>

/**
 * \function free_str_in_pair
 * Take the first of the `pair' as a string and dispose it by `free'.
 * \param pair the utillib_pair that holds the string.
 * \return void.
 */

static void free_str_in_pair(struct utillib_pair *pair) {
  free((void*)UTILLIB_PAIR_FIRST(pair));
}

/**
 * \function utillib_strref_init
 * Initializes the strref object.
 * \param self.
 */

void utillib_strref_init(struct utillib_strref *self) {
  utillib_unordered_map_init(&self->strref_map,
                             utillib_unordered_op_get_c_str());
}

/**
 * \function utillib_strref_incr
 * Increases the refcnt of `str' if it is present or `strdup' it and set
 * its refcnt to 1 if it is absent.
 * \param str The string to increase refcnt for.
 * \return The string to be shared. It is alway owned by self and
 * not necessarily has the same address as `str'.
 */

char const *utillib_strref_incr(struct utillib_strref *self, char const *str) {
  struct utillib_pair *pair =
      utillib_unordered_map_find(&self->strref_map, str);
  if (pair) {
    size_t cur_refcnt = (size_t)UTILLIB_PAIR_SECOND(pair);
    ++cur_refcnt;
    UTILLIB_PAIR_SECOND(pair) = (void const *)cur_refcnt;
    return UTILLIB_PAIR_FIRST(pair);
  }
  char const *dup = strdup(str);
  utillib_unordered_map_emplace(&self->strref_map, dup, (void const*)1);
  return dup;
}

/**
 * \function utillib_strref_decr
 * Decreases the refcnt of `str' and if it reaches zero, erases it from
 * the map. If the string with the same content has not been increased
 * refcnt for, returns silently.
 * \param str The string to decrease refcnt for.
 * \return void.
 */

void utillib_strref_decr(struct utillib_strref *self, char const *str) {
  struct utillib_pair *pair =
      utillib_unordered_map_find(&self->strref_map, str);
  if (pair) {
    size_t cur_refcnt = (size_t)UTILLIB_PAIR_SECOND(pair);
    if (cur_refcnt) {
      --cur_refcnt;
      UTILLIB_PAIR_SECOND(pair) = (void const*)(cur_refcnt);
      return;
    }
    utillib_unordered_map_erase(&self->strref_map, str);
  }
}

/**
 * \function utillib_strref_getcnt
 * Get the refcnt of `str'.
 * \param str The string we concern (mostly for debug issue).
 * \return 0 if no such a `str' inside self. Otherwise a positive value
 * no bigger than the maximum of size_t is returned.
 */

size_t utillib_strref_getcnt(struct utillib_strref *self, char const *str) {
  struct utillib_pair *pair =
      utillib_unordered_map_find(&self->strref_map, str);
  if (!pair) {
    return 0;
  }
  return (size_t)UTILLIB_PAIR_SECOND(pair);
}

/**
 * \function utillib_strref_destroy
 * Destructs self. Disposes all the owned string. Any shared
 * pointer to them is invalidated.
 * \param self.
 * return void.
 */
void utillib_strref_destroy(struct utillib_strref *self) {
  utillib_unordered_map_destroy_owning(
      &self->strref_map, (void*)free_str_in_pair);
}
