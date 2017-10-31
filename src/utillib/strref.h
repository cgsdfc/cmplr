#ifndef UTILLIB_STRREF_H
#define UTILLIB_STRREF_H
#include "unordered_map.h"
/**
 * \file utillib/strref.h
 * \brief Reference counted string.
 * <para> Constant strings can be quite long and it is a waste of memory
 * if we keep all the copies of them. Strref addresses this problem
 * by owning those constant strings itself and provides a immutable
 * view to client, who just keep a pointer to those shared strings.
 * It frees client from managing the life time of those strings.
 * </para>
 * <para>
 * Its usage should be a module or program scope pooling of strings such
 * as string literals or identifiers collected from source code.
 * Predefined language reserved words are better put into a perfect hash
 * map with static storage as they need no dynamic allocation.
 * </para>
 * <para>
 * Strref can be used as a dropin replacement of `strdup-free' pair
 * with less memory need.
 * </para>
 */

typedef struct utillib_strref {
	utillib_unordered_map strref_map;
} utillib_strref;

void utillib_strref_init(utillib_strref *);
char const *utillib_strref_incr(utillib_strref *, char const *);
void utillib_strref_decr(utillib_strref *, char const *);
void utillib_strref_destroy(utillib_strref *);
size_t utillib_strref_getcnt(utillib_strref *, char const *);
#endif				// UTILLIB_STRREF_H
