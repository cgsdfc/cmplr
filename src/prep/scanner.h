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
#ifndef PREP_SCANNER_H
#define PREP_SCANNER_H
#include <scanner/scanner.h>	// for scanner_base_t
#include <scanner/token.h>	// scanner_token
#include <stdio.h>
#include <utillib/vector.h>	// for utiilib_vector

typedef struct prep_scanner_t {
	scanner_base_t psp_scan;
	scanner_input_buf psp_buf;
	utillib_vector *psp_err;
} prep_scanner_t;

void prep_scanner_init(prep_scanner_t *, FILE *, char const *,
		       utillib_vector *);
int prep_scanner_yylex(prep_scanner_t *);
void prep_scanner_destroy(prep_scanner_t *);
char const *prep_scanner_get_text(prep_scanner_t *);
#endif				// PREP_SCANNER_H
