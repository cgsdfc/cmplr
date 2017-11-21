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
#ifndef UTILLIB_JSON_SCANNER_H
#define UTILLIB_JSON_SCANNER_H

#include "scanner.h"
#include "string.h"
#include "json_parser_impl.h"

struct utillib_json_scanner {
  struct utillib_string_scanner scanner;
  struct utillib_string buffer;
  size_t code;
};

extern const struct utillib_scanner_op utillib_json_scanner_op;
void utillib_json_scanner_init(struct utillib_json_scanner *self,
                               char const *str);
size_t utillib_json_scanner_lookahead(struct utillib_json_scanner *self);
void utillib_json_scanner_shiftaway(struct utillib_json_scanner *self);
void const *utillib_json_scanner_semantic(struct utillib_json_scanner *self);
void utillib_json_scanner_destroy(struct utillib_json_scanner *self);

#endif /* UTILLIB_JSON_SCANNER_H */
