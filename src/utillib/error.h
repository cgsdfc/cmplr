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
#ifndef UTILLIB_ERROR_H
#define UTILLIB_ERROR_H
#include "enum.h"
#include "position.h"
#include "slist.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#define COLOR_STRING(C, S) COLOR_STRING_BOLD(C, S)
#define GREEN_CARET COLOR_STRING(GREEN, "^")
#define RED_ERROR COLOR_STRING(RED, "error:")
#define RED_FATAL COLOR_STRING(RED, "fatal:")
#define RED_ICE COLOR_STRING(RED, "internal compiler error:")
#define PURPLE_WARNING COLOR_STRING(PURPLE, "warning:")
#define CYAN_NOTE COLOR_STRING(CYAN, "note:")

UTILLIB_ENUM_BEGIN(utillib_error_lv)
    UTILLIB_ENUM_ELEM(ERROR_LV_OK)
    UTILLIB_ENUM_ELEM(ERROR_LV_NOTE)
    UTILLIB_ENUM_ELEM(ERROR_LV_WARNING)
    UTILLIB_ENUM_ELEM(ERROR_LV_ERROR)
    UTILLIB_ENUM_ELEM(ERROR_LV_FATAL)
    UTILLIB_ENUM_ELEM(ERROR_LV_ICE)
    UTILLIB_ENUM_END(utillib_error_lv)

typedef void (utillib_error_cleanup_func_t) (void);

typedef struct utillib_error {
	int ue_lv;
	utillib_position ue_pos;
	char *ue_msg;
} utillib_error;

void utillib_die(const char *);
utillib_error_cleanup_func_t
    *utillib_error_cleanup_func(utillib_error_cleanup_func_t *);
utillib_error *utillib_make_error(int, utillib_position *, char *);
void utillib_destroy_error(utillib_error *);
#endif				// UTILLIB_ERROR_H
