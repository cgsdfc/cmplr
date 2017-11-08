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
#ifndef UTILLIB_TYPEDEF_H
#define UTILLIB_TYPEDEF_H
#include <stdbool.h>
#include <stddef.h>

/* Common typedef */
typedef void(utillib_destroy_func_t)(void *);
typedef void *utillib_element_t;
typedef utillib_element_t *utillib_element_pointer_t;

/* Utllib.Vector */
typedef struct utillib_vector utillib_vector;
/* Utillib.Slist */
typedef struct utillib_slist utillib_slist;
/* Utllib.String */
typedef struct utillib_string utillib_string;

/* Utillib.Unordered_map */
typedef void const *utillib_key_t;
typedef void *utillib_value_t;
typedef size_t(utillib_hash_func_t)(utillib_key_t);
typedef bool(utillib_equal_func_t)(utillib_key_t, utillib_key_t);

/* Utillib.Test */
typedef struct utillib_test_predicate_t utillib_test_predicate_t;
typedef struct utillib_test_entry_t utillib_test_entry_t;
typedef struct utillib_test_env_t utillib_test_env_t;
typedef void *utillib_test_fixture_t;

/* Defined by client as a single test */
typedef void(utillib_test_func_t)(utillib_test_entry_t *,
                                  utillib_test_fixture_t);
typedef void(utillib_test_fixfunc_t)(utillib_test_fixture_t);
/* Returns an registered `utillib_test_entry_t' */
typedef utillib_test_env_t *(utillib_test_getenv_func_t)(void);

/* Utillib.Print */
typedef struct utillib_printer_t utillib_printer_t;
typedef char const *(utillib_print_tostring_func_t)(utillib_element_t);

/* Utillib.JSON */
typedef struct utillib_json_value_t utillib_json_value_t;
typedef struct utillib_json_object_t utillib_json_object_t;
typedef struct utillib_json_array_t utillib_json_array_t;
typedef utillib_json_value_t *(utillib_json_value_create_func_t)(void const*,
                                                                 size_t);

#endif /* UTILLIB_TYPEDEF_H */
