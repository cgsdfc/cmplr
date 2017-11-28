/*
   Cmplr Library
   Copyright (C) 2017-2018 Cong Feng <cgsdfc@126.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301 USA

*/

#ifndef UTILLIB_JSON_FOREACH_H
#define UTILLIB_JSON_FOREACH_H
#include "json.h"
#include "json_impl.h"
#include "vector.h"
#include "pair.h"

/**
 * File json_foreach.h
 * When you need to traversal an json array or an object,
 * you must somehow expose the internal of these object,
 * i.e., their vectors.
 * These level of exposure differs from common usage of json,
 * i.e, tostring, so separate here.
 * Notes by default, there macros do **no checking** for the types
 * of jsons. You can define `JSON_CHECK_FOREACH' to insert a check(assertion)
 * before the conversion is done.
 */

#ifndef JSON_CHECK_FOREACH
#define UTILLIB_JSON_ARRAY_FOREACH(VALUE, ARRAY) \
UTILLIB_VECTOR_FOREACH(struct utillib_json_value*, (VALUE), _JSON_ARRAY(ARRAY))

#define UTILLIB_JSON_OBJECT_FOREACH(PAIR, OBJECT) \
UTILLIB_VECTOR_FOREACH(struct utillib_pair*, (PAIR), _JSON_OBJECT(OBJECT))
#else
#define UTILLIB_JSON_ARRAY_FOREACH(VALUE, ARRAY) \
json_value_check_kind(ARRAY, UT_JSON_ARRAY); \
UTILLIB_VECTOR_FOREACH(struct utillib_json_value*, (VALUE), _JSON_ARRAY(ARRAY))

#define UTILLIB_JSON_OBJECT_FOREACH(PAIR, OBJECT) \
json_value_check_kind(OBJECT, UT_JSON_OBJECT); \
UTILLIB_VECTOR_FOREACH(struct utillib_json_value*, (VALUE), _JSON_OBJECT(OBJECT))
#endif /* JSON_CHECK_FOREACH */

#endif /* UTILLIB_JSON_FOREACH_H */
