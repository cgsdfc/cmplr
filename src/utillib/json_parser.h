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

#ifndef UTILLIB_JSON_PARSER_H
#define UTILLIB_JSON_PARSER_H
#include "json.h"
#include "ll1_parser.h"
#include "vector.h"

UTILLIB_ENUM_BEGIN(utillib_json_parser_error_kind)
UTILLIB_ENUM_ELEM(JSON_ESTRING)
UTILLIB_ENUM_ELEM(JSON_EESCAPE)
UTILLIB_ENUM_ELEM(JSON_EUNKNOWN)
UTILLIB_ENUM_END(utillib_json_parser_error_kind);

struct utillib_json_parser_factory {
  struct utillib_ll1_factory factory;
};

struct utillib_json_parser {
  struct utillib_ll1_parser parser;
  struct utillib_vector values;
};

void utillib_json_parser_factory_init(struct utillib_json_parser_factory *self);

void utillib_json_parser_factory_destroy(
    struct utillib_json_parser_factory *self);

void utillib_json_parser_init(struct utillib_json_parser *self,
                              struct utillib_json_parser_factory *factory);

void utillib_json_parser_destroy(struct utillib_json_parser *self);

struct utillib_json_value *
utillib_json_parser_parse(struct utillib_json_parser *self, char const *str);

bool utillib_json_parser_parse_dbg(struct utillib_json_parser *self,
                                   size_t const *input);
#endif /* UTILLIB_JSON_PARSER_H */
