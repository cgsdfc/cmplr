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
#ifndef UTILLIB_TEST_IMPL_H
#define UTILLIB_TEST_IMPL_H

#include "color.h"

/**
 * Colorful status bar for display.
 */

#define WHITE_SKIP       "[  SKIPPED ]"

#ifndef WITH_WINDOWS

#define GREEN_BANG       COLOR_STRING_UNBOLD(GREEN, "[==========]")
#define GREEN_DASH       COLOR_STRING_UNBOLD(GREEN, "[----------]")
#define RED_DASH         COLOR_STRING_UNBOLD(RED, "[----------]")
#define GREEN_RUN        COLOR_STRING_UNBOLD(GREEN, "[ RUN      ]")
#define GREEN_OK         COLOR_STRING_UNBOLD(GREEN, "[       OK ]")
#define RED_BAD          COLOR_STRING_UNBOLD(RED, "[ BAD      ]")

#else /* Be less colorful */

#define GREEN_BANG "[==========]"
#define GREEN_DASH "[----------]"
#define RED_DASH "[----------]"
#define GREEN_RUN "[ RUN      ]"
#define GREEN_OK "[       OK ]"
#define RED_BAD "[ BAD      ]"

#endif /* WITH_WINDOWS */

struct utillib_test_suite;
struct utillib_json_value;

struct utillib_json_value const *
utillib_test_suite_json_object_create(struct utillib_test_suite const *self);

#endif /* UTILLIB_TEST_IMPL_H */
